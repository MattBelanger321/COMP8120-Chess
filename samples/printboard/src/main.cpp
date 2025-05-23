#include "piece.hpp"
#include <board.hpp>

#include <iostream>

int main()
{
    chess::game::board game_board;

    std::cout << game_board.to_string() << "\n";

    auto const & space = game_board.get( { chess::pieces::rank_t::one, chess::pieces::file_t::g } );

    auto possible_spaces = game_board.possible_moves( space );

    for ( auto const & space : possible_spaces ) {
        std::cout << to_string( space.position() ) << "\n";
    }
}