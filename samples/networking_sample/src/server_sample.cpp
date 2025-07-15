#include "server.hpp"
#include <atomic>
#include <iostream>
#include <thread>

using namespace chess::networking;

int main()
{
    server s( 8080 );
    if ( !s.open() ) {
        std::cerr << "Failed to start server.\n";
        return 1;
    }

    std::cout << "[server] Listening on port 8080...\n";
    std::atomic< bool > running( true );

    // Receiving thread
    std::thread receiver( [&]() {
        while ( running ) {
            std::string msg = s.read();
            if ( !msg.empty() ) {
                std::cout << "[client] " << msg << std::endl;
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

        if ( !s.write( message ) ) {
            std::cerr << "Failed to send message.\n";
        }
    }

    receiver.join();
    s.close();
    return 0;
}