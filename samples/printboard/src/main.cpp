#include <board.hpp>
#include <ci_controller.hpp>
#include <controller.hpp>
#include <csignal>
#include <piece.hpp>
#include <space.hpp>

#include <iostream>

std::atomic_flag should_close;  // global flag for the signal handler

void signal_handle( int signum )
{
    std::cout << "\nRECEIVED SHUTDOWN COMMAND - PRESS x TO EXIT\n";
    should_close.test_and_set();
    should_close.notify_one();
}

void wait_until_exit()
{
    while ( !should_close.test() ) {
        should_close.wait( false );
    }
}

int main()
{
    std::signal( SIGINT, signal_handle );

    chess::controller::ci_controller cont;
    cont.run();

    wait_until_exit();
}