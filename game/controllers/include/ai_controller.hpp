#ifndef __CHESS__CONTROLLER__AI_CONTROLLER__
#define __CHESS__CONTROLLER__AI_CONTROLLER__

#include "board.hpp"
#include "game.hpp"
#include "knight.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <array>
#include <controller.hpp>

#include <thread>

namespace chess::controller {
    struct chromosome_t {

        float pawn_val;
        float knight_val;
        float bishop_val;
        float rook_val;
        float queen_val;

        float material_score_val;
        float piece_mobility_val;
        float center_control_val;
        float king_safety_val;

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

            pawn_val           = chromosome[0];
            knight_val         = chromosome[1];
            bishop_val         = chromosome[2];
            rook_val           = chromosome[3];
            queen_val          = chromosome[4];
            material_score_val = chromosome[5];
            piece_mobility_val = chromosome[6];
            center_control_val = chromosome[7];
            king_safety_val    = chromosome[8];

            size_t offset = 9;
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

        float  minimax( const chess_game & game, const int depth, float alpha, float beta,
                        const bool maximizing_player ) const;
        move_t select_best_move( const int depth ) const;
        float  evaluate_position() const;
        float  evaluate_position( const chess_game & board ) const;
        // returns a value that should be added to the score of the entire position
        float king_safety( const chess_game & board ) const;
        float piece_score( const chess_game & game ) const;
        float position_score( const chess_game & game ) const;

    public:
        ai_controller( chromosome_t chromie );

        ~ai_controller();

        void activate();
    };
}  // namespace chess::controller

#endif