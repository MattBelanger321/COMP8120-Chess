#include <client_display.hpp>
#include <imgui_initializer.hpp>
#include <iostream>
int main()
{
    try {
        chess::display::imgui_initializer window( "COMP 8120 Chess Demo", 1200, 900 );

        // chess::display
        chess::networking::component_data board{
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
        chess::networking::component_data status{
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
        chess::networking::component_data control{
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
        auto                              url = "wss://localhost:8080";
        chess::networking::client_display cli( board, status, control );
        auto                              func = std::function< void() >( [&cli]() { cli.render(); } );
        std::thread                       loopy( [&window, &func]() { window.run( func ); } );
        if ( loopy.joinable() )
            loopy.join();
    }
    catch ( const std::exception & e ) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}