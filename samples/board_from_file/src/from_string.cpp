#include <ai_controller.hpp>
#include <fstream>
#include <functional>
#include <imgui_initializer.hpp>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <server_controller.hpp>
#include <thread>

#include <display.hpp>

#include <nlohmann/json.hpp>
#include <vector>

std::atomic_flag should_close;  // global flag for the signal handler

void signal_handle( int signum )
{
    std::cout << "\nRECEIVED SHUTDOWN COMMAND - PRESS x TO EXIT\n";
    should_close.test_and_set();
    should_close.notify_one();
}

int main()
{
    try {
        chess::display::imgui_initializer window( "Chess Server", 1200, 900 );

        // chess::display
        chess::controller::component_data board{
            .size =
                {
                    .width  = 900,
                    .height = 900,
                },
            .pos =
                {
                    .x = 0,
                    .y = 0,
                },
        };
        chess::controller::component_data status{
            .size =
                {
                    .width  = 300,
                    .height = 450,
                },
            .pos =
                {
                    .x = board.size.width,
                    .y = 0,
                },
        };
        chess::controller::component_data control{
            .size =
                {
                    .width  = 300,
                    .height = 450,
                },
            .pos =
                {
                    .x = board.size.width,
                    .y = status.size.height,
                },
        };

        // read sample board state
        std::ifstream file( "../board_position.txt" );
        if ( !file ) {
            std::cerr << "Failed to open board_position.txt\n";
            return 1;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();  // Read entire file into buffer
        std::string contents = buffer.str();

        chess::controller::display controller( board, status, control, contents );
        auto                       func = std::function< void() >( [&controller]() { controller.render(); } );
        std::thread                loopy( [&window, &func]() { window.run( func ); } );

        chess::controller::server_controller server( 8080 );

        if ( loopy.joinable() )
            loopy.join();
    }
    catch ( const std::exception & e ) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}