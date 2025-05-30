#include <display.hpp>

namespace chess::controller {

    display::display( unsigned int width, unsigned int height ) :
        width( width ), height( height ), board( width, height * 0.8 )
    {
    }

    void display::render()
    {
        // Set the position and size for the next window (top-left corner and full size)
        ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
        ImGui::SetNextWindowSize( ImVec2( static_cast< float >( width ), static_cast< float >( height ) ),
                                  ImGuiCond_Always );

        // Begin the window with no move, no resize, no collapse, and no title bar
        ImGui::Begin( "Chess Game", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );
        {
            board.render();
            board.render();
            board.render();
            board.render();
            board.render();
            board.render();
        }
        ImGui::End();
    }

}  // namespace chess::controller