#include "space.hpp"
#include <algorithm>
#include <array>
#include <board.hpp>
#include <cassert>
#include <chrono>
#include <game.hpp>
#include <iostream>
#include <piece.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace chess {

    chess_game::chess_game() :
        game_board(),
        white_king( *reinterpret_cast< pieces::king * >(
            game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() ) ),
        black_king(
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() ) )
    {
        start();
    }

    void chess_game::start()
    {
        state = game_state::white_move;
        game_board.reset();
        king_side_castle_white  = true;
        king_side_castle_black  = true;
        queen_side_castle_white = true;
        queen_side_castle_black = true;

        white_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() );
        black_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() );
        update_attack_map();
    }

    bool chess_game::checkmate( bool const colour ) const
    {
        std::vector< game::space > moves;
        moves.reserve( 64 );
        auto board_copy = game_board;
        // std::cout << game_attack_map.to_string();

        // auto rook_square = board_copy.get( pieces::piece::itopos( 2, 1 ).value() );
        // possible_moves( board_copy, rook_square, moves );
        // std::cout << moves.size();
        // exit( 1 );

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & src = board_copy.get( pieces::piece::itopos( i, j ).value() );
                if ( src.piece && src.piece->colour() == colour ) {
                    possible_moves( board_copy, src, moves );
                    if ( !moves.empty() ) {
                        return false;
                    }
                }
            }
        }

        std::cout << "game thinks its checkmate\n";
        // auto board = *this;
        // auto a     = generate_attack_map( board.game_board );
        // chess_game::attack_map m;
        // auto                   queen   = get( pieces::piece::itopos( 7, 6 ).value() );
        // auto                   attacks = possible_attacks( queen );

        // std::cout << "Queen attacks: ";
        // for ( auto p : attacks ) {
        //     std::cout << pieces::to_string( p ) << " ";
        // }
        // std::cout << std::endl;

        // chess_game::attack_map a;
        // a.clear();
        // std::cout << "queen color: " << ( queen.piece->colour() == true ) << std::endl;
        // std::cout << "Fresh map:\n";
        // std::cout << a.to_string();
        // for ( auto m : attacks ) {
        //     a.add_attacker( queen, m );
        // }

        // std::cout << "Post queen only:\n";
        // std::cout << a.to_string();

        // std::cout << "regular map and board:\n";
        // std::cout << game_attack_map.to_string();
        // std::cout << game_board.to_string();
        // std::cout << a.to_string();

        // auto                       black_king_space = get( pieces::piece::itopos( 8, 5 ).value() );
        // std::vector< game::space > black_king_moves;
        // possible_moves( black_king_space, black_king_moves );
        // std::cout << "king moves: " << black_king_moves.size() << std::endl;
        return true;
    }

    pieces::move_status chess_game::move( game::space const & src, game::space const & dst )
    {
        if ( !src.piece ) {
            return pieces::move_status::no_piece_to_move;
        }

        // deep copy
        auto src_piece_cpy = src.piece->copy_piece();

        std::vector< game::space > pos;
        auto                       status = possible_moves( game_board, src, pos );

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        if ( pos.empty() || std::find( pos.begin(), pos.end(), dst ) == pos.end() ) {
            return pieces::move_status::illegal_move;
        }

        if ( src.piece->colour() ) {
            // if the king is being attacked
            if ( src.piece->type() != pieces::name_t::king ) {
                if ( game_board.determine_threat( src, dst, game_board.get( white_king.get().position() ), true ) ) {
                    return pieces::move_status::enters_check;
                }
            }
            else {
                // if the king is moving into check
                if ( game_board.determine_threat( src, dst, dst, true ) ) {
                    return pieces::move_status::enters_check;
                }
            }
        }
        else {
            // if the king is being attacked
            if ( src.piece->type() != pieces::name_t::king ) {
                if ( game_board.determine_threat( src, dst, game_board.get( black_king.get().position() ), false ) ) {
                    return pieces::move_status::enters_check;
                }
            }
            else {
                // if the king is moving into check
                if ( game_board.determine_threat( src, dst, dst, false ) ) {
                    return pieces::move_status::enters_check;
                }
            }
        }

        auto copy_dst = dst.position();
        status        = game_board.move( src.position(), dst.position() );
        update_attack_map();
        // std::cout << "This one worked\n";
        // for ( int i = 0; i < 8; i++ ) {
        //     for ( int j = 0; j < 8; j++ ) {
        //         std::cout << attack_map[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << "-------------------\n";

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        if ( white_move() ) {
            if ( game_board.determine_threat( game_board.get( black_king.get().position() ),
                                              game_board.get( black_king.get().position() ),
                                              game_board.get( black_king.get().position() ), false ) ) {
                state = game_state::black_check;
                if ( checkmate( false ) ) {
                    std::cout << "White Wins\n";
                    state = game_state::white_wins;
                }
            }
            else {
                state = game_state::black_move;
            }

            white_castling_rights( src_piece_cpy, src.position() );

            if ( dst == pieces::piece::itopos( 8, 1 ) ) {
                queen_side_castle_black = false;
            }
            else if ( dst == pieces::piece::itopos( 8, 8 ) ) {
                king_side_castle_black = false;
            }
        }
        else if ( black_move() ) {
            if ( game_board.determine_threat( game_board.get( white_king.get().position() ),
                                              game_board.get( white_king.get().position() ),
                                              game_board.get( white_king.get().position() ), true ) ) {
                state = game_state::white_check;
                if ( checkmate( true ) ) {
                    std::cout << "Black Wins\n";
                    state = game_state::black_wins;
                }
            }
            else {
                state = game_state::white_move;
            }
            black_castling_rights( src_piece_cpy, src.position() );
            if ( dst == pieces::piece::itopos( 1, 1 ) ) {
                queen_side_castle_white = false;
            }
            else if ( dst == pieces::piece::itopos( 1, 8 ) ) {
                king_side_castle_white = false;
            }
        }
        else {
            throw std::logic_error( "Impossible Game State" );
        }
        return pieces::move_status::valid;
    }

    void chess_game::white_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                            pieces::position_t const &               src_pos )
    {
        // white castling logic
        if ( ( king_side_castle_white || queen_side_castle_white ) && moved_piece->colour() ) {
            if ( moved_piece->type() == pieces::name_t::king ) {
                king_side_castle_white  = false;
                queen_side_castle_white = false;
            }
            else if ( moved_piece->type() == pieces::name_t::rook ) {
                if ( queen_side_castle_white &&
                     src_pos == pieces::position_t( pieces::rank_t::one, pieces::file_t::a ) ) {
                    queen_side_castle_white = false;
                }
                else if ( king_side_castle_white &&
                          src_pos == pieces::position_t( pieces::rank_t::one, pieces::file_t::h ) ) {
                    king_side_castle_white = false;
                }
            }
        }
    }

    void chess_game::black_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                            pieces::position_t const &               src_pos )
    {
        // black castling logic
        if ( ( king_side_castle_black || queen_side_castle_black ) && !moved_piece->colour() ) {
            if ( moved_piece->type() == pieces::name_t::king ) {
                king_side_castle_black  = false;
                queen_side_castle_black = false;
            }
            else if ( moved_piece->type() == pieces::name_t::rook ) {
                if ( queen_side_castle_black &&
                     src_pos == pieces::position_t( pieces::rank_t::eight, pieces::file_t::a ) ) {
                    queen_side_castle_black = false;
                }
                else if ( king_side_castle_black &&
                          src_pos == pieces::position_t( pieces::rank_t::eight, pieces::file_t::h ) ) {
                    king_side_castle_black = false;
                }
            }
        }
    }

    bool chess_game::can_castle( bool const color ) const
    {
        if ( color ) {
            return ( king_side_castle_white || queen_side_castle_white );
        }
        else {
            return ( king_side_castle_black || queen_side_castle_black );
        }
    }

    void chess_game::remove_piece_at( pieces::position_t position ) { game_board.remove_piece_at( position ); }

    std::vector< std::string > chess_game::get_move_history() const { return game_board.get_move_history(); }

    bool chess_game::white_move() const { return state == game_state::white_move || state == game_state::white_check; }

    bool chess_game::black_move() const { return state == game_state::black_move || state == game_state::black_check; }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    void chess_game::update_attack_map()
    {
        game_attack_map.clear();

        // std::cout << game_board.to_string();
        // std::cout << game_attack_map.to_string();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game_board.get( pieces::piece::itopos( i, j ).value() );
                if ( !space.piece ) {
                    continue;
                }
                auto attacks = possible_attacks( space );

                for ( auto m : attacks ) {
                    game_attack_map.add_attacker( space, m );
                }
            }
        }

        // std::cout << "After:\n";
        // std::cout << game_attack_map.to_string();
        // std::cout << game_board.to_string();
    }

    chess_game::attack_map const chess_game::generate_attack_map( game::board board ) const
    {
        std::chrono::high_resolution_clock::time_point start, end, startl, endl;
        std::chrono::microseconds                      duration;

        start = std::chrono::high_resolution_clock::now();

        attack_map generated_map;
        generated_map.clear();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                std::cout << "before itopos\n";
                auto & space = board.get( pieces::piece::itopos( i, j ).value() );

                std::cout << "after itopos\n";
                if ( !space.piece ) {
                    continue;
                }

                std::cout << "after space.piece\n";

                auto attacks = possible_attacks( board, space );

                std::cout << "start adding attacker to map\n";
                for ( auto m : attacks ) {
                    generated_map.add_attacker( space, m );
                }
                std::cout << "end adding attacker to map\n";
            }
        }

        end      = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        // std::cout << "generate attack map took " << duration.count() << " microseconds\n";

        // exit( 1 );

        return generated_map;
    }

    std::vector< pieces::position_t > chess_game::possible_attacks( game::space const & src ) const
    {
        return possible_attacks( game_board, src );
    }
    std::vector< pieces::position_t > chess_game::possible_attacks( game::board const & b,
                                                                    game::space const & src ) const
    {
        std::vector< pieces::position_t > attack_squares;

        if ( !src.piece ) {
            return attack_squares;
        }

        auto piece_attack_squares = b.possible_attacks( src );

        // std::cout << "In possible attacks, size=: " << piece_attack_squares.size() << std::endl;

        return piece_attack_squares;
    }

    std::vector< game::space > chess_game::psuedo_possible_moves( game::board         game_board,
                                                                  game::space const & src ) const
    {
        return game_board.possible_moves( src );
    }

    std::vector< move_t > chess_game::legal_moves() const
    {
        auto                  board_copy = game_board;
        std::vector< move_t > moves;
        moves.reserve( 3000 );
        // std::cout << "legal moves started\n";
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & src = board_copy.get( pieces::piece::itopos( i, j ).value() );

                std::vector< game::space > dsts;
                dsts.reserve( 100 );
                possible_moves( board_copy, src, dsts );

                for ( auto const & dst : dsts ) {
                    moves.push_back( { src, dst } );
                }
            }
        }
        // std::cout << "legal moves terminated\n";
        return moves;
    }
    pieces::move_status chess_game::possible_moves( game::space const & src, std::vector< game::space > & moves ) const
    {
        auto board_copy = game_board;
        return possible_moves( board_copy, src, moves );
    }

    pieces::move_status chess_game::possible_moves( game::board board_copy, game::space const & src,
                                                    std::vector< game::space > & possible_moves ) const
    {

        // std::chrono::high_resolution_clock::time_point start, end;
        // std::chrono::microseconds                      duration;

        // start = std::chrono::high_resolution_clock::now();

        if ( !src.piece ) {
            return pieces::move_status::no_piece_to_move;
        }

        if ( src.piece->colour() ) {
            if ( !white_move() ) {  // if it is not whites move than we cannot move a white piece
                return pieces::move_status::invalid_turn;
            }
        }
        else {
            if ( !black_move() ) {  // and vice versa
                return pieces::move_status::invalid_turn;
            }
        }

        possible_moves = game_board.possible_moves( src );
        // std::cout << "in possible_moves() move count: " << possible_moves.size() << std::endl;
        // std::cout << "\t";
        // for ( auto m : possible_moves )
        //     std::cout << pieces::to_string( m.position() ) << " ";
        // std::cout << std::endl;
        // std::cout << "Initial pawn moves: " << possible_moves.size();
        // if applicable add castling logic
        if ( src.piece->type() == pieces::name_t::king ) {
            pieces::position_t king_position = src.piece->position();
            int                king_rank     = static_cast< int >( king_position.first );
            int                king_file     = static_cast< int >( king_position.second );

            // src.piece->colour() ? pieces::rank_t::one : pieces::rank_t::eight;

            // King cannot castle if currently in check
            bool king_in_check = game_attack_map.has_attackers( src, !src.piece->colour() );

            if ( src.piece->colour() ) {  // White
                if ( !king_in_check && king_side_castle_white ) {
                    // Check if kingside castling squares are empty
                    std::vector< game::space > castle_squares;
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 6 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 7 ).value() ) );

                    bool kingside_squares_empty = true;
                    bool kingside_path_safe     = true;

                    for ( auto s : castle_squares ) {
                        if ( s.piece ) {
                            kingside_squares_empty = false;
                        }
                        else if ( game_attack_map.has_attackers( s, !src.piece->colour() ) ) {
                            kingside_path_safe = false;
                        }
                    }

                    if ( kingside_squares_empty && kingside_path_safe ) {
                        possible_moves.push_back( board_copy.get( { pieces::rank_t::one, pieces::file_t::g } ) );
                    }
                }

                if ( !king_in_check && queen_side_castle_white ) {
                    // Check if queenside castling squares are empty
                    std::vector< game::space > castle_squares;
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 2 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 3 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 4 ).value() ) );

                    bool queenside_squares_empty = true;
                    bool queenside_path_safe     = true;

                    for ( auto s : castle_squares ) {
                        if ( s.piece ) {
                            queenside_squares_empty = false;
                        }
                        else if ( game_attack_map.has_attackers( s, !src.piece->colour() ) ) {
                            queenside_path_safe = false;
                        }
                    }

                    if ( queenside_squares_empty && queenside_path_safe ) {
                        possible_moves.push_back( board_copy.get( { pieces::rank_t::one, pieces::file_t::c } ) );
                    }
                }
            }
            else {  // Black
                if ( !king_in_check && king_side_castle_black ) {
                    // Check if kingside castling squares are empty

                    std::vector< game::space > castle_squares;
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 6 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 7 ).value() ) );

                    bool kingside_squares_empty = true;
                    bool kingside_path_safe     = true;

                    for ( auto s : castle_squares ) {
                        if ( s.piece ) {
                            kingside_squares_empty = false;
                        }
                        else if ( game_attack_map.has_attackers( s, !src.piece->colour() ) ) {
                            kingside_path_safe = false;
                        }
                    }

                    if ( kingside_squares_empty && kingside_path_safe ) {
                        possible_moves.push_back( board_copy.get( { pieces::rank_t::eight, pieces::file_t::g } ) );
                    }
                }

                if ( !king_in_check && queen_side_castle_black ) {
                    // Check if queenside castling squares are empty
                    std::vector< game::space > castle_squares;
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 2 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 3 ).value() ) );
                    castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 4 ).value() ) );

                    bool queenside_squares_empty = true;
                    bool queenside_path_safe     = true;

                    for ( auto s : castle_squares ) {
                        if ( s.piece ) {
                            queenside_squares_empty = false;
                        }
                        else if ( game_attack_map.has_attackers( s, !src.piece->colour() ) ) {
                            queenside_path_safe = false;
                        }
                    }
                    if ( queenside_squares_empty && queenside_path_safe ) {
                        possible_moves.push_back( board_copy.get( { pieces::rank_t::eight, pieces::file_t::c } ) );
                    }
                }
            }
        }
        std::erase_if( possible_moves, [this, &src, &board_copy]( game::space const & dst ) {
            std::cout << "erase start\n";
            int dst_rank = static_cast< int >( dst.position().first );
            int dst_file = static_cast< int >( dst.position().second );

            // std::cout << "Attack map in erase_if:\n";
            // for ( int i = 0; i < 8; i++ ) {
            //     for ( int j = 0; j < 8; j++ ) {
            //         std::cout << attack_map[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << "---------------------------\n";
            // std::cout << "--------------\n";
            // std::cout << "\niterating through: " << pieces::to_string( dst.position() ) << std::endl;
            if ( src.piece->colour() ) {
                auto king_pos  = board_copy.get( white_king.get().position() );
                int  king_rank = static_cast< int >( king_pos.position().first );
                int  king_file = static_cast< int >( king_pos.position().second );

                // make sure king is not moving into danger
                if ( src.piece->type() == pieces::name_t::king ) {
                    std::unique_ptr< pieces::piece > king_piece;
                    king_piece = pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );
                    std::cout << "king access start\n";
                    board_copy.remove_piece_at( src.position() );
                    auto temp_attack_map = generate_attack_map( board_copy );
                    board_copy.add_piece_at( *king_piece, src.position() );
                    std::cout << "king access end\n";

                    if ( temp_attack_map.has_attackers( dst, !src.piece->colour() ) ) {
                        return true;
                    }
                }
                // Otherwise, we're not moving the king so check if the king is safe
                else {
                    // save the piece
                    assert( board_copy.get( src.position() ).piece != nullptr );

                    std::unique_ptr< pieces::piece > src_piece, dst_piece;

                    src_piece = pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );

                    if ( board_copy.get( dst.position() ).piece ) {
                        dst_piece = pieces::piece::copy_piece( *board_copy.get( dst.position() ).piece );
                    }

                    // Remove the src piece
                    board_copy.remove_piece_at( src.position() );
                    assert( !board_copy.get( src.position() ).piece );

                    src_piece->place( dst.position() );
                    board_copy.add_piece_at( *src_piece, dst.position() );
                    assert( board_copy.get( dst.position() ).piece != nullptr );

                    auto temp_attack_map = generate_attack_map( board_copy );
                    // Since for speed I'm not copying the board more than I need to, we need to put the piece back
                    // to its original square
                    src_piece->place( src.position() );
                    board_copy.add_piece_at( *src_piece, src.position() );
                    assert( board_copy.get( src.position() ).piece != nullptr );

                    if ( dst_piece ) {
                        dst_piece->place( dst.position() );
                        board_copy.add_piece_at( *dst_piece, dst.position() );
                        assert( board_copy.get( dst.position() ).piece != nullptr );
                    }
                    else {
                        board_copy.remove_piece_at( dst.position() );
                    }

                    if ( temp_attack_map.has_attackers( king_pos, !src.piece->colour() ) ) {
                        // std::cout << "someone is attacking king somehow?\n";
                        // std::cout << game_attack_map.to_string();
                        // std::cout << "\n\n";
                        // std::cout << temp_attack_map.to_string();
                        return true;
                    }
                    else {
                        return false;
                    }
                }
            }
            else {
                std::cout << "black start\n";
                auto king_pos  = board_copy.get( black_king.get().position() );
                int  king_rank = static_cast< int >( king_pos.position().first );
                int  king_file = static_cast< int >( king_pos.position().second );
                // make sure
                // king is not moving into danger
                if ( src.piece->type() == pieces::name_t::king ) {
                    std::unique_ptr< pieces::piece > king_piece;
                    king_piece = pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );
                    board_copy.remove_piece_at( src.position() );
                    auto temp_attack_map = generate_attack_map( board_copy );
                    board_copy.add_piece_at( *king_piece, src.position() );

                    if ( temp_attack_map.has_attackers( dst, !src.piece->colour() ) ) {
                        return true;
                    }
                }
                // Otherwise, we're not moving the king so check if the king is safe
                else {
                    // Check if we're pinned to the king

                    // save the piece
                    std::unique_ptr< pieces::piece > src_piece, dst_piece;

                    src_piece = pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );
                    if ( board_copy.get( dst.position() ).piece ) {
                        dst_piece = pieces::piece::copy_piece( *board_copy.get( dst.position() ).piece );
                    }

                    // Remove the piece
                    board_copy.remove_piece_at( src.position() );
                    assert( !board_copy.get( src.position() ).piece );

                    // Here, the king is in check. Add the piece we removed to the destination square, recompute the
                    // attack map, and see if the king is still under attack. If not, it's a valid move

                    // if ( ( attack_map[king_rank - 1][king_file - 1] & 2 ) > 0 ) {
                    src_piece->place( dst.position() );
                    board_copy.add_piece_at( *src_piece, dst.position() );
                    assert( board_copy.get( dst.position() ).piece != nullptr );

                    std::cout << "before attack map\n";
                    auto temp_attack_map = generate_attack_map( board_copy );

                    std::cout << "after attack map\n";
                    // Since for speed I'm not copying the board more than I need to, we need to revert the moves we
                    // made

                    std::cout << "start revert\n";
                    src_piece->place( src.position() );
                    board_copy.add_piece_at( *src_piece, src.position() );
                    assert( board_copy.get( src.position() ).piece != nullptr );

                    std::cout << "end src re-add\n";
                    if ( dst_piece ) {
                        std::cout << "dst piece re-add";
                        dst_piece->place( dst.position() );
                        board_copy.add_piece_at( *dst_piece, dst.position() );
                        assert( board_copy.get( dst.position() ).piece != nullptr );
                    }
                    else {
                        std::cout << "dst position remove\n";
                        board_copy.remove_piece_at( dst.position() );
                    }
                    // Check if the king is now in check
                    // bool pinned = ( ( attack_map[king_rank - 1][king_file - 1] & 1 ) == 0 ) &&
                    //               ( temp_attack_map[king_rank - 1][king_file - 1] & 1 ) > 0;
                    // // If it is, we're pinned.
                    // if ( pinned ) {
                    //     board_copy.add_piece_at( *original_piece, src.position() );
                    //     return true;
                    // }

                    // If we reach here, we are not pinned to the king
                    // std::cout << "BLACK KING RANK FILE: " << king_rank << " " << king_file << std::endl;
                    // Here, the king is in check. Add the piece we removed to the destination square, recompute the
                    // attack map, and see if the king is still under attack. If not, it's a valid move
                    if ( temp_attack_map.has_attackers( king_pos, !src.piece->colour() ) ) {
                        return true;
                    }
                    else {
                        return false;
                    }
                    // if ( ( attack_map[king_rank - 1][king_file - 1] & 1 ) > 0 ) {
                    //     board_copy.add_piece_at( *original_piece, dst.position() );
                    //     assert( board_copy.get( dst.position() ).piece != nullptr );

                    //     temp_attack_map = generate_attack_map( board_copy );

                    //     // Since for speed I'm not copying the board more than I need to, we need to put the
                    //     piece back
                    //     // to its original square
                    //     board_copy.remove_piece_at( dst.position() );
                    //     assert( !board_copy.get( dst.position() ).piece );

                    //     board_copy.add_piece_at( *original_piece, src.position() );
                    //     assert( board_copy.get( src.position() ).piece != nullptr );

                    //     if ( ( temp_attack_map[king_rank - 1][king_file - 1] & 1 ) > 0 ) {
                    //         return true;
                    //     }
                    //     else {
                    //         return false;
                    //     }
                    // }
                    // else {
                    //     // Same thing, put the piece back to its original square
                    //     board_copy.add_piece_at( *original_piece, src.position() );
                    // }
                }
            }

            return false;
        } );

        std::cout << "erase end\n";

        // end      = std::chrono::high_resolution_clock::now();
        // duration = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        // std::cout << "Possible moves took " << duration.count() << " microseconds\n";
        return pieces::move_status::valid;
    }

    std::vector< game::space > chess_game::find_attackers( game::space const & src, bool const victim_color ) const
    {
        std::vector< game::space > attackers;

        auto moves = legal_moves();

        for ( auto m : moves ) {

            if ( m.second == src ) {
                if ( m.first.piece && m.first.piece->colour() && m.first.piece->colour() != victim_color ) {
                    attackers.push_back( m.second );
                }
            }
        }

        return attackers;
    }

}  // namespace chess