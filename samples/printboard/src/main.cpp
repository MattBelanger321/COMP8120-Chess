#include <board.hpp>
#include <piece.hpp>
#include <space.hpp>

#include <iostream>

int main()
{
    chess::game::board game_board;

    std::cout << game_board.to_string() << "\n";

    game_board.move( { chess::pieces::rank_t::one, chess::pieces::file_t::e },
                     { chess::pieces::rank_t::eight, chess::pieces::file_t::e } );

    std::cout << game_board.to_string() << "\n";
}