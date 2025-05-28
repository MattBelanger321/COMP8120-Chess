#include <functional>
#include <imgui_initializer.hpp>
#include <thread>

int main()
{
    chess::display::imgui_initalizer window( "DearIMGUI Sample" );

    auto func = std::function< void() >( []() {} );

    std::thread loopy( [&window, &func]() { window.run( func ); } );

    if ( loopy.joinable() )
        loopy.join();
}