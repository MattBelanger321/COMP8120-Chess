#include <functional>
#include <imgui_initializer.hpp>
#include <thread>

#include <display.hpp>

int main()
{
    chess::display::imgui_initializer window( "COMP 8120 Chess Demo", 1200, 900 );

    // chess::display

    chess::controller::display controller( 720, 900 );

    auto func = std::function< void() >( [&controller]() { controller.render(); } );

    std::thread loopy( [&window, &func]() { window.run( func ); } );

    if ( loopy.joinable() )
        loopy.join();
}