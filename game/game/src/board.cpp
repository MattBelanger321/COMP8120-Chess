#include "space.hpp"
#include <bishop.hpp>
#include <board.hpp>
#include <cstdlib>
#include <king.hpp>
#include <knight.hpp>
#include <pawn.hpp>
#include <piece.hpp>
#include <queen.hpp>
#include <rook.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

namespace chess::game {

    board::board( bool const empty ) { reset( empty ); }

    void board::reset( bool const empty )
    {
        game_board = {};
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

    pieces::move_status board::move_force( pieces::position_t const & src, pieces::position_t const & dst )
    {
        if ( !game_board.at( src.first ).at( src.second ).piece )
            return pieces::move_status::no_piece_to_move;

        auto status = game_board.at( src.first ).at( src.second ).piece->move( dst );

        if ( !is_success_status( status ) ) {
            return status;
        }

        game_board.at( dst.first )
            .at( dst.second )
            .piece.reset( game_board.at( src.first ).at( src.second ).piece.release() );

        // move the rook for castle
        if ( status == pieces::move_status::king_side_castle_white ) {
            auto & from = game_board.at( pieces::rank_t::one ).at( pieces::file_t::h ).piece;
            if ( from ) {
                from->place( { pieces::rank_t::one, pieces::file_t::f } );
            }
            game_board.at( pieces::rank_t::one ).at( pieces::file_t::f ).piece.reset( from.release() );
            move_history.push_back( "O-O" );

            return pieces::move_status::valid;
        }
        else if ( status == pieces::move_status::king_side_castle_black ) {
            auto & from = game_board.at( pieces::rank_t::eight ).at( pieces::file_t::h ).piece;
            if ( from ) {
                from->place( { pieces::rank_t::eight, pieces::file_t::f } );
            }
            game_board.at( pieces::rank_t::eight ).at( pieces::file_t::f ).piece.reset( from.release() );
            move_history.push_back( "O-O" );

            return pieces::move_status::valid;
        }
        else if ( status == pieces::move_status::queen_side_castle_white ) {
            auto & from = game_board.at( pieces::rank_t::one ).at( pieces::file_t::a ).piece;
            if ( from ) {
                from->place( { pieces::rank_t::one, pieces::file_t::d } );
            }
            game_board.at( pieces::rank_t::one ).at( pieces::file_t::d ).piece.reset( from.release() );
            move_history.push_back( "O-O-O" );

            return pieces::move_status::valid;
        }
        else if ( status == pieces::move_status::queen_side_castle_black ) {
            auto & from = game_board.at( pieces::rank_t::eight ).at( pieces::file_t::a ).piece;
            if ( from ) {
                from->place( { pieces::rank_t::eight, pieces::file_t::d } );
            }
            game_board.at( pieces::rank_t::eight ).at( pieces::file_t::d ).piece.reset( from.release() );
            move_history.push_back( "O-O-O" );

            return pieces::move_status::valid;
        }

        // pawn promotion
        auto & dst_piece = get( dst ).piece;
        if ( dst_piece && dst_piece->type() == pieces::name_t::pawn ) {
            if ( dst.first == pieces::rank_t::eight || dst.first == pieces::rank_t::one ) {
                ( *this )[dst.first].at( dst.second ).piece = std::make_unique< pieces::queen >(
                    dst_piece->colour(), dst_piece->position().first, dst_piece->position().second );

                move_history.push_back( pieces::to_string( dst ) + "=Q" );
                return pieces::move_status::valid;
            }
        }
        move_history.push_back( pieces::to_string( src ) + pieces::to_string( dst ) );
        return pieces::move_status::valid;
    }

    pieces::move_status board::move( pieces::position_t const & src, pieces::position_t const & dst )
    {
        return move_force( src, dst );
    }

    std::vector< space > board::possible_moves( space const & src ) const
    {
        auto positions = src.possible_moves();  //

        if ( positions.empty() ) {
            return {};
        }

        std::vector< space > spaces;

        for ( auto const & pos : positions ) {
            spaces.push_back( game_board.at( pos.first ).at( pos.second ) );
        }

        switch ( src.piece->type() ) {
        case pieces::name_t::rook:
            filter_rook_moves( src, spaces );
            break;
        case pieces::name_t::knight:
            filter_knight_moves( src, spaces );
            break;
        case pieces::name_t::bishop:
            filter_bishop_moves( src, spaces );
            break;
        case pieces::name_t::king:
            filter_king_moves( src, spaces );
            break;
        case pieces::name_t::queen:
            filter_queen_moves( src, spaces );
            break;
        case pieces::name_t::pawn:
            filter_pawn_moves( src, spaces );
            break;
        }

        return spaces;
    }

    void board::remove_piece_at( pieces::position_t const position )
    {
        remove_piece_at( ( *this ).game_board, position );
    }
    void board::remove_piece_at( rank_t & board, pieces::position_t const position )
    {
        board.at( position.first ).at( position.second ).piece.reset();
    }

    void board::add_piece_at( pieces::piece const & p, pieces::position_t const position )
    {
        add_piece_at( ( *this ).game_board, p, position );
    }
    void board::add_piece_at( rank_t & board, pieces::piece const & p, pieces::position_t const position )
    {
        board.at( position.first ).at( position.second ).piece = pieces::piece::copy_piece( p );
    }

    std::vector< std::string > board::get_move_history() const { return move_history; }

    void board::filter_pawn_moves( space const & current, std::vector< space > & moves ) const
    {
        if ( !current.piece ) {
            return;
        }

        auto piece_colour = current.piece->colour();

        auto x = std::erase_if( moves, [&current, &piece_colour, this]( space const & move ) {
            if ( std::abs( static_cast< int >( current.position().second ) -
                           static_cast< int >( move.position().second ) ) == 1 ) {
                return !move.piece || move.piece->colour() == current.piece->colour();
            }
            else {
                if ( std::abs( static_cast< int >( current.position().first ) -
                               static_cast< int >( move.position().first ) ) >= 1 ) {
                    if ( move.piece ) {
                        return true;
                    }
                    else if ( std::abs( static_cast< int >( current.position().first ) -
                                        static_cast< int >( move.position().first ) ) == 2 ) {
                        auto rank_val = static_cast< int >( move.position().first );
                        auto file_val = static_cast< int >( move.position().second );
                        if ( piece_colour ) {  // if the piece is white

                            if ( static_cast< int >( current.position().first ) != 2 ) {
                                return true;  // pawn has moved
                            }

                            bool piece_infront = game_board.at( pieces::rank_t( rank_val - 1 ) )
                                                     .at( pieces::file_t( file_val ) )
                                                     .piece.get();
                            return piece_infront;
                        }
                        else {  // if it is black

                            if ( static_cast< int >( current.position().first ) != 7 ) {
                                return true;  // pawn has moved
                            }

                            bool piece_infront = game_board.at( pieces::rank_t( rank_val + 1 ) )
                                                     .at( pieces::file_t( file_val ) )
                                                     .piece.get();
                            return piece_infront;
                        }
                    }
                }
            }
            return false;
        } );
    }

    void board::filter_knight_moves( space const & current, std::vector< space > & moves ) const
    {
        if ( !current.piece ) {
            return;
        }

        auto x = std::erase_if( moves, [&current]( space const & move ) {
            return move.piece && move.piece->colour() == current.piece->colour();
        } );
    }

    void board::filter_bishop_moves( space const & current, std::vector< space > & moves ) const
    {
        auto rank_val = static_cast< int >( current.position().first );
        auto file_val = static_cast< int >( current.position().second );

        int left_boundary   = static_cast< int >( file_val ) - 1;
        int right_boundary  = 8 - static_cast< int >( file_val );
        int bottom_boundary = static_cast< int >( rank_val ) - 1;
        int top_boundary    = 8 - static_cast< int >( rank_val );

        std::vector< pieces::position_t > to_delete;

        bool blocked = false;
        for ( int i = 1; i <= right_boundary && i <= top_boundary; i++ ) {
            if ( blocked ) {
                auto const & space = game_board.at( static_cast< pieces::rank_t >( rank_val + i ) )
                                         .at( static_cast< pieces::file_t >( file_val + i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece = game_board.at( static_cast< pieces::rank_t >( rank_val + i ) )
                                     .at( static_cast< pieces::file_t >( file_val + i ) )
                                     .piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= left_boundary && i <= top_boundary; i++ ) {
            if ( blocked ) {
                auto const & space = game_board.at( static_cast< pieces::rank_t >( rank_val + i ) )
                                         .at( static_cast< pieces::file_t >( file_val - i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece = game_board.at( static_cast< pieces::rank_t >( rank_val + i ) )
                                     .at( static_cast< pieces::file_t >( file_val - i ) )
                                     .piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= left_boundary && i <= bottom_boundary; i++ ) {
            if ( blocked ) {
                auto const & space = game_board.at( static_cast< pieces::rank_t >( rank_val - i ) )
                                         .at( static_cast< pieces::file_t >( file_val - i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece = game_board.at( static_cast< pieces::rank_t >( rank_val - i ) )
                                     .at( static_cast< pieces::file_t >( file_val - i ) )
                                     .piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= right_boundary && i <= bottom_boundary; i++ ) {
            if ( blocked ) {
                auto const & space = game_board.at( static_cast< pieces::rank_t >( rank_val - i ) )
                                         .at( static_cast< pieces::file_t >( file_val + i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece = game_board.at( static_cast< pieces::rank_t >( rank_val - i ) )
                                     .at( static_cast< pieces::file_t >( file_val + i ) )
                                     .piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        // game class will handle check and castling logic
        auto x = std::erase_if( moves, [&to_delete]( space const & move ) {
            for ( auto del : to_delete ) {
                if ( del == move ) {
                    return true;
                }
            }
            return false;
        } );
    }

    void board::filter_rook_moves( space const & current, std::vector< space > & moves ) const
    {
        auto rank_val = static_cast< int >( current.position().first );
        auto file_val = static_cast< int >( current.position().second );

        int left_boundary   = static_cast< int >( file_val ) - 1;
        int right_boundary  = 8 - static_cast< int >( file_val );
        int bottom_boundary = static_cast< int >( rank_val ) - 1;
        int top_boundary    = 8 - static_cast< int >( rank_val );

        std::vector< pieces::position_t > to_delete;

        bool blocked = false;
        for ( int i = 1; i <= right_boundary; i++ ) {
            if ( blocked ) {
                auto const & space =
                    game_board.at( current.position().first ).at( static_cast< pieces::file_t >( file_val + i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece =
                game_board.at( current.position().first ).at( static_cast< pieces::file_t >( file_val + i ) ).piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= left_boundary; i++ ) {
            if ( blocked ) {
                auto const & space =
                    game_board.at( current.position().first ).at( static_cast< pieces::file_t >( file_val - i ) );
                to_delete.push_back( space.position() );
            }

            auto const & piece =
                game_board.at( current.position().first ).at( static_cast< pieces::file_t >( file_val - i ) ).piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= top_boundary; i++ ) {
            if ( blocked ) {
                auto const & space =
                    game_board.at( static_cast< pieces::rank_t >( rank_val + i ) ).at( current.position().second );
                to_delete.push_back( space.position() );
            }

            auto const & piece =
                game_board.at( static_cast< pieces::rank_t >( rank_val + i ) ).at( current.position().second ).piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        blocked = false;
        for ( int i = 1; i <= bottom_boundary; i++ ) {
            if ( blocked ) {
                auto const & space =
                    game_board.at( static_cast< pieces::rank_t >( rank_val - i ) ).at( current.position().second );
                to_delete.push_back( space.position() );
            }

            auto const & piece =
                game_board.at( static_cast< pieces::rank_t >( rank_val - i ) ).at( current.position().second ).piece;

            if ( piece ) {
                // if the piece encountered is the same colour then we cant move there
                if ( piece->colour() == current.piece->colour() ) {
                    to_delete.push_back( piece->position() );
                }
                blocked = true;
            }
        }

        // game class will handle check and castling logic
        auto x = std::erase_if( moves, [&to_delete]( space const & move ) {
            for ( auto del : to_delete ) {
                if ( del == move ) {
                    return true;
                }
            }
            return false;
        } );
    }
    void board::filter_queen_moves( space const & current, std::vector< space > & moves ) const
    {
        filter_rook_moves( current, moves );
        filter_bishop_moves( current, moves );
    }
    void board::filter_king_moves( space const & current, std::vector< space > & moves ) const
    {
        // game class will handle check and castling logic
        auto x = std::erase_if( moves, [&current]( space const & move ) {
            return move.piece && move.piece->colour() == current.piece->colour();
        } );
    }

    void board::add_pawn_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        int rank = static_cast< int >( current.position().first );
        int file = static_cast< int >( current.position().second );
        if ( current.piece->colour() ) {
            if ( rank < 8 && file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file - 1 ).value() );
            }
            if ( rank < 8 && file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file + 1 ).value() );
            }
        }
        else {
            if ( rank > 1 && file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file - 1 ).value() );
            }
            if ( rank > 1 && file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file + 1 ).value() );
            }
        }
    }
    void board::add_knight_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        int rank = static_cast< int >( current.position().first );
        int file = static_cast< int >( current.position().second );

        if ( rank - 2 >= 1 ) {
            if ( file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank - 2, file - 1 ).value() );
            }
            if ( file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank - 2, file + 1 ).value() );
            }
        }
        if ( rank + 2 <= 8 ) {
            if ( file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank + 2, file - 1 ).value() );
            }
            if ( file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank + 2, file + 1 ).value() );
            }
        }

        if ( file - 2 >= 1 ) {
            if ( rank - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file - 2 ).value() );
            }
            if ( rank + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file - 2 ).value() );
            }
        }
        if ( file + 2 <= 8 ) {
            if ( rank - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file + 2 ).value() );
            }
            if ( rank + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file + 2 ).value() );
            }
        }
    }

    void board::add_bishop_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        int rank_val = static_cast< int >( current.position().first );
        int file_val = static_cast< int >( current.position().second );

        for ( int rank = rank_val - 1, file = file_val - 1; rank >= 1 && file >= 1; rank--, file-- ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int rank = rank_val - 1, file = file_val + 1; rank >= 1 && file <= 8; rank--, file++ ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int rank = rank_val + 1, file = file_val - 1; rank <= 8 && file >= 1; rank++, file-- ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int rank = rank_val + 1, file = file_val + 1; rank <= 8 && file <= 8; rank++, file++ ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }
    }

    void board::add_rook_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        int rank_val = static_cast< int >( current.position().first );
        int file_val = static_cast< int >( current.position().second );

        for ( int rank = rank_val - 1; rank >= 1; rank-- ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file_val ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int rank = rank_val + 1; rank <= 8; rank++ ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank ) ).at( static_cast< pieces::file_t >( file_val ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int file = file_val - 1; file >= 1; file-- ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank_val ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }

        for ( int file = file_val + 1; file <= 8; file++ ) {
            auto const & space =
                game_board.at( static_cast< pieces::rank_t >( rank_val ) ).at( static_cast< pieces::file_t >( file ) );
            moves.push_back( space.position() );

            if ( space.piece ) {
                break;
            }
        }
    }
    void board::add_queen_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        add_rook_attacks( current, moves );
        add_bishop_attacks( current, moves );
    }
    void board::add_king_attacks( space const & current, std::vector< pieces::position_t > & moves ) const
    {
        int rank = static_cast< int >( current.position().first );
        int file = static_cast< int >( current.position().second );

        if ( rank - 1 >= 1 ) {
            if ( file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file - 1 ).value() );
            }
            if ( file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank - 1, file + 1 ).value() );
            }
            moves.push_back( pieces::piece::itopos( rank - 1, file ).value() );
        }
        if ( rank + 1 <= 8 ) {
            if ( file - 1 >= 1 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file - 1 ).value() );
            }
            if ( file + 1 <= 8 ) {
                moves.push_back( pieces::piece::itopos( rank + 1, file + 1 ).value() );
            }
            moves.push_back( pieces::piece::itopos( rank + 1, file ).value() );
        }

        if ( file - 1 >= 1 ) {
            moves.push_back( pieces::piece::itopos( rank, file - 1 ).value() );
        }
        if ( file + 1 <= 8 ) {
            moves.push_back( pieces::piece::itopos( rank, file + 1 ).value() );
        }
    }

    std::vector< pieces::position_t > board::possible_attacks( space const & src ) const
    {
        std::vector< pieces::position_t > positions;

        switch ( src.piece->type() ) {
        case pieces::name_t::rook:
            add_rook_attacks( src, positions );
            break;
        case pieces::name_t::knight:
            add_knight_attacks( src, positions );
            break;
        case pieces::name_t::bishop:
            add_bishop_attacks( src, positions );
            break;
        case pieces::name_t::king:
            add_king_attacks( src, positions );
            break;
        case pieces::name_t::queen:
            add_queen_attacks( src, positions );
            break;
        case pieces::name_t::pawn:
            add_pawn_attacks( src, positions );
            break;
        }

        return positions;
    }

    space const & board::get( pieces::position_t pos ) const { return game_board.at( pos.first ).at( pos.second ); }

    std::string board::to_string() const
    {
        std::stringstream serialized;

        for ( int i = 8; i > 0; i-- ) {
            serialized << i << "\t|";
            for ( int j = 1; j <= 8; j++ ) {
                auto rank = static_cast< pieces::rank_t >( i );
                auto file = static_cast< pieces::file_t >( j );

                space const & current_space = game_board.at( rank ).at( file );

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
                        serialized << "1|";
                    }
                    else {
                        serialized << "0|";
                    }
                }
            }
            serialized << "\n";
        }

        serialized << "\n\t ";
        for ( int j = 1; j <= 8; j++ ) {
            serialized << char( 'A' + j - 1 ) << " ";
        }
        serialized << "\n";
        return serialized.str();
    }

    void board::place_pieces()
    {

        auto rank   = static_cast< pieces::rank_t >( 1 );
        bool colour = true;
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            switch ( file ) {
            case pieces::file_t::a:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            case pieces::file_t::b:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::c:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::d:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::queen >( colour, rank, file );
                break;
            case pieces::file_t::e:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::king >( colour, rank, file );
                break;
            case pieces::file_t::f:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::g:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::h:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            }
        }

        colour = false;
        rank   = static_cast< pieces::rank_t >( 8 );
        for ( int j = 1; j <= 8; j++ ) {
            auto file = static_cast< pieces::file_t >( j );

            switch ( file ) {
            case pieces::file_t::a:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::rook >( colour, rank, file );
                break;
            case pieces::file_t::b:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::c:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::d:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::queen >( colour, rank, file );
                break;
            case pieces::file_t::e:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::king >( colour, rank, file );
                break;
            case pieces::file_t::f:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::bishop >( colour, rank, file );
                break;
            case pieces::file_t::g:
                game_board.at( rank ).at( file ).piece = std::make_unique< pieces::knight >( colour, rank, file );
                break;
            case pieces::file_t::h:
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

    bool board::determine_threat( space const & src, space const & dst, space const & target,
                                  bool const victim_colour ) const
    {
        if ( !src.piece ) {
            return false;
        }

        // copy the board
        auto board = *this;

        // force the move on the copied board (src to dst)
        board[dst.position().first].at( dst.position().second ).piece = src.piece->copy_piece();
        if ( board[dst.position().first].at( dst.position().second ).piece ) {
            board[dst.position().first].at( dst.position().second ).piece->place( src.position() );
        }
        board[src.position().first].at( src.position().second ).piece.reset();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto          pos = pieces::piece::itopos( i, j ).value();
                space const & sp  = board.get( pos );
                // if sp has a piece that is a valid attacker
                if ( sp.piece && sp.piece->colour() != victim_colour ) {
                    auto possible_moves = board.possible_moves( sp );

                    if ( std::find( possible_moves.begin(), possible_moves.end(), target ) != possible_moves.end() ) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

}  // namespace chess::game