#include "controller.hpp"
#include <exception>
#include <iostream>
#include <mutex>
#include <server_controller.hpp>
#include <stdexcept>
#include <thread>

namespace chess::controller {
    server_controller::server_controller( int port ) : controller(), server( port ) { start_listening(); }

    void server_controller::start_listening()
    {
        should_close.clear();

        if ( listener_thread.joinable() ) {
            std::cout << "Already Listening, Cannot create thread\n";
            return;
        }

        if ( !server.open() ) {
            std::cerr << "Failed to start server.\n";
            throw std::runtime_error( "Error opening server" );
        }

        std::cout << "Started Server\n";

        listener_thread = std::thread( [this]() { listen(); } );
        sender_thread   = std::thread( [this]() { send(); } );
    }

    void server_controller::listen()
    {
        while ( !should_close.test() ) {

            std::string board;
            {
                std::lock_guard guard( game_mutex );
                board = game.to_string();
            }
            auto read = server.read();

            try {
                std::lock_guard guard( game_mutex );
                game.load_from_string( read );
            }
            catch ( std::exception const & e ) {
                std::cout << "Message Could not be deserialized: " << e.what() << "\nReceived:\n" << read << "\n";
            }
        }
        std::cout << "Listen() Closed\n";
    }

    void server_controller::send()
    {
        using namespace std::chrono_literals;
        while ( !should_close.test() ) {
            std::string board;
            {
                std::lock_guard guard( game_mutex );
                board = game.to_string();
            }
            if ( !server.write( board ) ) {
                std::cout << "Error Sending Board\n";
            }
            std::this_thread::sleep_for( 1s );
        }
        std::cout << "Send() Closed\n";
    }

    void server_controller::close()
    {
        should_close.test_and_set();
        if ( listener_thread.joinable() ) {
            listener_thread.join();
        }
        if ( sender_thread.joinable() ) {
            sender_thread.join();
        }

        std::cout << "Server Closed\n";
    }

    server_controller::~server_controller()
    {
        server.close();
        close();
    }
}  // namespace chess::controller