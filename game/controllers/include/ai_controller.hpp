#ifndef __CHESS__CONTROLLER__AI_CONTROLLER__
#define __CHESS__CONTROLLER__AI_CONTROLLER__

#include "board.hpp"
#include "game.hpp"
#include "knight.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <array>
#include <controller.hpp>

#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>

namespace chess::controller {
    struct chromosome_t {
        float material_score_bonus;
        float piece_mobility_bonus;
        float castling_bonus;
        float development_speed_bonus;
        float doubled_pawn_penalty;
        float isolated_pawn_penalty;
        float connected_pawn_bonus;
        float passed_pawn_bonus;
        float enemy_king_pressure_bonus;
        float piece_defense_bonus;
        float bishop_pair_bonus;
        float connected_rooks_bonus;
        float king_centralization_val;
        float knight_outpost_bonus;
        float blocked_piece_penalty;
        float space_control_in_opponent_half_bonus;
        float king_shield_bonus;
        float king_pressure_penalty;

        using grid_t = std::array< std::array< float, 8 >, 8 >;

        grid_t pawn_position_weights;
        grid_t knight_position_weights;
        grid_t bishop_position_weights;
        grid_t rook_position_weights;
        grid_t queen_position_weights;
        grid_t king_position_weights;

        chromosome_t( const std::vector< float > & chromosome )
        {
            constexpr size_t expected_size = 18 + 6 * 64;
            if ( chromosome.size() != expected_size ) {
                std::cout << "This exception doesn't get received ever, so it's a silent death" << std::flush;
                std::cout << expected_size << " " << chromosome.size() << std::flush;
                throw std::invalid_argument( "Chromosome size " + std::to_string( chromosome.size() ) +
                                             " invalid, expected " + std::to_string( expected_size ) + "\n" );
            }

            material_score_bonus                 = chromosome[0];
            piece_mobility_bonus                 = chromosome[1];
            castling_bonus                       = chromosome[2];
            development_speed_bonus              = chromosome[3];
            doubled_pawn_penalty                 = chromosome[4];
            isolated_pawn_penalty                = chromosome[5];
            connected_pawn_bonus                 = chromosome[6];
            passed_pawn_bonus                    = chromosome[7];
            enemy_king_pressure_bonus            = chromosome[8];
            piece_defense_bonus                  = chromosome[9];
            bishop_pair_bonus                    = chromosome[10];
            connected_rooks_bonus                = chromosome[11];
            king_centralization_val              = chromosome[12];
            knight_outpost_bonus                 = chromosome[13];
            blocked_piece_penalty                = chromosome[14];
            space_control_in_opponent_half_bonus = chromosome[15];
            king_shield_bonus                    = chromosome[16];
            king_pressure_penalty                = chromosome[17];

            size_t offset = 18;
            assign_grid( pawn_position_weights, chromosome, offset );
            offset += 64;
            assign_grid( knight_position_weights, chromosome, offset );
            offset += 64;
            assign_grid( bishop_position_weights, chromosome, offset );
            offset += 64;
            assign_grid( rook_position_weights, chromosome, offset );
            offset += 64;
            assign_grid( queen_position_weights, chromosome, offset );
            offset += 64;
            assign_grid( king_position_weights, chromosome, offset );
        }

    private:
        void assign_grid( grid_t & table, const std::vector< float > & data, size_t offset )
        {
            for ( size_t row = 0; row < 8; ++row ) {
                for ( size_t col = 0; col < 8; ++col ) {
                    table[row][col] = data[offset + row * 8 + col];
                }
            }
        }
    };

    namespace values {
        constexpr int pawn   = 1;
        constexpr int bishop = 3;
        constexpr int knight = 3;
        constexpr int rook   = 5;
        constexpr int queen  = 9;
        constexpr int king   = 0;
    }  // namespace values

    // E4, E5, D4.D5
    const std::array< pieces::position_t, 4 > center_positions = {
        *pieces::piece::itopos( 4, 4 ), *pieces::piece::itopos( 4, 5 ), *pieces::piece::itopos( 5, 5 ),
        *pieces::piece::itopos( 5, 4 ) };

    class ai_controller : public controller {
    private:
        chromosome_t chromosome;
        struct zobrist_t {
            uint64_t piece_square[12][64];
            uint64_t castling_availability[4];
            uint64_t white_to_move;

            zobrist_t()
            {
                std::mt19937_64                           rng( 20250517 );
                std::uniform_int_distribution< uint64_t > distribution;

                for ( int i = 0; i < 12; i++ ) {
                    for ( int j = 0; j < 64; j++ ) {
                        piece_square[i][j] = distribution( rng );
                    }
                }

                for ( int i = 0; i < 4; i++ ) {
                    castling_availability[i] = distribution( rng );
                }

                white_to_move = distribution( rng );
            }
        };
        zobrist_t zobrist_hash;

        struct cache_entry {
            float score;
            int   depth;
        };

        mutable std::unordered_map< uint64_t, cache_entry > position_cache;

        bool        should_close;
        std::thread runner;

        void play();

        uint64_t compute_zobrist_hash( const game::board & b ) const;
        float    compute_material_score( const chess_game & game, const bool white ) const;
        float    compute_piece_mobility( const chess_game & game, const bool white ) const;
        float    compute_castling_bonus( const chess_game & game, const bool white ) const;
        float    compute_development_speed( const chess_game & game, const bool white ) const;
        float    compute_doubled_pawn_score( const chess_game & game, const bool white ) const;
        float    compute_isolated_pawn_score( const chess_game & game, const bool white ) const;
        float    compute_connected_pawn_score( const chess_game & game, const bool white ) const;
        float    compute_passed_pawn_score( const chess_game & game, const bool white ) const;
        float    compute_king_pressure_score( const chess_game & game, const bool white ) const;
        float    compute_piece_defense_score( const chess_game & game, const bool white ) const;
        float    compute_bishop_pair_score( const chess_game & game, const bool white ) const;
        float    compute_connected_rooks_score( const chess_game & game, const bool white ) const;
        float    compute_king_centralization_score( const chess_game & game, const bool white ) const;
        float    compute_knight_outpost_score( const chess_game & game, const bool white ) const;
        float    compute_blocked_piece_score( const chess_game & game, const bool white ) const;
        float    compute_space_control_score( const chess_game & game, const bool white ) const;
        float    compute_king_shield_score( const chess_game & game, const bool white ) const;

        float  move_score( const chess_game & game, const move_t move ) const;
        float  order_moves( const chess_game & game ) const;
        float  minimax( chess_game & game, const int depth, float alpha, float beta,
                        const bool maximizing_player ) const;
        move_t select_best_move( const int depth ) const;
        float  evaluate_position() const;
        float  evaluate_position( const chess_game & board, const bool white ) const;
        // returns a value that should be added to the score of the entire position
        float position_score( const chess_game & game ) const;

    public:
        ai_controller( chromosome_t chromosome );

        ~ai_controller();

        void activate();
    };
}  // namespace chess::controller

#endif