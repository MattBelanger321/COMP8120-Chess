#include "controller.hpp"
#include "game.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <ai_controller.hpp>
#include <algorithm>
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

    float ai_controller::compute_piece_mobility( const chess_game & game, const bool white ) const
    {
        int  score = 0;
        auto color = white;

        auto moves = game.legal_moves();

        for ( auto const & move : moves ) {
            // Only consider moves by the player with the given color
            if ( move.first.piece && move.first.piece->colour() != color ) {
                continue;
            }

            auto attackers = game.find_attackers( move.second, !color );
            auto defenders = game.find_attackers( move.second, color );

            if ( attackers.size() <= defenders.size() ) {
                score++;
            }
        }

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

        for ( const auto & [df, dr] : directions ) {
            int f = king_file + df;
            int r = king_rank + dr;
            if ( f >= 1 && f <= 8 && r >= 1 && r <= 8 ) {
                auto pos = game.get( pieces::piece::itopos( r, f ).value() );

                if ( pos.piece && game.find_attackers( pos, white ).size() > 0 ) {
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

        score -= temp_game.find_attackers( king_space, white ).size();

        return score;
    }

    float ai_controller::compute_piece_defense_score( const chess_game & game, const bool white ) const
    {
        float score = 0.0f;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game.get( pieces::piece::itopos( i, j ).value() );

                auto attackers = game.find_attackers( space, !white );

                if ( attackers.size() > 0 ) {
                    score += 1;
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

        for ( auto space : spaces ) {
            auto attackers = game.find_attackers( space, !white );
            if ( attackers.size() != 0 ) {
                score += 1;
            }
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
        float score = 0;

        float material_score = compute_material_score( game, white );
        score += material_score * chromosome.material_score_bonus;
        std::cout << "material_score: " << material_score << std::endl;

        float piece_mobility_score = compute_piece_mobility( game, white );
        score += piece_mobility_score * chromosome.piece_mobility_bonus;
        std::cout << "piece_mobility_score: " << piece_mobility_score << std::endl;

        float castling_score = compute_castling_bonus( game, white );
        score += castling_score * chromosome.castling_bonus;
        std::cout << "castling_score: " << castling_score << std::endl;

        float development_speed_score = compute_development_speed( game, white );
        score += development_speed_score * chromosome.development_speed_bonus;
        std::cout << "development_speed_score: " << development_speed_score << std::endl;

        float doubled_pawn_score = compute_doubled_pawn_score( game, white );
        score += doubled_pawn_score * chromosome.doubled_pawn_penalty;
        std::cout << "doubled_pawn_score: " << doubled_pawn_score << std::endl;

        float isolated_pawn_score = compute_isolated_pawn_score( game, white );
        score += isolated_pawn_score * chromosome.isolated_pawn_penalty;
        std::cout << "isolated_pawn_score: " << isolated_pawn_score << std::endl;

        float connected_pawn_score = compute_connected_pawn_score( game, white );
        score += connected_pawn_score * chromosome.connected_pawn_bonus;
        std::cout << "connected_pawn_score: " << connected_pawn_score << std::endl;

        float passed_pawn_score = compute_passed_pawn_score( game, white );
        score += passed_pawn_score * chromosome.passed_pawn_bonus;
        std::cout << "passed_pawn_score: " << passed_pawn_score << std::endl;

        float enemy_king_pressure_score = compute_king_pressure_score( game, not white );
        score += enemy_king_pressure_score * chromosome.enemy_king_pressure_bonus;
        std::cout << "enemy_king_pressure_score: " << enemy_king_pressure_score << std::endl;

        float piece_defense_score = compute_piece_defense_score( game, white );
        score += piece_defense_score * chromosome.piece_defense_bonus;
        std::cout << "piece_defense_score: " << piece_defense_score << std::endl;

        float bishop_pair_score = compute_bishop_pair_score( game, white );
        score += bishop_pair_score * chromosome.bishop_pair_bonus;
        std::cout << "bishop_pair_score: " << bishop_pair_score << std::endl;

        float connected_rooks_score = compute_connected_rooks_score( game, white );
        score += connected_rooks_score * chromosome.connected_rooks_bonus;
        std::cout << "connected_rooks_score: " << connected_rooks_score << std::endl;

        float king_centralization_score = compute_king_centralization_score( game, white );
        score += king_centralization_score * chromosome.king_centralization_val;
        std::cout << "king_centralization_score: " << king_centralization_score << std::endl;

        float knight_outpost_score = compute_knight_outpost_score( game, white );
        score += knight_outpost_score * chromosome.knight_outpost_bonus;
        std::cout << "knight_outpost_score: " << knight_outpost_score << std::endl;

        float blocked_piece_score = compute_blocked_piece_score( game, white );
        score += blocked_piece_score * chromosome.blocked_piece_penalty;
        std::cout << "blocked_piece_score: " << blocked_piece_score << std::endl;

        float space_control_score = compute_space_control_score( game, white );
        score += space_control_score * chromosome.space_control_in_opponent_half_bonus;
        std::cout << "space_control_score: " << space_control_score << std::endl;

        float king_shield_score = compute_king_shield_score( game, white );
        score += king_shield_score * chromosome.king_shield_bonus;
        std::cout << "king_shield_score: " << king_shield_score << std::endl;

        float king_pressure_score = -compute_king_pressure_score( game, white );
        score += king_pressure_score * chromosome.king_pressure_penalty;
        std::cout << "king_pressure_score: " << king_pressure_score << std::endl;

        float position_scores = position_score( game );
        std::cout << "position_scores: " << position_scores;
        score += position_scores;

        std::cout << "final score: " << score << std::endl;
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
            auto selected_move = select_best_move( 1 );
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