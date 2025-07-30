#include <client_display.hpp>
#include <imgui.h>
#include <iostream>
#include <piece.hpp>
#include <space.hpp>
#include <stdexcept>

#include <algorithm>

#include <game.hpp>

namespace chess::networking {

    client_display::client_display( component_data const board_dims, component_data const status_dims,
                                    component_data const control_dims ) :
        game( "ws://localhost:8080" ),
        board_dims( board_dims ),
        status_dims( status_dims ),
        control_dims( control_dims ),
        board(
            board_dims.size.width * .975f, board_dims.size.height * .975f,  //
            [this]( int i, int j ) -> controller::space_context_t const {   //
                return this->get( i, j );
            },
            [this]( game::space const & sp ) {  //
                on_select( sp );
            } )
    {
    }

    controller::space_context_t const client_display::get( int const i, int const j )
    {

        auto pos = pieces::piece::itopos( i, j );
        if ( !pos )
            throw std::runtime_error( "Invalid Coordinates Given to itopos" );

        auto sp       = game.get( *pos );
        bool possible = false;

        if ( game.selected_space->piece ) {
            if ( std::find( possible_moves.begin(), possible_moves.end(), sp ) != possible_moves.end() ) {
                auto & selected_space_current = game.game_board.get( game.selected_space->position() );
                if ( !selected_space_current.piece ) {
                    game.selected_space.reset();
                }
                else if ( sp.piece && sp.piece->colour() == game.selected_space->piece->colour() ) {
                    game.selected_space.reset();
                }
                else {
                    possible = true;
                }
            }
        }

        return { .sp = sp, .possible = possible };
    }

    void client_display::on_select( game::space const & sp )
    {
        std::cout << "Selected a Space: " << to_string( sp.position() ) << "\n";
        // std::cout << "\tpossible moves1: " << possible_moves.size() << std::endl;

        if ( !possible_moves.empty() ) {
            if ( game.selected_space &&
                 std::find( possible_moves.begin(), possible_moves.end(), sp ) != possible_moves.end() ) {
                auto status = game.move( sp );

                if ( status ) {
                    std::cout << "\nMove To Space: " << to_string( sp.position() ) << " FAILED\n";
                }
            }
            possible_moves = {};
            return;
        }

        // game.possible_moves populates vector
        possible_moves = game.possible_moves( sp );

        if ( possible_moves.empty() ) {
            return;
        }
        game.select_space( sp );

        std::cout << "Possible Moves: ";
        for ( auto & sp : possible_moves ) {
            std::cout << to_string( sp.position() ) << ", ";
        }
        std::cout << "\n";
    }

    void client_display::render()
    {
        game.update_board();

        auto new_state = game.get_state();
        if ( game.game_state != new_state ) {
            possible_moves = {};
            game.selected_space.reset();
        }
        game.game_state = new_state;
        chess_board();
        status_dialog();
        control_panel();
    }

    void client_display::chess_board()
    {

        // Position this dialog to the right of the main panel
        ImVec2 chess_board_pos = ImVec2( board_dims.pos.x, board_dims.pos.y );
        ImGui::SetNextWindowPos( chess_board_pos );
        ImGui::SetNextWindowSize( ImVec2( board_dims.size.width, board_dims.size.height ) );  // Optional: match height
        ImGui::Begin( "Chess Game", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );

        board.render();

        ImGui::End();
    }

    void client_display::status_dialog()
    {

        // Position this dialog to the right of the main panel
        ImVec2 status_dialog_pos = ImVec2( status_dims.pos.x, status_dims.pos.y );
        ImGui::SetNextWindowPos( status_dialog_pos );
        ImGui::SetNextWindowSize(
            ImVec2( status_dims.size.width, status_dims.size.height ) );  // Optional: match height

        ImGui::Begin( "Status Dialog", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        ImGui::SetWindowFontScale( 2.f );  // Scale factor (1.0 = normal size)
        ImGui::Text( "%s", game.game_state.c_str() );
        ImGui::SetWindowFontScale( 1.f );  // Scale factor (1.0 = normal size)
        ImGui::End();
    }

    void client_display::control_panel()
    {

        // Position this dialog to the right of the main panel
        ImVec2 control_panel_pos = ImVec2( control_dims.pos.x, control_dims.pos.y );
        ImGui::SetNextWindowPos( control_panel_pos );
        ImGui::SetNextWindowSize(
            ImVec2( control_dims.size.width, control_dims.size.height ) );  // Optional: match height

        ImGui::Begin( "Control Panel", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        ImGui::Button( "Dump Board State to Terminal" );
        ImGui::End();
    }

}  // namespace chess::networking