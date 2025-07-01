#include "controller.hpp"
#include "game.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <ai_controller.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace chess::controller {
    ai_controller::ai_controller( chromosome_t chromie ) : controller(), chromosome( chromie ) {}

    float ai_controller::evaluate_position() { return evaluate_position( game ); }

    // TODO: add to game class
    std::vector< game::space > attacks( std::vector< move_t > const & legal_moves, pieces::position_t const & pos )
    {
        std::vector< game::space > attackers;

        for ( auto & move : legal_moves ) {
            if ( move.second.position() == pos ) {
                attackers.push_back( move.first );
            }
        }

        return attackers;
    }

    float ai_controller::king_saftey( const chess_game & game )
    {
        float       king_safety_penalty = 0.f;
        const float penalty             = 1.f;

        // 1. Find white king position
        pieces::position_t king_pos = game.white_king.get().position();

        int king_rank = static_cast< int >( king_pos.first );
        int king_file = static_cast< int >( king_pos.second );

        // 2. Check open file above king
        for ( int r = king_rank + 1; r <= 8; ++r ) {
            const auto & sp = game.get( pieces::piece::itopos( king_file, r ).value() );
            if ( sp.piece ) {
                if ( sp.piece->colour() )
                    break;
                king_safety_penalty += 0.8f * penalty;
                break;
            }
            else if ( r == 8 ) {
                king_safety_penalty += 0.8f * penalty;
            }
        }

        // 3. Adjacent open files
        for ( int f : { king_file - 1, king_file + 1 } ) {
            if ( f < 1 || f > 8 )
                continue;
            for ( int r = king_rank; r <= 8; ++r ) {
                const auto & sp = game.get( pieces::piece::itopos( f, r ).value() );
                if ( sp.piece ) {
                    if ( sp.piece->colour() )
                        break;
                    king_safety_penalty += 0.8f * penalty;
                    break;
                }
                else if ( r == 8 ) {
                    king_safety_penalty += 0.8f * penalty;
                }
            }
        }

        // 4. Evaluate adjacent squares

        // clang-format off
        const std::vector< std::pair< int, int > > adjacent_offsets = { 
            { -1, -1 }, { 0, -1 }, { 1, -1 },
            { -1, 0 },             { 1, 0 },
            { -1, 1 },  { 0, 1 },  { 1, 1 } 
        };
        // clang-format on

        for ( const auto & [dx, dy] : adjacent_offsets ) {
            int f = king_file + dx;
            int r = king_rank + dy;
            if ( f < 1 || f > 8 || r < 1 || r > 8 )
                continue;

            const auto & sp = game.get( pieces::piece::itopos( f, r ).value() );

            if ( !sp.piece ) {
                king_safety_penalty += 0.1f * penalty;
            }
            else if ( !sp.piece->colour() ) {
                switch ( sp.piece->type() ) {
                case pieces::name_t::pawn:
                    king_safety_penalty += 1.f;
                    break;
                case pieces::name_t::knight:
                case pieces::name_t::bishop:
                    king_safety_penalty += 3.f;
                    break;
                case pieces::name_t::rook:
                    king_safety_penalty += 5.f;
                    break;
                case pieces::name_t::queen:
                    king_safety_penalty += 9.f;
                    break;
                default:
                    break;
                }
            }
        }

        // 5. Not on edge penalty
        if ( king_rank != 1 ) {
            if ( king_file != 1 && king_file != 8 && king_rank != 8 ) {
                king_safety_penalty += 0.5f * penalty;
            }
            else {
                king_safety_penalty += 0.3f * penalty;
            }
        }

        // 6. Simulate black turn — can they check or checkmate?
        chess_game board_copy = game;
        board_copy.set_turn( false );

        auto black_moves = board_copy.legal_moves();
        for ( const auto & move : black_moves ) {
            chess_game try_board = board_copy;
            try_board.move( move.first, move.second );

            if ( try_board.checkmate( true ) ) {
                king_safety_penalty += 10.f * penalty;
            }
            else if ( try_board.get_state() == game_state::white_check ) {
                king_safety_penalty += 2.f * penalty;
            }
        }

        // 7. Try white king pseudo-legal moves (to see if most are under attack)
        std::vector< game::space > king_pseudo_moves = game.psuedo_possible_moves( game.get( king_pos ) );

        for ( const auto & move : king_pseudo_moves ) {
            chess_game try_board = game;
            try_board.move( game.get( king_pos ), move );

            if ( try_board.get_state() == game_state::white_check ) {
                king_safety_penalty += 1.f * penalty;
            }
        }

        return -king_safety_penalty;
    }

    float ai_controller::evaluate_position( const chess_game & game )
    {
        float score = 0.f;

        score += piece_score( game );

        auto legals = game.legal_moves();

        // if the is white that mean we are trying to choose
        //  a black move since the board is one move ahead of the actual
        score += ( game.white_move() ? -legals.size() : legals.size() ) * chromosome.piece_mobility_val;

        for ( auto const & pos : center_positions ) {
            const auto & sp = game.get( pos );
            if ( sp.piece ) {
                if ( sp.piece->colour() ) {
                    score += chromosome.center_control_val;
                }
                else {
                    score -= chromosome.center_control_val;
                }
            }
        }

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {  // loop over all the spaces
                auto & sp = game.get( pieces::piece::itopos( i, j ).value() );
                if ( sp.piece ) {
                    auto attackers = attacks( legals, sp.position() );
                    for ( auto const & c_pos : center_positions ) {
                        // if c_pos is in the
                        if ( std::ranges::find_if( attackers, [&c_pos]( const game::space & sp ) {
                                 return sp.position() == c_pos;
                             } ) != attackers.end() ) {

                            score += 0.5f * chromosome.center_control_val * ( sp.piece->colour() ? 1 : -1 );
                        }
                    }
                }
            }
        }

        score += chromosome.king_safety_val * king_saftey( game );

        score += position_score( game );

        return score;
    }

    float ai_controller::position_score( const chess_game & game )
    {
        float score = 0.f;

        for ( int i = 1; i <= 8; ++i ) {
            for ( int j = 1; j <= 8; ++j ) {
                auto & sp = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !sp.piece )
                    continue;  // skip empty squares

                bool is_white = sp.piece->colour();
                int  row      = i - 1;
                int  col      = j - 1;

                // For black pieces, mirror vertically
                if ( !is_white ) {
                    auto [mi, mj] = mirror_vertically( sp.position() );
                    row           = static_cast< int >( mi ) - 1;
                    col           = static_cast< int >( mj ) - 1;
                }

                float value = 0.0f;

                switch ( sp.piece->type() ) {
                case pieces::name_t::pawn:
                    value = chromosome.pawn_position_weights[row][col];
                    break;
                case pieces::name_t::knight:
                    value = chromosome.knight_position_weights[row][col];
                    break;
                case pieces::name_t::bishop:
                    value = chromosome.bishop_position_weights[row][col];
                    break;
                case pieces::name_t::rook:
                    value = chromosome.rook_position_weights[row][col];
                    break;
                case pieces::name_t::queen:
                    value = chromosome.queen_position_weights[row][col];
                    break;
                default:
                    break;  // skip kings or unknowns
                }

                if ( is_white ) {
                    score += value;
                }
                else {
                    score -= value;
                }
            }
        }
        return score;
    }

    float ai_controller::piece_score( const chess_game & game )
    {
        float score = 0.f;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                auto temp_score     = 0.f;
                auto material_score = 0.f;

                if ( space.piece->colour() ) {
                    switch ( space.get_piece()->type() ) {
                    case pieces::name_t::rook:
                        temp_score += 5;
                        material_score += chromosome.rook_val;
                        break;
                    case pieces::name_t::knight:
                        temp_score += 3;
                        material_score += chromosome.knight_val;
                        break;
                    case pieces::name_t::bishop:
                        temp_score += 3;
                        material_score += chromosome.bishop_val;
                        break;
                    case pieces::name_t::king:
                        break;
                    case pieces::name_t::queen:
                        temp_score += 9;
                        material_score += chromosome.queen_val;
                        break;
                    case pieces::name_t::pawn:
                        temp_score += 1;
                        material_score += chromosome.pawn_val;
                        break;
                    }
                }

                temp_score += material_score * chromosome.material_score_val;

                if ( !space.piece->colour() ) {
                    score += temp_score;
                }
                else {
                    score -= temp_score;
                }
            }
        }

        return score;
    }

    move_t ai_controller::select_best_move()
    {
        std::vector< move_t > legal_moves = game.legal_moves();

        if ( legal_moves.empty() ) {
            throw std::runtime_error( "No Legal Moves" );
        }

        std::optional< move_t > best_move;

        // black wants lower scores, white wants higher scores
        double best_score =
            game.white_move() ? -std::numeric_limits< double >::infinity() : std::numeric_limits< double >::infinity();

        for ( const move_t & move : legal_moves ) {
            // set if its whites turn
            bool current_turn = game.white_move();

            chess_game possible_move = game;  // this is a copy of the game board that the candidate move will be
                                              // applied to to evaluate the move

            possible_move.move( move.first, move.second );
            float score = evaluate_position( possible_move );

            if ( current_turn ) {  // White's turn
                if ( score > best_score ) {
                    best_score = score;
                    best_move  = move;
                }
            }
            else {  // Black's turn
                if ( score < best_score ) {
                    best_score = score;
                    best_move  = move;
                }
            }
        }

        return best_move.value();
    }

    void ai_controller::play()
    {
        using namespace std::chrono_literals;

        while ( !should_close ) {
            if ( game.checkmate( true ) || game.checkmate( false ) ) {
                return;
            }

            if ( game.black_move() ) {
                std::this_thread::sleep_for( 1s );
                continue;
            }

            auto selected_move = select_best_move();

            select_space( selected_move.first.position() );
            if ( move( selected_move.second ) != pieces::move_status::valid ) {
                std::cout << "Error: AI Selected Invalid move_t, AI Offline\n";
                return;
            }
        }
    }

    void ai_controller::activate()
    {
        should_close = false;
        runner       = std::thread( [this]() { play(); } );
    }

    ai_controller::~ai_controller()
    {
        should_close = true;
        if ( runner.joinable() ) {
            runner.join();
        }
    }
}  // namespace chess::controller