#include <board.hpp>

#include <iostream>

int main()
{
    chess::game::board game_board;

    std::cout << game_board.to_string() << "\n";
}