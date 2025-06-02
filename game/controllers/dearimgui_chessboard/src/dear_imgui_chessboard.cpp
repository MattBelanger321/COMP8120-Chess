#include <dear_imgui_chessboard.hpp>
#include <filesystem>
#include <iostream>
#include <space.hpp>
#include <stdexcept>
#include <texture_loader.hpp>

#include <imgui.h>

namespace chess::controller {

    imgui_chessboard::imgui_chessboard( unsigned int const width, unsigned int const height,
                                        board_callback const game_board, selection_callback const select ) :
        width( width ), height( height ), game_board( game_board ), select( select )
    {
    }

    void imgui_chessboard::init()
    {
        const std::filesystem::path dir_path = "/home/matthew/GradSchool/COMP8120-Chess/assets/img/pieces";

        if ( !std::filesystem::exists( dir_path ) || !std::filesystem::is_directory( dir_path ) ) {
            throw std::runtime_error( "ICONS DIRECTORY NOT FOUND" );
        }

        for ( const auto & entry : std::filesystem::directory_iterator( dir_path ) ) {
            if ( entry.is_regular_file() ) {
                const auto & path = entry.path();

                gl_img_t img;
                auto     icon = detect_icon( path );
                if ( !texture_loader::load_from_file( path.string(), img.img, img.width, img.height ) ) {
                    throw std::runtime_error( std::string( "Failed to process image: " ) + path.string() + "\n" );
                }

                icons[icon] = std::move( img );
            }
        }
    }

    icon_t imgui_chessboard::detect_icon( const std::filesystem::path & path ) const
    {
        const std::string filename = path.filename().string();

        if ( filename.empty() )
            throw std::runtime_error( "Invalid Filename Given" );

        bool is_white = filename[0] == 'w';
        bool is_black = filename[0] == 'b';

        if ( !is_white && !is_black )
            throw std::runtime_error( "Invalid Filename Given" );

        if ( filename.find( "pawn" ) != std::string::npos ) {
            return is_white ? icon_t::white_pawn : icon_t::black_pawn;
        }
        else if ( filename.find( "knight" ) != std::string::npos ) {
            return is_white ? icon_t::white_knight : icon_t::black_knight;
        }
        else if ( filename.find( "bishop" ) != std::string::npos ) {
            return is_white ? icon_t::white_bishop : icon_t::black_bishop;
        }
        else if ( filename.find( "rook" ) != std::string::npos ) {
            return is_white ? icon_t::white_rook : icon_t::black_rook;
        }
        else if ( filename.find( "queen" ) != std::string::npos ) {
            return is_white ? icon_t::white_queen : icon_t::black_queen;
        }
        else if ( filename.find( "king" ) != std::string::npos ) {
            return is_white ? icon_t::white_king : icon_t::black_king;
        }

        throw std::runtime_error( "Invalid Filename Given" );
    }

    void imgui_chessboard::render()
    {
        ImGui::Begin( "Chess Game", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );
        draw_board();
        ImGui::End();
    }

    void imgui_chessboard::draw_board()
    {
        if ( icons.empty() ) {
            init();
        }

        // Use a unique ID for the child panel
        ImGui::BeginChild( "ChessboardPanel", ImVec2( static_cast< float >( width ), static_cast< float >( width ) ),
                           false,  // border
                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                               ImGuiWindowFlags_NoScrollWithMouse );
        for ( int r = 1; r <= 8; r++ ) {
            for ( int f = 1; f <= 8; f++ ) {
                draw_square( game_board( r, f ) );
            }
        }

        ImGui::EndChild();
    }

    void imgui_chessboard::draw_square( space_context_t const & sp )
    {
        int i = static_cast< int >( sp.sp.position().second ) - 1;
        int j = 8 - static_cast< int >( sp.sp.position().first );

        // Get the draw list for the current window
        ImDrawList * draw_list = ImGui::GetWindowDrawList();

        // Choose a starting point (top-left corner of the square)
        ImVec2 p           = ImGui::GetCursorScreenPos();
        float  square_size = width / 8.f;

        ImU32 colour;

        if ( sp.sp.piece && sp.possible ) {
            colour = IM_COL32( 255, 0, 0, 255 );
        }
        else {
            // select the colour of the square
            bool is_light_square = sp.sp.colour();
            colour               = is_light_square ? IM_COL32( 240, 217, 181, 255 ) :  // Light squares (cream/beige)
                         IM_COL32( 181, 136, 99, 255 );                  // Dark squares (brown)
        }

        // Define the square corners
        ImVec2 top_left     = ImVec2( p.x + ( square_size * i ), p.y + ( square_size * j ) );
        ImVec2 bottom_right = ImVec2( top_left.x + square_size, top_left.y + square_size );

        // Draw the square
        draw_list->AddRectFilled( top_left, bottom_right, colour, 0.0f, ImDrawFlags_Closed );

        if ( !sp.sp.piece && sp.possible ) {
            // Compute square center
            ImVec2 center = ImVec2( ( top_left.x + bottom_right.x ) * 0.5f, ( top_left.y + bottom_right.y ) * 0.5f );

            // Radius as a fraction of square size (adjust as needed)
            float radius = square_size * 0.1f;  // 10% of square size

            // Draw the filled circle (e.g., blue)
            draw_list->AddCircleFilled( center, radius, IM_COL32( 0, 0, 255, 180 ) );
        }

        // Add clickable area
        ImGui::PushID( i * 8 + j );  // Unique ID per square
        ImVec2 cursor_backup = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos( top_left );
        if ( ImGui::InvisibleButton( "square", ImVec2( square_size, square_size ) ) ) {
            select( sp.sp );
        }
        ImGui::SetCursorScreenPos( cursor_backup );
        ImGui::PopID();

        if ( !sp.sp.piece ) {
            return;
        }
        icon_t icon;

        if ( sp.sp.piece->colour() ) {  // Assuming true = white
            switch ( sp.sp.piece->type() ) {
            case pieces::name_t::rook:
                icon = icon_t::white_rook;
                break;
            case pieces::name_t::knight:
                icon = icon_t::white_knight;
                break;
            case pieces::name_t::bishop:
                icon = icon_t::white_bishop;
                break;
            case pieces::name_t::king:
                icon = icon_t::white_king;
                break;
            case pieces::name_t::queen:
                icon = icon_t::white_queen;
                break;
            case pieces::name_t::pawn:
                icon = icon_t::white_pawn;
                break;
            }
        }
        else {  // Black
            switch ( sp.sp.piece->type() ) {
            case pieces::name_t::rook:
                icon = icon_t::black_rook;
                break;
            case pieces::name_t::knight:
                icon = icon_t::black_knight;
                break;
            case pieces::name_t::bishop:
                icon = icon_t::black_bishop;
                break;
            case pieces::name_t::king:
                icon = icon_t::black_king;
                break;
            case pieces::name_t::queen:
                icon = icon_t::black_queen;
                break;
            case pieces::name_t::pawn:
                icon = icon_t::black_pawn;
                break;
            }
        }

        float  padding            = square_size * 0.1f;
        ImVec2 image_top_left     = ImVec2( top_left.x + padding, top_left.y + padding );
        ImVec2 image_bottom_right = ImVec2( bottom_right.x - padding, bottom_right.y - padding );

        ImTextureID img = (ImTextureID)(uintptr_t)icons.at( icon ).img;
        draw_list->AddImage( img, image_top_left, image_bottom_right );
    }

}  // namespace chess::controller