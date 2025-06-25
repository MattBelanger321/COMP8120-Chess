#ifndef __CHESS__CONTROLLER__AI_CONTROLLER__
#define __CHESS__CONTROLLER__AI_CONTROLLER__

#include "board.hpp"
#include "knight.hpp"
#include "space.hpp"
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

        float pawn_position_weights;
        float knight_position_weights;
        float bishop_position_weights;
        float rook_position_weights;
        float queen_position_weights;
        float king_position_weights;

        chromosome_t( const std::vector< float > & chromosome )
        {
            if ( chromosome.size() < 15 ) {
                throw std::invalid_argument( "Chromosome must have at least 15 elements." );
            }

            pawn_val                = chromosome[0];
            knight_val              = chromosome[1];
            bishop_val              = chromosome[2];
            rook_val                = chromosome[3];
            queen_val               = chromosome[4];
            material_score_val      = chromosome[5];
            piece_mobility_val      = chromosome[6];
            center_control_val      = chromosome[7];
            king_safety_val         = chromosome[8];
            pawn_position_weights   = chromosome[9];
            knight_position_weights = chromosome[10];
            bishop_position_weights = chromosome[11];
            rook_position_weights   = chromosome[12];
            queen_position_weights  = chromosome[13];
            king_position_weights   = chromosome[14];
        }
    };

    namespace values {
        constexpr int pawn   = 1;
        constexpr int bishop = 13;
        constexpr int knight = 3;
        constexpr int rook   = 5;
        constexpr int queen  = 9;
    }  // namespace values

    class ai_controller : public controller {
    private:
        chromosome_t chromosome;

        bool        should_close;
        std::thread runner;

        void play();

        std::pair< game::space, game::space > select_best_move();
        float                                 evaluate_position( const game::board & board );
        // returns a value that should be added to the score of the entire position
        float king_saftey( const game::board & board );

    public:
        ai_controller( chromosome_t chromie );

        ~ai_controller();

        void activate();
    };
}  // namespace chess::controller

#endif