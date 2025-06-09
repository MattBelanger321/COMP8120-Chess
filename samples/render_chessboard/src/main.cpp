#include <functional>
#include <imgui_initializer.hpp>
#include <thread>

#include <display.hpp>

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

    if ( loopy.joinable() )
        loopy.join();
}