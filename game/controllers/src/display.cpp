#include <display.hpp>
#include <piece.hpp>
#include <stdexcept>

namespace chess::controller {

    display::display( unsigned int width, unsigned int height ) :
        width( width ),
        height( height ),
        board( height * 0.8f, height * 0.8f,
               board_callback( [this]( int i, int j ) -> game::space const & { return this->get( i, j ); } ) )
    {
    }

    game::space const & display::get( int const i, int const j )
    {
        auto pos = pieces::piece::itopos( i, j );

        if ( pos ) {
            return game.get( *pos );
        }

        throw std::runtime_error( "Invalid Coordinates Given to itopos" );
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