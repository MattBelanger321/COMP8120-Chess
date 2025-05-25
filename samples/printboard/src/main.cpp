#include <board.hpp>
#include <ci_controller.hpp>
#include <controller.hpp>
#include <piece.hpp>
#include <space.hpp>

#include <iostream>

int main()
{
    chess::controller::ci_controller cont;
    cont.run();
    while ( true )
        ;
}