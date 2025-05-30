#include <dear_imgui_chessboard.hpp>

#include <iostream>

namespace chess::controller {

    imgui_chessboard::imgui_chessboard( unsigned int width, unsigned int height ) : width( width ), height( height ) {}

    void imgui_chessboard::render()
    {
        // Use a unique ID for the child panel
        ImGui::BeginChild( "ChessboardPanel", ImVec2( static_cast< float >( width ), static_cast< float >( height ) ),
                           true,  // border
                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                               ImGuiWindowFlags_NoScrollWithMouse );

        // Center the button in the child panel
        float panel_center_x =
            ImGui::GetCursorPosX() +
            ( width - ImGui::CalcTextSize( "That was easy" ).x - ImGui::GetStyle().FramePadding.x * 2 ) / 2;
        ImGui::SetCursorPosX( panel_center_x );

        // Render the button
        if ( ImGui::Button( "That was easy" ) ) {
            std::cout << "THAT WAS EASY\n";
        }

        ImGui::EndChild();
    }

}  // namespace chess::controller