#ifndef __CHESS__GAME__
#define __CHESS__GAME__

#include "king.hpp"
#include <board.hpp>
#include <memory>
#include <piece.hpp>
#include <space.hpp>

namespace chess {
    enum class game_state {
        white_move,
        black_move,
        white_check,
        black_check,
        white_wins,
        black_wins,
        white_offers_draw,
        black_offers_draw,
        white_resigns,
        black_resigns,
        draw,
        invalid_game_state,
    };

    inline std::string to_string( game_state state )
    {
        switch ( state ) {
        case game_state::white_move:
            return "White to move";
        case game_state::black_move:
            return "Black to move";
        case game_state::white_check:
            return "White is in check";
        case game_state::black_check:
            return "Black is in check";
        case game_state::white_wins:
            return "White wins";
        case game_state::black_wins:
            return "Black wins";
        case game_state::white_offers_draw:
            return "White offers a draw";
        case game_state::black_offers_draw:
            return "Black offers a draw";
        case game_state::white_resigns:
            return "White resigns";
        case game_state::black_resigns:
            return "Black resigns";
        case game_state::draw:
            return "Game is a draw";
        case game_state::invalid_game_state:
            return "Invalid game state";
        default:
            return "Unknown game state";
        }
    }

    using move_t = std::pair< game::space, game::space >;

    class chess_game {
    private:
        game_state  state;
        game::board game_board;

        // based on the moved piece thene functions update the castling status flags
        void white_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                    pieces::position_t const &               src_pos );
        void black_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                    pieces::position_t const &               src_pos );

        // set if the king or rook has not moved yet
        bool king_side_castle_white;
        bool king_side_castle_black;
        bool queen_side_castle_white;
        bool queen_side_castle_black;

    public:
        struct color_attack_map {
            int                               num_attackers;
            std::vector< pieces::position_t > attacker_spaces;

            void clear()
            {
                num_attackers = 0;
                attacker_spaces.clear();
            }

            void add_attacker( pieces::position_t attacker )
            {
                num_attackers++;
                attacker_spaces.push_back( attacker );
            }

            void remove_attacker( pieces::position_t attacker )
            {
                num_attackers--;
                std::erase( attacker_spaces, attacker );
            }

            std::string to_string() const { return std::to_string( num_attackers ); }
        };

        struct attack_map {
            color_attack_map white_attack_map[8][8];
            color_attack_map black_attack_map[8][8];

            void clear()
            {
                for ( int i = 0; i < 8; i++ ) {
                    for ( int j = 0; j < 8; j++ ) {
                        white_attack_map[i][j].clear();
                        black_attack_map[i][j].clear();
                    }
                }
            }

            void add_attacker( game::space attacker, pieces::position_t position_attacked )
            {
                if ( !attacker.piece ) {
                    return;
                }

                int rank = static_cast< int >( position_attacked.first );
                int file = static_cast< int >( position_attacked.second );

                if ( attacker.piece->colour() ) {
                    white_attack_map[rank - 1][file - 1].add_attacker( position_attacked );
                }
                else {
                    black_attack_map[rank - 1][file - 1].add_attacker( position_attacked );
                }
            }

            void remove_attacker( game::space attacker, pieces::position_t position_attacked )
            {
                if ( !attacker.piece ) {
                    return;
                }

                int rank = static_cast< int >( position_attacked.first );
                int file = static_cast< int >( position_attacked.second );

                if ( attacker.piece->colour() ) {
                    white_attack_map[rank - 1][file - 1].remove_attacker( position_attacked );
                }
                else {
                    black_attack_map[rank - 1][file - 1].remove_attacker( position_attacked );
                }
            }

            int has_attackers( game::space s, bool color ) const
            {
                // return num_attackers(s, color) > 0;
                int rank = static_cast< int >( s.position().first );
                int file = static_cast< int >( s.position().second );

                if ( color ) {
                    return white_attack_map[rank - 1][file - 1].num_attackers > 0;
                }
                else {
                    return black_attack_map[rank - 1][file - 1].num_attackers > 0;
                }
            }

            int num_attackers( game::space s, bool color ) const
            {
                int rank = static_cast< int >( s.position().first );
                int file = static_cast< int >( s.position().second );

                if ( color ) {
                    return white_attack_map[rank - 1][file - 1].num_attackers;
                }
                else {
                    return black_attack_map[rank - 1][file - 1].num_attackers;
                }
            }

            std::string to_string() const
            {
                std::string output = "White Attack Map:\n";
                for ( int i = 0; i < 8; i++ ) {
                    for ( int j = 0; j < 8; j++ ) {
                        output += white_attack_map[7 - i][j].to_string() + " ";
                    }
                    output += "\n";
                }
                output += "\nBlack Attack Map\n";
                for ( int i = 0; i < 8; i++ ) {
                    for ( int j = 0; j < 8; j++ ) {
                        output += black_attack_map[7 - i][j].to_string() + " ";
                    }
                    output += "\n";
                }
                output += "\n";

                return output;
            }
        };

        attack_map game_attack_map;

        chess_game();

        pieces::move_status move( game::space const & src, game::space const & dst );

        game::space const & get( pieces::position_t const & pos ) const;

        std::string to_string() const;

        std::vector< game::space > psuedo_possible_moves( game::board game_board, game::space const & src ) const;
        pieces::move_status        possible_moves( game::space const &          src,
                                                   std::vector< game::space > & possible_moves ) const;
        pieces::move_status        possible_moves( game::board game_board, game::space const & src,
                                                   std::vector< game::space > & possible_moves ) const;
        std::vector< move_t >      legal_moves() const;

        std::vector< game::space > find_attackers( game::space const & src, bool const victim_color ) const;

        bool                              can_castle( bool const color ) const;
        void                              remove_piece_at( pieces::position_t position );
        std::vector< std::string >        get_move_history() const;
        attack_map const                  generate_attack_map( game::board ) const;
        void                              update_attack_map();
        std::vector< pieces::position_t > possible_attacks( game::space const & src ) const;
        std::vector< pieces::position_t > possible_attacks( game::board const & b, game::space const & src ) const;

        bool white_move() const;
        bool black_move() const;

        void start();
        void stop();

        inline game_state const  get_state() const { return state; }
        inline game::board const get_board() const { return game_board; }

        inline void const set_turn( bool const colour )
        {
            state = colour ? game_state::white_move : game_state::black_move;
        }

        bool checkmate( bool const colour ) const;

        // read only references to the king so we can check for checks
        std::reference_wrapper< pieces::king const > white_king;
        std::reference_wrapper< pieces::king const > black_king;
    };
}  // namespace chess

#endif