#include "controller.hpp"
#include "game.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <ai_controller.hpp>
#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace chess::controller {
    ai_controller::ai_controller( chromosome_t chromie ) : controller(), chromosome( chromie ) {}

    float ai_controller::evaluate_position() const { return evaluate_position( game, true ); }

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

    float ai_controller::compute_piece_mobility( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;
        auto  color = white;

        std::chrono::high_resolution_clock::time_point start, end, startl, endl;
        std::chrono::microseconds                      duration;

        start      = std::chrono::high_resolution_clock::now();
        auto moves = game.legal_moves();
        end        = std::chrono::high_resolution_clock::now();
        duration   = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        // std::cout << "Legal moves took " << duration.count() << " microseconds\n";
        // exit( 1 );

        // for ( int i = 1; i <= 8; i++ ) {
        //     for ( int j = 1; j <= 8; j++ ) {
        //         auto space = game.get( pieces::piece::itopos( i, j ).value() );
        //         if ( !space.piece ) {
        //             std::cout << "0 ";
        //         }
        //         else {
        //             switch ( space.piece->type() ) {
        //             case pieces::name_t::pawn:
        //                 std::cout << "P ";
        //                 break;
        //             case pieces::name_t::knight:
        //                 std::cout << "H ";
        //                 break;
        //             case pieces::name_t::bishop:
        //                 std::cout << "B ";
        //                 break;
        //             case pieces::name_t::rook:
        //                 std::cout << "R ";
        //                 break;
        //             case pieces::name_t::queen:
        //                 std::cout << "Q ";
        //                 break;
        //             case pieces::name_t::king:
        //                 std::cout << "K ";
        //                 break;
        //             }
        //         }
        //     }

        //     std::cout << "\n";
        // }
        // std::cout << "\n";
        chess_game::attack_map game_attack_map = game.game_attack_map;

        // std::cout << "in controller:\n";
        // std::cout << game_attack_map.to_string() << std::endl;
        // exit( 1 );
        // for ( int i = 1; i <= 8; i++ ) {
        //     for ( int j = 1; j <= 8; j++ ) {
        //         auto space = game.get( pieces::piece::itopos( i, j ).value() );

        //         if ( !( space.piece && space.piece->colour() == white ) ) {
        //             continue;
        //         }

        //         int num_attackers = game_attack_map.num_attackers( space, white ) ) {
        //             score++;
        //         }
        //     }
        // }
        // std::cout << score << std::endl;
        // exit( 1 );
        // score = 0.0f;
        // startl = std::chrono::high_resolution_clock::now();
        // std::cout << "num moves: " << moves.size() << std::endl;
        // std::cout << "white: " << white << "\n";

        for ( auto const & move : moves ) {
            int num_defenders = game_attack_map.num_attackers( move.second, white );
            int num_attackers = game_attack_map.num_attackers( move.second, !white );
            // std::cout << "Space " << pieces::to_string( move.second.position() ) << " is defended by " <<
            // num_defenders << " white pieces, and attacked by " << num_defenders << " black pieces\n";

            if ( num_defenders >= num_attackers ) {
                score++;
            }
            // else {
            //     std::cout << pieces::to_string( move.first.position() ) << pieces::to_string( move.second.position()
            //     ); std::cout << "\n";
            // }
        }
        // std::cout << score << std::endl;
        // score = 0.0f;
        // exit( 1 );
        // for ( auto const & move : moves ) {
        //     // Only consider moves by the player with the given color
        //     if ( move.first.piece && move.first.piece->colour() != color ) {
        //         continue;
        //     }

        //     // startl         = std::chrono::high_resolution_clock::now();
        //     auto attackers = game.find_attackers( move.second, !color );
        //     // endl           = std::chrono::high_resolution_clock::now();
        //     // duration       = std::chrono::duration_cast< std::chrono::microseconds >( endl - startl );
        //     // std::cout << "Find attackers took " << duration.count() << " microseconds\n";
        //     // exit( 1 );
        //     auto defenders = game.find_attackers( move.second, color );

        //     if ( attackers.size() <= defenders.size() ) {
        //         score++;
        //     }
        // }
        // std::cout << score << std::endl;
        // exit( 1 );
        // endl     = std::chrono::high_resolution_clock::now();
        // duration = std::chrono::duration_cast< std::chrono::microseconds >( endl - startl );
        // std::cout << "Piece mobility took " << duration.count() << " microseconds\n";
        // exit( 1 );
        return score;
    }

    float ai_controller::compute_castling_bonus( const chess_game & game, const bool white ) const
    {
        float score = 0;

        if ( game.can_castle( white ) ) {
            score += 1;
        }
        else {
            std::vector< std::string > move_history = game.get_move_history();
            if ( move_history.size() > 2 ) {
                std::string second_last = move_history.at( move_history.size() - 2 );
                if ( second_last == "O-O" || second_last == "O-O-O" ) {
                    score += 2;
                }
            }
        }

        return score;
    }

    float ai_controller::compute_development_speed( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;

        pieces::rank_t pawn_starting_rank;
        pieces::rank_t piece_starting_rank;

        if ( white ) {
            pawn_starting_rank  = pieces::rank_t::two;
            piece_starting_rank = pieces::rank_t::one;
        }
        else {
            pawn_starting_rank  = pieces::rank_t::seven;
            piece_starting_rank = pieces::rank_t::eight;
        }

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn ) {
                        if ( space.piece->position().first != pawn_starting_rank ) {
                            score += 0.5;
                        }
                    }
                    else {
                        if ( space.piece->position().first != piece_starting_rank ) {
                            score += 1.0;
                        }
                    }
                }
            }
        }

        return score;
    }

    float ai_controller::compute_doubled_pawn_score( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;

        std::map< pieces::file_t, int > pawn_counts;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn ) {
                        pawn_counts[space.piece->position().second]++;
                    }
                }
            }
        }

        for ( auto c : pawn_counts ) {
            if ( c.second > 1 ) {
                score -= ( c.second - 1 );
            }
        }

        return score;
    }

    float ai_controller::compute_isolated_pawn_score( const chess_game & game, const bool white ) const
    {
        float              score = 0.0f;
        std::vector< int > pawn_files;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn &&
                         std::find( pawn_files.begin(), pawn_files.end(),
                                    static_cast< int >( space.piece->position().second ) ) == pawn_files.end() ) {
                        pawn_files.push_back( static_cast< int >( space.piece->position().second ) );
                    }
                }
            }
        }

        bool isolated_left  = false;
        bool isolated_right = false;
        int  file;
        for ( int i = 0; i < pawn_files.size(); i++ ) {
            file = static_cast< int >( pawn_files[i] );
            isolated_left =
                file - 1 < 1 || std::find( pawn_files.begin(), pawn_files.end(), file - 1 ) == pawn_files.end();
            isolated_right =
                file + 1 > 8 || std::find( pawn_files.begin(), pawn_files.end(), file + 1 ) == pawn_files.end();

            if ( isolated_left && isolated_right ) {
                score -= 1;
            }
        }

        return score;
    }

    float ai_controller::compute_connected_pawn_score( const chess_game & game, const bool white ) const
    {
        float                      score = 0.0f;
        std::vector< game::space > pawn_squares;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn ) {
                        pawn_squares.push_back( space );
                    }
                }
            }
        }

        int rank_direction;
        if ( white ) {
            rank_direction = 1;
        }
        else {
            rank_direction = -1;
        }

        int            rank, file;
        pieces::rank_t behind_rank;
        for ( int i = 0; i < pawn_squares.size(); i++ ) {
            rank = static_cast< int >( pawn_squares[i].piece->position().first );
            file = static_cast< int >( pawn_squares[i].piece->position().second );

            if ( 1 <= rank + rank_direction && rank + rank_direction <= 8 && 1 <= file - 1 ) {

                auto & left_space = game.get( pieces::piece::itopos( rank + rank_direction, file - 1 ).value() );
                if ( left_space.piece && left_space.piece->colour() == white ) {
                    score += 1;
                }
            }

            if ( 1 <= rank + rank_direction && rank + rank_direction <= 8 && file + 1 <= 8 ) {
                auto & right_space = game.get( pieces::piece::itopos( rank + rank_direction, file + 1 ).value() );

                if ( right_space.piece && right_space.piece->colour() == white ) {
                    score += 1;
                }
            }
        }

        return score;
    }
    // TODO: need to check adjacent files
    float ai_controller::compute_passed_pawn_score( const chess_game & game, const bool white ) const
    {
        float                      score = 0.0f;
        std::vector< game::space > pawn_squares;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece )
                    continue;

                if ( space.piece->colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn ) {
                        pawn_squares.push_back( space );
                    }
                }
            }
        }

        int pawn_rank, pawn_file, blocked;
        for ( int i = 0; i < pawn_squares.size(); i++ ) {
            pawn_rank = static_cast< int >( pawn_squares[i].piece->position().first );
            pawn_file = static_cast< int >( pawn_squares[i].piece->position().second );
            blocked   = 0;
            if ( white ) {
                for ( int j = pawn_rank + 1; j <= 8; j++ ) {
                    auto & space = game.get( pieces::piece::itopos( j, pawn_file ).value() );
                    if ( space.piece && space.piece->colour() != white &&
                         space.piece->type() == pieces::name_t::pawn ) {
                        blocked = 1;
                    }
                }
            }
            else {
                for ( int j = pawn_rank - 1; j >= 1; j-- ) {
                    auto & space = game.get( pieces::piece::itopos( j, pawn_file ).value() );
                    if ( space.piece && space.piece->colour() != white &&
                         space.piece->type() == pieces::name_t::pawn ) {
                        blocked = 1;
                    }
                }
            }
            if ( !blocked ) {
                score += 1;
            }
        }

        return score;
    }

    float ai_controller::compute_king_pressure_score( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;

        int king_rank, king_file;

        pieces::position_t king_position;
        if ( white ) {
            king_position = game.white_king.get().position();
        }
        else {
            king_position = game.black_king.get().position();
        }

        king_rank             = static_cast< int >( king_position.first );
        king_file             = static_cast< int >( king_position.second );
        auto &     king_space = game.get( pieces::piece::itopos( king_rank, king_file ).value() );
        std::array directions = { std::pair{ -1, -1 }, std::pair{ 0, -1 }, std::pair{ 1, -1 }, std::pair{ -1, 0 },
                                  std::pair{ 1, 0 },   std::pair{ -1, 1 }, std::pair{ 0, 1 },  std::pair{ 1, 1 } };

        chess_game::attack_map game_attack_map = game.game_attack_map;

        for ( const auto & [df, dr] : directions ) {
            int f = king_file + df;
            int r = king_rank + dr;
            if ( f >= 1 && f <= 8 && r >= 1 && r <= 8 ) {
                auto pos = game.get( pieces::piece::itopos( r, f ).value() );

                if ( game_attack_map.has_attackers( pos, white ) ) {
                    score -= 1;
                }
            }
        }

        chess_game temp_game = game;
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = temp_game.get( pieces::piece::itopos( i, j ).value() );

                if ( space.piece && space.piece->colour() == white && space.piece->type() != pieces::name_t::king ) {
                    temp_game.remove_piece_at( space.piece->position() );
                }
            }
        }

        chess_game::attack_map temp_attack_map = game.generate_attack_map( temp_game.get_board() );
        score -= temp_attack_map.has_attackers( king_space, white );

        return score;
    }

    float ai_controller::compute_piece_defense_score( const chess_game & game, const bool white ) const
    {
        chess_game::attack_map game_attack_map = game.game_attack_map;
        float                  score           = 0.0f;
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( !( space.piece && space.piece->colour() == white ) ) {
                    continue;
                }

                if ( game_attack_map.has_attackers( space, white ) ) {
                    score++;
                }
            }
        }
        return score;
    }

    float ai_controller::compute_connected_rooks_score( const chess_game & game, const bool white ) const
    {
        float                      score = 0.0f;
        std::vector< game::space > rooks;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( space.piece && space.piece->colour() == white && space.piece->type() == pieces::name_t::rook ) {
                    rooks.push_back( space );
                }
            }
        }

        int rook1_rank, rook2_rank;
        int rook1_file, rook2_file;

        for ( int i = 0; i < rooks.size(); i++ ) {
            for ( int j = i + 1; j < rooks.size(); j++ ) {
                rook1_rank = static_cast< int >( rooks[i].position().first );
                rook1_file = static_cast< int >( rooks[i].position().second );

                rook2_rank = static_cast< int >( rooks[j].position().first );
                rook2_file = static_cast< int >( rooks[j].position().second );

                if ( rook1_rank == rook2_rank ) {
                    int step    = ( rook1_file < rook2_file ) ? 1 : -1;
                    int blocked = 0;
                    for ( int file = rook1_file + step; file != rook2_file; file += step ) {
                        auto s = pieces::piece::itopos( rook1_rank, file );
                        if ( !s.has_value() )
                            continue;

                        auto & space = game.get( s.value() );
                        if ( space.piece ) {
                            blocked = 1;
                            break;
                        }
                    }

                    if ( !blocked ) {
                        score += 1;
                    }
                }
                else if ( rook1_file == rook2_file ) {
                    int step    = ( rook1_rank < rook2_rank ) ? 1 : -1;
                    int blocked = 0;
                    for ( int rank = rook1_rank + step; rank != rook2_rank; rank += step ) {
                        auto s = pieces::piece::itopos( rank, rook1_file );
                        if ( !s.has_value() )
                            continue;

                        auto & space = game.get( s.value() );
                        if ( space.piece ) {
                            blocked = 1;
                            break;
                        }
                    }

                    if ( !blocked ) {
                        score += 1;
                    }
                }
            }
        }

        return score;
    }
    float ai_controller::compute_bishop_pair_score( const chess_game & game, const bool white ) const
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
    float ai_controller::compute_king_centralization_score( const chess_game & game, const bool white ) const
    {
        float              score = 0.0f;
        pieces::position_t king_position;

        if ( white ) {
            king_position = game.white_king.get().position();
        }
        else {
            king_position = game.black_king.get().position();
        }

        int king_rank = static_cast< int >( king_position.first );
        int king_file = static_cast< int >( king_position.second );

        std::vector< std::pair< int, int > > center_squares = { { 4, 4 }, { 5, 4 }, { 4, 5 }, { 5, 5 } };

        int min_distance = 8;  // Max number of moves across the board for the king cannot be more than 8
        for ( const auto & [cf, cr] : center_squares ) {
            int distance = std::max( std::abs( king_file - cf ), std::abs( king_rank - cr ) );
            min_distance = std::min( distance, min_distance );
        }

        int centralization_score = -min_distance;

        int distance_from_opposite_side;
        if ( white ) {
            distance_from_opposite_side = 7 - king_rank;
        }
        else {
            distance_from_opposite_side = king_rank;
        }

        score = centralization_score - distance_from_opposite_side * 0.25;

        return score;
    }
    float ai_controller::compute_knight_outpost_score( const chess_game & game, const bool white ) const
    {
        float                      score = 0.0f;
        std::vector< game::space > knights;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( space.piece && space.piece->colour() == white && space.piece->type() == pieces::name_t::knight ) {
                    knights.push_back( space );
                }
            }
        }

        for ( int i = 0; i < knights.size(); i++ ) {
            int knight_rank = static_cast< int >( knights[i].position().first );
            if ( ( white && knight_rank <= 4 ) || ( !white && knight_rank >= 5 ) ) {
                continue;
            }

            // check if a friendly pawn is defending the knight
            int  defended_by_pawn = 0;
            auto defenders        = game.find_attackers( knights[i], !white );
            for ( auto defender : defenders ) {
                if ( defender.piece->type() == pieces::name_t::pawn ) {
                    defended_by_pawn = 1;
                }
            }

            if ( !defended_by_pawn ) {
                continue;
            }

            // check if being attacked by enemy pawn
            int  attacked_by_enemy_pawn = 0;
            auto attackers              = game.find_attackers( knights[i], white );

            for ( auto attacker : attackers ) {
                if ( attacker.piece->type() == pieces::name_t::pawn ) {
                    attacked_by_enemy_pawn = 1;
                }
            }

            if ( attacked_by_enemy_pawn ) {
                continue;
            }

            score += 1;
        }

        return score;
    }
    float ai_controller::compute_blocked_piece_score( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;

        std::vector< game::space > possible_moves;
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                if ( space.piece && space.piece->colour() == white ) {
                    switch ( space.piece->type() ) {
                    case pieces::name_t::bishop:
                    case pieces::name_t::knight:
                    case pieces::name_t::rook:
                    case pieces::name_t::queen:
                        game.possible_moves( space, possible_moves );
                        if ( possible_moves.size() == 0 ) {
                            score += 1;
                        }

                        possible_moves.clear();
                        break;
                    default:
                        continue;
                    }
                }
            }
        }
        return score;
    }
    float ai_controller::compute_space_control_score( const chess_game & game, const bool white ) const
    {
        float                      score = 0.0f;
        std::vector< game::space > spaces;
        if ( white ) {
            for ( int i = 4; i <= 8; i++ ) {
                for ( int j = 1; j <= 8; j++ ) {
                    auto & space = game.get( pieces::piece::itopos( i, j ).value() );
                    spaces.push_back( space );
                }
            }
        }
        else {
            for ( int i = 1; i <= 4; i++ ) {
                for ( int j = 1; j <= 8; j++ ) {
                    auto & space = game.get( pieces::piece::itopos( i, j ).value() );
                    spaces.push_back( space );
                }
            }
        }

        chess_game::attack_map game_attack_map = game.game_attack_map;
        for ( auto space : spaces ) {
            if ( game_attack_map.has_attackers( space, white ) ) {
                score += 1;
            }
            // auto attackers = game.find_attackers( space, !white );
            // if ( attackers.size() != 0 ) {
            //     score += 1;
            // }
        }
        return score;
    }
    float ai_controller::compute_king_shield_score( const chess_game & game, const bool white ) const
    {
        float              score = 0.0f;
        pieces::position_t king_position;
        if ( white ) {
            king_position = game.white_king.get().position();
        }
        else {
            king_position = game.black_king.get().position();
        }

        int king_rank = static_cast< int >( king_position.first );
        int king_file = static_cast< int >( king_position.second );

        int forward_rank;
        if ( white ) {
            forward_rank = king_rank + 1;
        }
        else {
            forward_rank = king_rank - 1;
        }

        for ( int i = king_file - 1; i <= king_file + 1; i++ ) {
            if ( 1 <= i && i <= 8 ) {
                auto & space = game.get( pieces::piece::itopos( forward_rank, i ).value() );
                if ( space.piece && space.colour() == white ) {
                    if ( space.piece->type() == pieces::name_t::pawn ) {
                        score += 1;
                    }
                    else {
                        score += 2;
                    }
                }
            }
        }

        return score;
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
    float ai_controller::evaluate_position( const chess_game & game, const bool white ) const
    {
        chess_game game_copy = game;
        game_copy.set_turn( white );

        float                                          score = 0;
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::microseconds                      duration;

        start                = std::chrono::high_resolution_clock::now();
        float material_score = compute_material_score( game_copy, white );
        end                  = std::chrono::high_resolution_clock::now();
        duration             = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += material_score * chromosome.material_score_bonus;
        // std::cout << "material_score_time " << duration.count() << " microseconds\n";
        // std::cout << "material_score: " << material_score << std::endl;

        start                      = std::chrono::high_resolution_clock::now();
        float piece_mobility_score = compute_piece_mobility( game_copy, white );
        end                        = std::chrono::high_resolution_clock::now();
        duration                   = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += piece_mobility_score * chromosome.piece_mobility_bonus;
        // std::cout << "piece_mobility_score " << duration.count() << " microseconds\n";
        // std::cout << "piece_mobility_score: " << piece_mobility_score << std::endl;

        start                = std::chrono::high_resolution_clock::now();
        float castling_score = compute_castling_bonus( game_copy, white );
        end                  = std::chrono::high_resolution_clock::now();
        duration             = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += castling_score * chromosome.castling_bonus;
        // std::cout << "castling_score " << duration.count() << " microseconds\n";
        // std::cout << "castling_score: " << castling_score << std::endl;

        start                         = std::chrono::high_resolution_clock::now();
        float development_speed_score = compute_development_speed( game_copy, white );
        end                           = std::chrono::high_resolution_clock::now();
        duration                      = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += development_speed_score * chromosome.development_speed_bonus;
        // std::cout << "development_speed_score " << duration.count() << " microseconds\n";
        // std::cout << "development_speed_score: " << development_speed_score << std::endl;

        start                    = std::chrono::high_resolution_clock::now();
        float doubled_pawn_score = compute_doubled_pawn_score( game_copy, white );
        end                      = std::chrono::high_resolution_clock::now();
        duration                 = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += doubled_pawn_score * chromosome.doubled_pawn_penalty;
        // std::cout << "doubled_pawn_score " << duration.count() << " microseconds\n";
        // std::cout << "doubled_pawn_score: " << doubled_pawn_score << std::endl;

        start                     = std::chrono::high_resolution_clock::now();
        float isolated_pawn_score = compute_isolated_pawn_score( game_copy, white );
        end                       = std::chrono::high_resolution_clock::now();
        duration                  = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += isolated_pawn_score * chromosome.isolated_pawn_penalty;
        // std::cout << "isolated_pawn_score " << duration.count() << " microseconds\n";
        // std::cout << "isolated_pawn_score: " << isolated_pawn_score << std::endl;

        start                      = std::chrono::high_resolution_clock::now();
        float connected_pawn_score = compute_connected_pawn_score( game_copy, white );
        end                        = std::chrono::high_resolution_clock::now();
        duration                   = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += connected_pawn_score * chromosome.connected_pawn_bonus;
        // std::cout << "connected_pawn_score " << duration.count() << " microseconds\n";
        // std::cout << "connected_pawn_score: " << connected_pawn_score << std::endl;

        start                   = std::chrono::high_resolution_clock::now();
        float passed_pawn_score = compute_passed_pawn_score( game_copy, white );
        end                     = std::chrono::high_resolution_clock::now();
        duration                = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += passed_pawn_score * chromosome.passed_pawn_bonus;
        // std::cout << "passed_pawn_score " << duration.count() << " microseconds\n";
        // std::cout << "passed_pawn_score: " << passed_pawn_score << std::endl;

        start                           = std::chrono::high_resolution_clock::now();
        float enemy_king_pressure_score = compute_king_pressure_score( game_copy, not white );
        end                             = std::chrono::high_resolution_clock::now();
        duration                        = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += enemy_king_pressure_score * chromosome.enemy_king_pressure_bonus;
        // std::cout << "enemy_king_pressure_score " << duration.count() << " microseconds\n";
        // std::cout << "enemy_king_pressure_score: " << enemy_king_pressure_score << std::endl;

        start                     = std::chrono::high_resolution_clock::now();
        float piece_defense_score = compute_piece_defense_score( game_copy, white );
        end                       = std::chrono::high_resolution_clock::now();
        duration                  = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += piece_defense_score * chromosome.piece_defense_bonus;
        // std::cout << "piece_defense_score " << duration.count() << " microseconds\n";
        // std::cout << "piece_defense_score: " << piece_defense_score << std::endl;

        start                   = std::chrono::high_resolution_clock::now();
        float bishop_pair_score = compute_bishop_pair_score( game_copy, white );
        end                     = std::chrono::high_resolution_clock::now();
        duration                = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += bishop_pair_score * chromosome.bishop_pair_bonus;
        // std::cout << "bishop_pair_score " << duration.count() << " microseconds\n";
        // std::cout << "bishop_pair_score: " << bishop_pair_score << std::endl;

        start                       = std::chrono::high_resolution_clock::now();
        float connected_rooks_score = compute_connected_rooks_score( game_copy, white );
        end                         = std::chrono::high_resolution_clock::now();
        duration                    = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += connected_rooks_score * chromosome.connected_rooks_bonus;
        // std::cout << "connected_rooks_score " << duration.count() << " microseconds\n";
        // std::cout << "connected_rooks_score: " << connected_rooks_score << std::endl;

        start                           = std::chrono::high_resolution_clock::now();
        float king_centralization_score = compute_king_centralization_score( game_copy, white );
        end                             = std::chrono::high_resolution_clock::now();
        duration                        = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += king_centralization_score * chromosome.king_centralization_val;
        // std::cout << "king_centralization_score " << duration.count() << " microseconds\n";
        // std::cout << "king_centralization_score: " << king_centralization_score << std::endl;

        start                      = std::chrono::high_resolution_clock::now();
        float knight_outpost_score = compute_knight_outpost_score( game_copy, white );
        end                        = std::chrono::high_resolution_clock::now();
        duration                   = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += knight_outpost_score * chromosome.knight_outpost_bonus;
        // std::cout << "knight_outpost_score " << duration.count() << " microseconds\n";
        // std::cout << "knight_outpost_score: " << knight_outpost_score << std::endl;

        start                     = std::chrono::high_resolution_clock::now();
        float blocked_piece_score = compute_blocked_piece_score( game_copy, white );
        end                       = std::chrono::high_resolution_clock::now();
        duration                  = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += blocked_piece_score * chromosome.blocked_piece_penalty;
        // std::cout << "blocked_piece_score " << duration.count() << " microseconds\n";
        // std::cout << "blocked_piece_score: " << blocked_piece_score << std::endl;

        start                     = std::chrono::high_resolution_clock::now();
        float space_control_score = compute_space_control_score( game_copy, white );
        end                       = std::chrono::high_resolution_clock::now();
        duration                  = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += space_control_score * chromosome.space_control_in_opponent_half_bonus;
        // std::cout << "space_control_score " << duration.count() << " microseconds\n";
        // std::cout << "space_control_score: " << space_control_score << std::endl;

        start                   = std::chrono::high_resolution_clock::now();
        float king_shield_score = compute_king_shield_score( game_copy, white );
        end                     = std::chrono::high_resolution_clock::now();
        duration                = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += king_shield_score * chromosome.king_shield_bonus;
        // std::cout << "king_shield_score " << duration.count() << " microseconds\n";
        // std::cout << "king_shield_score: " << king_shield_score << std::endl;

        start                     = std::chrono::high_resolution_clock::now();
        float king_pressure_score = -compute_king_pressure_score( game_copy, white );
        end                       = std::chrono::high_resolution_clock::now();
        duration                  = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += king_pressure_score * chromosome.king_pressure_penalty;
        // std::cout << "king_pressure_score " << duration.count() << " microseconds\n";
        // std::cout << "king_pressure_score: " << king_pressure_score << std::endl;

        start                 = std::chrono::high_resolution_clock::now();
        float position_scores = position_score( game_copy );
        // std::cout << "position_scores: " << position_scores;
        end      = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast< std::chrono::microseconds >( end - start );
        score += position_scores;
        // std::cout << "position_scores " << duration.count() << " microseconds\n\n\n";
        // exit( 1 );
        // std::cout << "final score: " << score << std::endl;
        return score;
    }

    float ai_controller::minimax( const chess_game & game, const int depth, float alpha, float beta,
                                  const bool maximizing_player ) const
    {
        if ( depth == 0 || game.get_state() == chess::game_state::white_wins ||
             game.get_state() == chess::game_state::black_wins || game.get_state() == chess::game_state::draw ) {
            return evaluate_position( game, maximizing_player );
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
        auto m = best_move.value();

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

            game.update_attack_map();
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