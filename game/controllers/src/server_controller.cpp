#include "controller.hpp"
#include "piece.hpp"
#include "server.hpp"
#include "space.hpp"
#include <exception>
#include <iostream>
#include <mutex>
#include <server_controller.hpp>
#include <sstream>
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
    }

    void server_controller::listen()
    {
        while ( !should_close.test() ) {
            auto read = server.read();
            if ( read.starts_with( networking::update_board_command ) ) {
                update_board_handler();
            }
            else if ( read.starts_with( networking::possible_moves_command ) ) {
                std::string move = std::string( read.begin() + networking::possible_moves_command.size(), read.end() );

                if ( move.size() != 2 ) {
                    server.write( "" );
                    std::cout << "Invalid Move Received: " << move << "\n";
                }

                std::cout << "Checking Possible Moves For: " << move << "\n";

                possible_moves_handler( move );
            }
            else if ( read.starts_with( networking::move_command ) ) {
                std::string moves = std::string( read.begin() + networking::move_command.size(), read.end() );

                if ( moves.size() != 4 ) {
                    server.write( "" );
                    std::cout << "Invalid Move Received: " << moves << "\n";
                }

                move_handler( moves );
            }
            else if ( read.starts_with( networking::status_command ) ) {
                server.write( networking::status_command + to_string( game.get_state() ) );
            }
            else {
                std::cout << "ERROR\n";
            }
        }
        std::cout << "Listen() Closed\n";
    }

    void server_controller::update_board_handler()
    {
        try {
            std::lock_guard guard( game_mutex );
            server.write( networking::update_board_command + game.get_board().to_string() );
        }
        catch ( std::exception const & e ) {
            std::cout << "Message Could not be deserialized: " << e.what() << "\n";
        }
    }

    pieces::position_t to_pos( char const & file, char const & rank )
    {
        auto pos = pieces::piece::itopos( 1 + rank - '1', 1 + file - 'A' );

        if ( pos.has_value() ) {
            return *pos;
        }
        else {
            throw std::runtime_error( std::string( "Non Position String:" ) + file + rank );
        }
    }

    void server_controller::move_handler( std::string const & moves )
    {
        auto src = to_pos( moves[0], moves[1] );
        auto dst = to_pos( moves[2], moves[3] );

        auto status = game.move( game.get( src ), game.get( dst ) );

        if ( status != pieces::move_status::valid ) {
            server.write( "" );
        }
        else {
            server.write( networking::move_command );
        }
    }

    void server_controller::possible_moves_handler( std::string const & move )
    {
        std::vector< game::space > possible_moves;
        possible_moves.reserve( 64 );

        try {
            {
                std::lock_guard guard( game_mutex );
                game.possible_moves( game.get( to_pos( move[0], move[1] ) ), possible_moves );
            }

            std::stringstream ss;
            for ( auto & sp : possible_moves ) {
                ss << to_string( sp.position() );
            }

            server.write( networking::possible_moves_command + ss.str() );
        }
        catch ( std::exception const & e ) {
            server.write( "" );
            std::cout << e.what() << std::endl;
        }
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