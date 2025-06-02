#include "space.hpp"
#include <display.hpp>
#include <iostream>
#include <piece.hpp>
#include <stdexcept>

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
            possible_moves = {};
        }

        if ( pieces::move_status::valid != game.possible_moves( sp, possible_moves ) ) {
            possible_moves = {};
        }

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