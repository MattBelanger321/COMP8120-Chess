#include <board.hpp>
#include <game.hpp>
#include <iostream>
#include <piece.hpp>
#include <stdexcept>
#include <string>

#include <algorithm>
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
    }

    bool chess_game::checkmate( bool const colour ) const
    {
        std::vector< game::space > moves;
        moves.reserve( 64 );
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & src = game_board.get( pieces::piece::itopos( i, j ).value() );
                if ( src.piece && src.piece->colour() == colour ) {

                    possible_moves( src, moves );
                    if ( !moves.empty() ) {
                        return false;
                    }
                }
            }
        }
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
        auto                       status = possible_moves( src, pos );

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
                if ( game_board.determine_threat( src, dst, dst, true ) ) {
                    return pieces::move_status::enters_check;
                }
            }
        }

        status = game_board.move( src.position(), dst.position() );

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

    bool chess_game::white_move() const { return state == game_state::white_move || state == game_state::white_check; }

    bool chess_game::black_move() const { return state == game_state::black_move || state == game_state::black_check; }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    pieces::move_status chess_game::possible_moves( game::space const &          src,
                                                    std::vector< game::space > & possible_moves ) const
    {
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

        // if applicable add castling logic
        if ( src.piece->type() == pieces::name_t::king ) {
            // King cannot castle if currently in check
            pieces::rank_t king_rank = src.piece->colour() ? pieces::rank_t::one : pieces::rank_t::eight;
            bool           king_in_check =
                game_board.determine_threat( src, game_board.get( { king_rank, pieces::file_t::e } ),
                                             game_board.get( { king_rank, pieces::file_t::e } ), src.piece->colour() );

            if ( !king_in_check ) {
                if ( src.piece->colour() ) {  // White
                    if ( king_side_castle_white ) {
                        // Check if kingside castling squares are empty
                        bool kingside_squares_empty =
                            !game_board.get( { pieces::rank_t::one, pieces::file_t::f } ).piece &&
                            !game_board.get( { pieces::rank_t::one, pieces::file_t::g } ).piece;

                        // Check if king won't pass through or land on attacked squares
                        bool kingside_path_safe =
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::one, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::one, pieces::file_t::f } ), true ) &&
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::one, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::one, pieces::file_t::g } ), true );

                        if ( kingside_squares_empty && kingside_path_safe ) {
                            possible_moves.push_back( game_board.get( { pieces::rank_t::one, pieces::file_t::g } ) );
                        }
                    }

                    if ( queen_side_castle_white ) {
                        // Check if queenside castling squares are empty
                        bool queenside_squares_empty =
                            !game_board.get( { pieces::rank_t::one, pieces::file_t::b } ).piece &&
                            !game_board.get( { pieces::rank_t::one, pieces::file_t::c } ).piece &&
                            !game_board.get( { pieces::rank_t::one, pieces::file_t::d } ).piece;

                        // Check if king won't pass through or land on attacked squares
                        bool queenside_path_safe =
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::one, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::one, pieces::file_t::d } ), true ) &&
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::one, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::one, pieces::file_t::c } ), true );

                        if ( queenside_squares_empty && queenside_path_safe ) {
                            possible_moves.push_back( game_board.get( { pieces::rank_t::one, pieces::file_t::c } ) );
                        }
                    }
                }
                else {  // Black
                    if ( king_side_castle_black ) {
                        // Check if kingside castling squares are empty
                        bool kingside_squares_empty =
                            !game_board.get( { pieces::rank_t::eight, pieces::file_t::f } ).piece &&
                            !game_board.get( { pieces::rank_t::eight, pieces::file_t::g } ).piece;

                        // Check if king won't pass through or land on attacked squares
                        bool kingside_path_safe =
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::eight, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::eight, pieces::file_t::f } ), false ) &&
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::eight, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::eight, pieces::file_t::g } ), false );

                        if ( kingside_squares_empty && kingside_path_safe ) {
                            possible_moves.push_back( game_board.get( { pieces::rank_t::eight, pieces::file_t::g } ) );
                        }
                    }

                    if ( queen_side_castle_black ) {
                        // Check if queenside castling squares are empty
                        bool queenside_squares_empty =
                            !game_board.get( { pieces::rank_t::eight, pieces::file_t::b } ).piece &&
                            !game_board.get( { pieces::rank_t::eight, pieces::file_t::c } ).piece &&
                            !game_board.get( { pieces::rank_t::eight, pieces::file_t::d } ).piece;

                        // Check if king won't pass through or land on attacked squares
                        bool queenside_path_safe =
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::eight, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::eight, pieces::file_t::d } ), false ) &&
                            !game_board.determine_threat(
                                src, game_board.get( { pieces::rank_t::eight, pieces::file_t::e } ),
                                game_board.get( { pieces::rank_t::eight, pieces::file_t::c } ), false );

                        if ( queenside_squares_empty && queenside_path_safe ) {
                            possible_moves.push_back( game_board.get( { pieces::rank_t::eight, pieces::file_t::c } ) );
                        }
                    }
                }
            }
            // If king is in check, castling is not allowed
        }

        std::erase_if( possible_moves, [this, &src, possible_moves]( game::space const & dst ) {
            auto king_pos = game_board.get( white_king.get().position() );

            if ( src.piece->colour() ) {
                // make sure king is not moving into danger
                if ( src.piece->type() == pieces::name_t::king ) {
                    if ( game_board.determine_threat( src, dst, dst, true ) ) {
                        return true;
                    }

                    // check castling through check
                    if ( dst.position() == pieces::position_t{ pieces::rank_t::one, pieces::file_t::g } ) {
                        if ( possible_moves.empty() ||
                             std::find( possible_moves.begin(), possible_moves.end(),
                                        pieces::position_t{ pieces::rank_t::one, pieces::file_t::f } ) ==
                                 possible_moves.end() ) {
                            return true;
                        }
                    }

                    if ( dst.position() == pieces::position_t{ pieces::rank_t::one, pieces::file_t::c } ) {
                        if ( possible_moves.empty() ||
                             std::find( possible_moves.begin(), possible_moves.end(),
                                        pieces::position_t{ pieces::rank_t::one, pieces::file_t::d } ) ==
                                 possible_moves.end() ) {
                            return true;
                        }
                    }
                }
                // otherwise, make sure the move is not entering check (or keeping us in check)
                else if ( game_board.determine_threat( src, dst, king_pos, true ) ) {
                    return true;
                }
            }
            else {
                auto king_pos = game_board.get( black_king.get().position() );

                // make sure king is not moving into danger
                if ( src.piece->type() == pieces::name_t::king ) {
                    if ( game_board.determine_threat( src, dst, dst, false ) ) {
                        return true;
                    }

                    // check castling through check
                    if ( dst.position() == pieces::position_t{ pieces::rank_t::eight, pieces::file_t::g } ) {
                        if ( possible_moves.empty() ||
                             std::find( possible_moves.begin(), possible_moves.end(),
                                        pieces::position_t{ pieces::rank_t::eight, pieces::file_t::f } ) ==
                                 possible_moves.end() ) {
                            return true;
                        }
                    }

                    if ( dst.position() == pieces::position_t{ pieces::rank_t::eight, pieces::file_t::c } ) {
                        if ( possible_moves.empty() ||
                             std::find( possible_moves.begin(), possible_moves.end(),
                                        pieces::position_t{ pieces::rank_t::eight, pieces::file_t::d } ) ==
                                 possible_moves.end() ) {
                            return true;
                        }
                    }
                }
                // otherwise, make sure the move is not entering check (or keeping us in check)
                else if ( game_board.determine_threat( src, dst, king_pos, false ) ) {
                    return true;
                }
            }

            return false;
        } );

        return pieces::move_status::valid;
    }

}  // namespace chess