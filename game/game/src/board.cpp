#include "bishop.hpp"
#include "king.hpp"
#include "knight.hpp"
#include "pawn.hpp"
#include "queen.hpp"
#include "rook.hpp"
#include <board.hpp>
#include <memory>
#include <sstream>

namespace chess::game {

    board::board( bool const empty )
    {
        for ( int i = 1; i <= 8; i++ ) {
            file_t file_map;
            auto   rank = static_cast< pieces::rank_t >( i );
            for ( int j = 1; j <= 8; j++ ) {
                auto file = static_cast< pieces::file_t >( j );

                file_map.emplace( file, space( ( i + j ) % 2 == 1, rank, file ) );
            }
            game_board[rank] = std::move( file_map );
        }

        if ( !empty )
            place_pieces();
    }

    std::string board::to_string()
    {
        std::stringstream serialized;

        for ( int i = 8; i > 0; i-- ) {
            serialized << "|";
            for ( int j = 1; j <= 8; j++ ) {
                auto rank = static_cast< pieces::rank_t >( i );
                auto file = static_cast< pieces::file_t >( j );

                space & current_space = game_board.at( rank ).at( file );

                if ( current_space.piece ) {
                    char piece_icon;

                    switch ( current_space.piece->type() ) {
                    case pieces::name_t::rook:
                        piece_icon = 'r';
                        break;
                    case pieces::name_t::knight:
                        piece_icon = 'n';
                        break;
                    case pieces::name_t::bishop:
                        piece_icon = 'b';
                        break;
                    case pieces::name_t::king:
                        piece_icon = 'k';
                        break;
                    case pieces::name_t::queen:
                        piece_icon = 'q';
                        break;
                    case pieces::name_t::pawn:
                        piece_icon = 'p';
                        break;
                    }

                    if ( current_space.piece->colour() ) {
                        piece_icon -= 32;  // convert to upper case
                    }

                    serialized << piece_icon << "|";
                }
                else {
                    if ( current_space.colour() ) {
                        serialized << "0|";
                    }
                    else {
                        serialized << "1|";
                    }
                }
            }
            serialized << "\n";
        }
        return serialized.str();
    }

    void board::place_pieces()
    {

        auto rank   = static_cast< pieces::rank_t >( 1 );
        bool colour = true;
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            switch ( file ) {
            case pieces::file_t::one:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            case pieces::file_t::two:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::three:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::four:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::queen >( colour, rank, file );
                break;
            case pieces::file_t::five:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::king >( colour, rank, file );
                break;
            case pieces::file_t::six:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::seven:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::eight:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            }
        }

        colour = false;
        rank   = static_cast< pieces::rank_t >( 8 );
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            switch ( file ) {
            case pieces::file_t::one:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            case pieces::file_t::two:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::three:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::four:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::queen >( colour, rank, file );
                break;
            case pieces::file_t::five:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::king >( colour, rank, file );
                break;
            case pieces::file_t::six:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::seven:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::eight:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            }
        }

        rank   = static_cast< pieces::rank_t >( 2 );
        colour = true;
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            game_board.at( rank ).at( file ).piece = std::make_unique< pieces::pawn >( colour, rank, file );
        }

        colour = false;
        rank   = static_cast< pieces::rank_t >( 7 );
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            game_board.at( rank ).at( file ).piece = std::make_unique< pieces::pawn >( colour, rank, file );
        }
    }

}  // namespace chess::game