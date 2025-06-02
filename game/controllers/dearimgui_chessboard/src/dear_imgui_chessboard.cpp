#include "space.hpp"
#include <dear_imgui_chessboard.hpp>

#include <imgui.h>
#include <iostream>

namespace chess::controller {

    imgui_chessboard::imgui_chessboard( unsigned int const width, unsigned int const height,
                                        board_callback const game_board ) :
        width( width ), height( height ), game_board( game_board )
    {
    }

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
        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                draw_square( game_board( i, j ) );
            }
        }

        ImGui::EndChild();
    }

    void imgui_chessboard::draw_square( game::space const & sp )
    {

        // Get the draw list for the current window
        ImDrawList * draw_list = ImGui::GetWindowDrawList();

        // Choose a starting point (top-left corner of the square)
        ImVec2 p           = ImGui::GetCursorScreenPos();
        float  square_size = width / 8.f;

        // select the colour of the square
        ImU32 colour;
        if ( sp.colour() ) {
            colour = IM_COL32( 255, 255, 255, 255 );
        }
        else {
            colour = IM_COL32( 0, 0, 0, 255 );
        }

        // Define the square corners
        ImVec2 top_left     = ImVec2( p.x + ( square_size * ( static_cast< int >( sp.position().first ) - 1 ) ),
                                      p.y + ( square_size * ( static_cast< int >( sp.position().second ) - 1 ) ) );
        ImVec2 bottom_right = ImVec2( top_left.x + square_size, top_left.y + square_size );

        // Draw the square
        draw_list->AddRectFilled( top_left, bottom_right, colour, 0.0f, ImDrawFlags_Closed );

        // if ( sp.piece ) {
        //     // Optional padding inside the square
        //     float  padding            = square_size * 0.1f;
        //     ImVec2 image_top_left     = ImVec2( top_left.x + padding, top_left.y + padding );
        //     ImVec2 image_bottom_right = ImVec2( bottom_right.x - padding, bottom_right.y - padding );

        //     draw_list->AddImage( nullptr, image_top_left, image_bottom_right );
        // }
    }

}  // namespace chess::controller