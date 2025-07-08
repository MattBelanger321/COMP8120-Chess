#ifndef __CHESS__CONTROLLER__AI_CONTROLLER__
#define __CHESS__CONTROLLER__AI_CONTROLLER__

#include "board.hpp"
#include "game.hpp"
#include "knight.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <array>
#include <controller.hpp>

#include <string>
#include <thread>

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
            constexpr size_t expected_size = 9 + 6 * 64;
            if ( chromosome.size() < expected_size ) {
                throw std::invalid_argument( "Chromosome must have at least 393 elements." );
            }

            float material_score_bonus                 = chromosome[0];
            float piece_mobility_bonus                 = chromosome[1];
            float castling_bonus                       = chromosome[2];
            float development_speed_bonus              = chromosome[3];
            float doubled_pawn_penalty                 = chromosome[4];
            float isolated_pawn_penalty                = chromosome[5];
            float connected_pawn_penalty               = chromosome[6];
            float passed_pawn_bonus                    = chromosome[7];
            float enemy_king_pressure_bonus            = chromosome[8];
            float piece_defense_bonus                  = chromosome[9];
            float bishop_pair_bonus                    = chromosome[10];
            float connected_rooks_bonus                = chromosome[11];
            float king_centralization_val              = chromosome[12];
            float knight_outpost_bonus                 = chromosome[13];
            float blocked_piece_penalty                = chromosome[14];
            float space_control_in_opponent_half_bonus = chromosome[15];
            float king_shield_bonus                    = chromosome[16];
            float king_pressure_penalty                = chromosome[17];

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

        bool        should_close;
        std::thread runner;

        void play();

        float compute_material_score( const chess_game & game, const bool white ) const;
        float compute_piece_mobility( const chess_game & game, const bool white ) const;
        float compute_castling_bonus( const chess_game & game, const bool white ) const;
        float compute_development_speed( const chess_game & game, const bool white ) const;
        float compute_doubled_pawn_score( const chess_game & game, const bool white ) const;
        float compute_isolated_pawn_score( const chess_game & game, const bool white ) const;
        float compute_connected_pawn_score( const chess_game & game, const bool white ) const;
        float compute_passed_pawn_score( const chess_game & game, const bool white ) const;
        float compute_king_pressure_score( const chess_game & game, const bool white ) const;
        float compute_piece_defense_score( const chess_game & game, const bool white ) const;
        float compute_bishop_pair_score( const chess_game & game, const bool white ) const;
        float compute_connected_rooks_score( const chess_game & game, const bool white ) const;
        float compute_king_centralization_score( const chess_game & game, const bool white ) const;
        float compute_knight_outpost_score( const chess_game & game, const bool white ) const;
        float compute_blocked_piece_score( const chess_game & game, const bool white ) const;
        float compute_space_control_score( const chess_game & game, const bool white ) const;
        float compute_king_shield_score( const chess_game & game, const bool white ) const;

        float  move_score( const chess_game & game, const move_t move ) const;
        float  order_moves( const chess_game & game ) const;
        float  minimax( const chess_game & game, const int depth, float alpha, float beta,
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