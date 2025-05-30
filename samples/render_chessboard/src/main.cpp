#include <functional>
#include <imgui_initializer.hpp>
#include <thread>

#include <display.hpp>

int main()
{
    chess::display::imgui_initializer window( "DearIMGUI Sample" );

    // chess::display

    chess::controller::display controller( 1280, 720 );

    auto func = std::function< void() >( [&controller]() { controller.render(); } );

    std::thread loopy( [&window, &func]() { window.run( func ); } );

    if ( loopy.joinable() )
        loopy.join();
}