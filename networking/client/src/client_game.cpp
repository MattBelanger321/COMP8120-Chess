#include "piece.hpp"
#include <client_game.hpp>
#include <exception>
#include <iostream>
#include <mutex>
#include <stdexcept>

namespace chess::networking {

    client_game::client_game( std::string const & url ) : cli( url ) { cli.open(); }

    void client_game::select_space( game::space const & sp ) { selected_space = sp; }

    void client_game::update_board()
    {
        std::string msg;
        {
            std::lock_guard cli_guard( client_mutex );
            if ( !cli.write( update_board_command ) ) {
                std::cout << "Unable to send message\n";
            }
            msg = cli.read();
        }

        {
            std::lock_guard board_guard( board_mutex );
            if ( !msg.starts_with( update_board_command ) ) {
                std::cout << "INVALID RESPONSE TO " << update_board_command << "\n";
                return;
            }

            game_board.load_from_string( msg );
        }
    }

    std::vector< game::space > client_game::possible_moves( game::space const & sp )
    {

        std::string msg;
        {
            std::lock_guard cli_guard( client_mutex );
            if ( !cli.write( possible_moves_command + to_string( sp.position() ) ) ) {
                std::cout << "Unable to send message\n";
            }
            msg = cli.read();
        }

        if ( !msg.starts_with( possible_moves_command ) ) {
            std::cout << "INVALID RESPONSE TO " << possible_moves_command << "\n";
            return {};
        }

        auto moves = string_to_spaces( std::string( msg.begin() + possible_moves_command.size(), msg.end() ) );

        for ( auto & mv : moves ) {
            std::cout << to_string( mv.position() ) << ", ";
        }
        std::cout << "\n";
        return moves;
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

    std::vector< game::space > client_game::string_to_spaces( std::string const & str )
    {
        std::vector< game::space > ret;
        ret.reserve( 64 );

        if ( str.size() % 2 != 0 ) {
            throw std::runtime_error( "Possible Moves Response is not even length: " + str );
        }

        {
            std::lock_guard board_guard( board_mutex );
            for ( int i = 0; i < str.size(); i += 2 ) {
                auto pos = to_pos( str[i], str[i + 1] );

                ret.push_back( game_board.get( pos ) );
            }
        }

        return ret;
    }

}  // namespace chess::networking