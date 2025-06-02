#include <dear_imgui_chessboard.hpp>

#include <imgui.h>
#include <iostream>

namespace chess::controller {

    imgui_chessboard::imgui_chessboard( unsigned int width, unsigned int height ) : width( width ), height( height ) {}

    void imgui_chessboard::render()
    {
        ImGui::Begin( "Chess Game", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );
        draw_board();
        ImGui::End();
    }

    void imgui_chessboard::draw_board()
    {  // Use a unique ID for the child panel
        ImGui::BeginChild( "ChessboardPanel", ImVec2( static_cast< float >( width ), static_cast< float >( width ) ),
                           false,  // border
                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                               ImGuiWindowFlags_NoScrollWithMouse );
        for ( int i = 0; i < 8; i++ ) {
            for ( int j = 0; j < 8; j++ ) {
                draw_square( i, j );
            }
        }

        ImGui::EndChild();
    }

    void imgui_chessboard::draw_square( int const i, int const j )
    {

        // Get the draw list for the current window
        ImDrawList * draw_list = ImGui::GetWindowDrawList();

        // Choose a starting point (top-left corner of the square)
        ImVec2 p           = ImGui::GetCursorScreenPos();
        float  square_size = width / 8.f;

        // select the colour of the square
        ImU32 colour;
        if ( ( i + j ) % 2 == 1 ) {
            colour = IM_COL32( 255, 255, 255, 255 );
        }
        else {
            colour = IM_COL32( 0, 0, 0, 255 );
        }

        // Define the square corners
        ImVec2 top_left     = ImVec2( p.x + ( square_size * i ), p.y + ( square_size * j ) );
        ImVec2 bottom_right = ImVec2( top_left.x + square_size, top_left.y + square_size );

        // Draw the square
        draw_list->AddRectFilled( top_left, bottom_right, colour, 0.0f, ImDrawFlags_Closed );
    }

}  // namespace chess::controller