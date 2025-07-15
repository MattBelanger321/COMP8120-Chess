#include "client.hpp"
#include <atomic>
#include <iostream>
#include <thread>

using namespace chess::networking;

int main()
{
    client c( "ws://localhost:8080" );
    if ( !c.open() ) {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    std::cout << "[client] Connected to server!\n";
    std::atomic< bool > running( true );

    // Receiving thread
    std::thread receiver( [&]() {
        while ( running ) {
            std::string msg = c.read();
            if ( !msg.empty() ) {
                std::cout << "[server] " << msg << std::endl;
            }
        }
    } );

    // Sending thread
    while ( running ) {
        std::cout << "[you] > ";
        std::string message;
        std::getline( std::cin, message );

        if ( message == "/quit" ) {
            running = false;
            break;
        }

        if ( !c.write( message ) ) {
            std::cerr << "Failed to send message.\n";
        }
    }

    receiver.join();
    c.close();
    return 0;
}