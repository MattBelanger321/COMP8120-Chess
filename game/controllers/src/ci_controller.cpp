#include "piece.hpp"
#include <cctype>
#include <ci_controller.hpp>
#include <iostream>

namespace chess::controller {

    void ci_controller::render()
    {
        while ( !should_close ) {
            std::cout << game.to_string() << "\n";

            int rank = -1;
            do {
                std::cout << "Select a Rank (1-8): ";
                std::cin >> rank;
            } while ( rank < 1 || rank > 8 );

            char file = '\0';
            do {
                std::cout << "Select a File (a-h): ";
                std::cin >> file;
            } while ( ( file < 'a' || file > 'h' ) && ( file < 'A' || file > 'H' ) );

            file = std::toupper( file );

            pieces::position_t sel_pos = { static_cast< pieces::rank_t >( rank ),
                                           static_cast< pieces::file_t >( file - ( 'A' - 1 ) ) };

            select_space( sel_pos );

            std::cout << "You Selected Space: " << to_string( selected_space.value().position() ) << "\n";

            auto possible_moves = game.possible_moves( selected_space.value() );

            if ( possible_moves.empty() ) {
                std::cout << "There Are No Possible Moves From This Space, Please Select Another Space\n";
                continue;
            }

            std::cout << "Your Possible Moves Are: ";
            for ( auto const & move : possible_moves ) {
                std::cout << to_string( move.position() ) << ", ";
            }
            std::cout << "\nSelect Your Destination\n";

            rank = -1;
            do {
                std::cout << "Select a Rank To Move To(1-8): ";
                std::cin >> rank;
            } while ( rank < 1 || rank > 8 );

            file = '\0';
            do {
                std::cout << "Select a File To Move To(a-h): ";
                std::cin >> file;
            } while ( ( file < 'a' || file > 'h' ) && ( file < 'A' || file > 'H' ) );

            file = std::toupper( file );

            pieces::position_t mov_pos = { static_cast< pieces::rank_t >( rank ),
                                           static_cast< pieces::file_t >( file - ( 'A' - 1 ) ) };

            move( game.get( mov_pos ) );

            std::cout << "You Moved From Space: " << to_string( sel_pos ) << " To Space: " << to_string( mov_pos )
                      << "\n";
        }
    }

    void ci_controller::run()
    {
        should_close = false;
        runner       = std::thread( [this]() { render(); } );
    }

    void ci_controller::close()
    {
        should_close = true;
        if ( runner.joinable() )
            runner.join();
    }

}  // namespace chess::controller
