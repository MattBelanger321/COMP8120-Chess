#include <display.hpp>
#include <iostream>
#include <piece.hpp>
#include <space.hpp>
#include <stdexcept>

#include <algorithm>

namespace chess::controller {

    display::display( unsigned int width, unsigned int height ) :
        width( width ),
        height( height ),
        board(
            height * 0.8f, height * 0.8f,                      //
            [this]( int i, int j ) -> space_context_t const {  //
                return this->get( i, j );
            },
            [this]( game::space const & sp ) {  //
                on_select( sp );
            } )
    {
    }

    space_context_t const display::get( int const i, int const j )
    {
        auto pos = pieces::piece::itopos( i, j );
        if ( !pos )
            throw std::runtime_error( "Invalid Coordinates Given to itopos" );

        auto & sp = game.get( *pos );

        bool possible = false;
        if ( std::find( possible_moves.begin(), possible_moves.end(), sp ) != possible_moves.end() ) {
            possible = true;
        }

        return { .sp = sp, .possible = possible };
    }

    void display::on_select( game::space const & sp )
    {
        std::cout << "Selected a Space: " << to_string( sp.position() ) << "\n";

        if ( !possible_moves.empty() ) {
            if ( selected_space &&
                 std::find( possible_moves.begin(), possible_moves.end(), sp ) != possible_moves.end() ) {
                auto status = move( sp );

                if ( status != pieces::move_status::valid ) {
                    std::cout << "\nMove To Space: " << to_string( sp.position() ) << " FAILED: " << to_string( status )
                              << ", Try Again\n";
                }
            }
            possible_moves = {};
            return;
        }

        // game.possible_moves populates vector
        if ( pieces::move_status::valid != game.possible_moves( sp, possible_moves ) ) {
            possible_moves = {};
        }

        if ( possible_moves.empty() ) {
            return;
        }

        select_space( sp.position() );

        std::cout << "Possible Moves: ";
        for ( auto & sp : possible_moves ) {
            std::cout << to_string( sp.position() ) << ", ";
        }
        std::cout << "\n";
    }

    void display::render()
    {
        // Set the position and size for the next window (top-left corner and full size)
        ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
        ImGui::SetNextWindowSize( ImVec2( static_cast< float >( height ), static_cast< float >( height ) ),
                                  ImGuiCond_Always );

        board.render();
    }

}  // namespace chess::controller