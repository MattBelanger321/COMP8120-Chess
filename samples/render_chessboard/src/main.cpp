#include "ai_controller.hpp"
#include <fstream>
#include <functional>
#include <imgui_initializer.hpp>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
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
    chess::display::imgui_initializer window( "COMP 8120 Chess Demo", 1200, 900 );

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

    chess::controller::display controller( board, status, control );

    auto func = std::function< void() >( [&controller]() { controller.render(); } );

    std::thread loopy( [&window, &func]() { window.run( func ); } );

    nlohmann::json chromie_json = nlohmann::json::parse( std::ifstream( "chromosome.json" ) );

    chess::controller::chromosome_t chromie( chromie_json["chromosome"].get< std::vector< float > >() );

    chess::controller::ai_controller ai( chromie );

    ai.activate();

    if ( loopy.joinable() )
        loopy.join();
}