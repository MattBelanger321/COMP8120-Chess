#include "piece.hpp"
#include <cctype>
#include <ci_controller.hpp>
#include <iostream>
#include <string>

namespace chess::controller {

    void ci_controller::render()
    {
        while ( !should_close ) {
            std::cout << game.to_string() << "\n";

            if ( std::cin.eof() ) {
                return;
            }

            char file = '\0';
            do {

                std::cout << "\nSelect a File (a-h): ";
                std::cin >> file;

                if ( should_close ) {
                    return;
                }

                if ( std::cin.fail() ) {
                    std::cin.clear();                                                        // Clear the error flag
                    std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );  // Discard invalid input
                    std::cout << "\nInvalid input.\n";
                }
            } while ( ( file < 'a' || file > 'h' ) && ( file < 'A' || file > 'H' ) );

            file = std::toupper( file );

            int rank = -1;
            do {

                std::cout << "\nSelect a Rank (1-8): ";
                std::cin >> rank;

                if ( should_close ) {
                    return;
                }

                if ( std::cin.eof() ) {
                    return;
                }

                if ( std::cin.fail() ) {

                    std::cin.clear();                                                        // Clear the error flag
                    std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );  // Discard invalid input
                    std::cout << "\nInvalid input.\n";
                }
            } while ( rank < 1 || rank > 8 );

            pieces::position_t sel_pos = { static_cast< pieces::rank_t >( rank ),
                                           static_cast< pieces::file_t >( file - ( 'A' - 1 ) ) };

            select_space( sel_pos );

            std::cout << "\nYou Selected Space: " << to_string( selected_space.value().position() ) << "\n";

            if ( !selected_space->piece ) {
                std::cout << "\nThere is No Piece on Space " << to_string( selected_space.value().position() ) << "\n";
                continue;
            }

            std::vector< game::space > possible_moves;
            auto                       status = game.possible_moves( selected_space.value(), possible_moves );

            if ( status != pieces::move_status::valid ) {
                std::cout << "\nThere Are No Possible Moves From This Space: " << pieces::to_string( status )
                          << " Please Select Another Space\n";
                continue;
            }
            else if ( possible_moves.empty() ) {
                std::cout << "\nThere Are No Possible Moves From This Space, Please Select Another Space\n";
                continue;
            }

            std::cout << "\nYour Possible Moves Are: ";
            for ( auto const & move : possible_moves ) {
                std::cout << to_string( move.position() ) << ", ";
            }
            std::cout << "\nSelect Your Destination\n";

            file = '\0';
            do {

                std::cout << "\nSelect a File To Move To(a-h): ";
                std::cin >> file;

                if ( should_close ) {
                    return;
                }

                if ( std::cin.fail() ) {
                    std::cin.clear();                                                        // Clear the error flag
                    std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );  // Discard invalid input
                    std::cout << "\nInvalid input.\n";
                }
            } while ( ( file < 'a' || file > 'h' ) && ( file < 'A' || file > 'H' ) );

            file = std::toupper( file );

            rank = -1;
            do {
                std::cout << "\nSelect a Rank To Move To(1-8): ";
                std::cin >> rank;

                if ( should_close ) {
                    return;
                }

                if ( std::cin.fail() ) {
                    std::cin.clear();                                                        // Clear the error flag
                    std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );  // Discard invalid input
                    std::cout << "\nInvalid input.\n";
                }
            } while ( rank < 1 || rank > 8 );

            pieces::position_t mov_pos = { static_cast< pieces::rank_t >( rank ),
                                           static_cast< pieces::file_t >( file - ( 'A' - 1 ) ) };

            status = move( game.get( mov_pos ) );
            if ( status == pieces::move_status::valid ) {
                std::cout << "\nYou Moved From Space: " << to_string( sel_pos ) << " To Space: " << to_string( mov_pos )
                          << "\n";
            }
            else {
                std::cout << "\nSpace: " << to_string( sel_pos ) << " To Space: " << to_string( mov_pos )
                          << " FAILED: " << to_string( status ) << ", Try Again\n";
            }
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

    ci_controller::~ci_controller()
    {
        should_close = true;
        if ( runner.joinable() ) {
            runner.join();
        }
    }

}  // namespace chess::controller
