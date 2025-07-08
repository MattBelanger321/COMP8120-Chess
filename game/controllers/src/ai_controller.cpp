#include "controller.hpp"
#include "game.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <ai_controller.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace chess::controller {
    ai_controller::ai_controller( chromosome_t chromie ) : controller(), chromosome( chromie ) {}

    float ai_controller::evaluate_position() const { return evaluate_position( game ); }

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

    float ai_controller::compute_material_score( const chess_game & game, const bool white ) const
    {
        float score = 0.f;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                float material_score = 0.f;

                if ( space.piece->colour() ) {
                    switch ( space.piece->type() ) {
                    case pieces::name_t::rook:
                        material_score += values::rook;
                        break;
                    case pieces::name_t::knight:
                        material_score += values::knight;
                        break;
                    case pieces::name_t::bishop:
                        material_score += values::bishop;
                        break;
                    case pieces::name_t::king:
                        material_score += values::king;
                        break;
                    case pieces::name_t::queen:
                        material_score += values::queen;
                        break;
                    case pieces::name_t::pawn:
                        material_score += values::pawn;
                        break;
                    }
                }

                if ( space.piece->colour() == white ) {
                    score += material_score * chromosome.material_score_bonus;
                }
                else {
                    score -= material_score * chromosome.material_score_bonus;
                }
            }
        }

        return score;
    }

    float ai_controller::compute_piece_mobility( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_castling_bonus( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_development_speed( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_doubled_pawn_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_isolated_pawn_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_connected_pawn_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_passed_pawn_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_king_pressure_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_piece_defense_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_bishop_pair_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_king_centralization_score( const chess_game & game, const bool white ) const
    {
        int bishop_count = 0;
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() && space.piece->colour() == white ) {
                    switch ( space.piece->type() ) {
                    case pieces::name_t::bishop:
                        bishop_count++;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        return bishop_count > 1;
    }
    float ai_controller::compute_knight_outpost_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_blocked_piece_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_space_control_score( const chess_game & game, const bool white ) const { return 0; }
    float ai_controller::compute_king_shield_score( const chess_game & game, const bool white ) const { return 0; }

    float ai_controller::evaluate_position( const chess_game & game, const bool white ) const
    {
        float score = 0;

        float material_score = compute_material_score( game, white );
        score += material_score * chromosome.material_score_bonus;

        float piece_mobility_score = compute_piece_mobility( game, white );
        score += piece_mobility_score * chromosome.piece_mobility_bonus;

        float castling_score = compute_castling_bonus( game, white );
        score += castling_score * chromosome.castling_bonus;

        float development_speed_score = compute_development_speed( game, white );
        score += development_speed_score * chromosome.development_speed_bonus;

        float doubled_pawn_score = compute_doubled_pawn_score( game, white );
        score += doubled_pawn_score * chromosome.doubled_pawn_penalty;

        float isolated_pawn_score = compute_isolated_pawn_score( game, white );
        score += isolated_pawn_score * chromosome.isolated_pawn_penalty;

        float connected_pawn_score = compute_connected_pawn_score( game, white );
        score += connected_pawn_score * chromosome.connected_pawn_bonus;

        float passed_pawn_score = compute_passed_pawn_score( game, white );
        score += passed_pawn_score * chromosome.passed_pawn_bonus;

        float enemy_king_pressure_score = compute_king_pressure_score( game, not white );
        score += enemy_king_pressure_score * chromosome.enemy_king_pressure_bonus;

        float piece_defense_score = compute_piece_defense_score( game, white );
        score += piece_defense_score * chromosome.piece_defense_bonus;

        float bishop_pair_score = compute_bishop_pair_score( game, white );
        score += bishop_pair_score * chromosome.bishop_pair_bonus;

        float connected_rooks_score = compute_connected_rooks_score( game, white );
        score += connected_rooks_score * chromosome.connected_rooks_bonus;

        float king_centralization_score = compute_king_centralization_score( game, white );
        score += king_centralization_score * chromosome.king_centralization_val;

        float knight_outpost_score = compute_knight_outpost_score( game, white );
        score += knight_outpost_score * chromosome.knight_outpost_bonus;

        float blocked_piece_score = compute_blocked_piece_score( game, white );
        score += blocked_piece_score * chromosome.blocked_piece_penalty;

        float space_control_score = compute_space_control_score( game, white );
        score += space_control_score * chromosome.space_control_in_opponent_half_bonus;

        float king_shield_score = compute_king_shield_score( game, white );
        score += king_shield_score * chromosome.king_shield_bonus;

        float king_pressure_score = -compute_king_pressure_score( game, white );
        score += king_pressure_score * chromosome.king_pressure_penalty;

        float pawn_position_score = 0
        for square in board.pieces(chess.PAWN, chess.WHITE):
            pawn_position_score += pawn_position_weights[square]
        for square in board.pieces(chess.PAWN, chess.BLACK):
            pawn_position_score -= pawn_position_weights[chess.square_mirror(square)]
        score += pawn_position_score
        
        float knight_position_score = 0
        for square in board.pieces(chess.KNIGHT, chess.WHITE):
            knight_position_score += knight_position_weights[square]
        for square in board.pieces(chess.KNIGHT, chess.BLACK):
            knight_position_score -= knight_position_weights[chess.square_mirror(square)]
        score += knight_position_score
        
        float bishop_position_score = 0
        for square in board.pieces(chess.BISHOP, chess.WHITE):
            bishop_position_score += bishop_position_weights[square]
        for square in board.pieces(chess.BISHOP, chess.BLACK):
            bishop_position_score -= bishop_position_weights[chess.square_mirror(square)]
        score += bishop_position_score
        
        float rook_position_score = 0
        for square in board.pieces(chess.ROOK, chess.WHITE):
            rook_position_score += rook_position_weights[square]
        for square in board.pieces(chess.ROOK, chess.BLACK):
            rook_position_score -= rook_position_weights[chess.square_mirror(square)]
        score += rook_position_score
        
        float queen_position_score = 0
        for square in board.pieces(chess.QUEEN, chess.WHITE):
            queen_position_score += queen_position_weights[square]
        for square in board.pieces(chess.QUEEN, chess.BLACK):
            queen_position_score -= queen_position_weights[chess.square_mirror(square)]
        score += queen_position_score
        
        float king_position_score = 0
        for square in board.pieces(chess.KING, chess.WHITE):
            king_position_score += king_position_weights[square]
        for square in board.pieces(chess.KING, chess.BLACK):
            king_position_score -= king_position_weights[chess.square_mirror(square)]
        score += king_position_score
            
        return score
        /*
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

        score += chromosome.king_safety_val * king_safety( game );

        score += position_score( game );

        return score;
        */
    }

    float ai_controller::position_score( const chess_game & game ) const
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

    float ai_controller::minimax( const chess_game & game, const int depth, float alpha, float beta,
                                  const bool maximizing_player ) const
    {
        if ( depth == 0 || game.get_state() == chess::game_state::white_wins ||
             game.get_state() == chess::game_state::black_wins || game.get_state() == chess::game_state::draw ) {
            return evaluate_position( game );
        }

        float max_eval;
        float min_eval;

        std::vector< move_t > legal_moves = game.legal_moves();
        if ( maximizing_player ) {
            max_eval = -std::numeric_limits< double >::infinity();

            for ( const move_t & move : legal_moves ) {
                chess_game possible_move = game;
                possible_move.move( move.first, move.second );
                float score = minimax( possible_move, depth - 1, alpha, beta, false );

                max_eval = std::max( max_eval, score );
                alpha    = std::max( alpha, score );
                if ( beta <= alpha ) {
                    break;
                }
            }

            return max_eval;
        }
        else {
            min_eval = std::numeric_limits< double >::infinity();

            for ( const move_t & move : legal_moves ) {
                chess_game possible_move = game;
                possible_move.move( move.first, move.second );
                float score = minimax( possible_move, depth - 1, alpha, beta, true );

                min_eval = std::min( min_eval, score );
                beta     = std::min( beta, score );
                if ( beta <= alpha ) {
                    break;
                }
            }

            return min_eval;
        }
    }

    move_t ai_controller::select_best_move( const int depth ) const
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

            chess_game possible_move = game;
            possible_move.move( move.first, move.second );
            float score = minimax( possible_move, depth - 1, -std::numeric_limits< double >::infinity(),
                                   std::numeric_limits< double >::infinity(), !current_turn );

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
            std::cout << "AI Online\n";

            if ( ( game.white_move() && game.checkmate( true ) ) || ( game.black_move() && game.checkmate( false ) ) ) {
                break;
            }

            if ( game.black_move() ) {
                std::this_thread::sleep_for( 1s );
                continue;
            }

            std::cout << "AI Selecting Move...";
            auto selected_move = select_best_move( 2 );
            std::cout << "AI selected move: " << to_string( selected_move.first.position() ) << " to "
                      << to_string( selected_move.second.position() ) << "\n";

            select_space( selected_move.first.position() );
            if ( move( selected_move.second ) != pieces::move_status::valid ) {
                std::cout << "Error: AI Selected Invalid move_t, AI Offline\n";
                return;
            }
        }

        std::cout << "AI Offline\n";
    }

    void ai_controller::activate()
    {
        should_close = false;
        runner       = std::thread( [this]() {
            try {
                play();
            }
            catch ( std::exception const & e ) {
                std::cout << "AI Crashed: " << e.what() << "\n";
            }
        } );
    }

    ai_controller::~ai_controller()
    {
        should_close = true;
        if ( runner.joinable() ) {
            runner.join();
        }
    }
}  // namespace chess::controller