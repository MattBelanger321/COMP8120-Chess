#include "space.hpp"
#include <algorithm>
#include <board.hpp>
#include <cassert>
#include <chrono>
#include <game.hpp>
#include <iostream>
#include <ostream>
#include <piece.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace chess {

    void chess_game::color_attack_map::clear()
    {
        num_attackers = 0;
        attacker_spaces.clear();
    }

    void chess_game::color_attack_map::add_attacker( pieces::position_t attacker )
    {
        num_attackers++;
        attacker_spaces.push_back( attacker );
    }

    void chess_game::color_attack_map::remove_attacker( pieces::position_t attacker )
    {
        num_attackers--;
        std::erase( attacker_spaces, attacker );
    }

    std::string chess_game::color_attack_map::to_string() const { return std::to_string( num_attackers ); }

    void chess_game::attack_map::clear()
    {
        for ( int i = 0; i < 8; i++ ) {
            for ( int j = 0; j < 8; j++ ) {
                white_attack_map[i][j].clear();
                black_attack_map[i][j].clear();
            }
        }
    }

    void chess_game::attack_map::add_attacker( game::space const &        attacker,
                                               pieces::position_t const & position_attacked )
    {
        if ( !attacker.piece ) {
            return;
        }

        int rank = static_cast< int >( position_attacked.first );
        int file = static_cast< int >( position_attacked.second );

        if ( attacker.piece->colour() ) {
            white_attack_map[rank - 1][file - 1].add_attacker( position_attacked );
        }
        else {
            black_attack_map[rank - 1][file - 1].add_attacker( position_attacked );
        }
    }

    void chess_game::attack_map::remove_attacker( game::space const &        attacker,
                                                  pieces::position_t const & position_attacked )
    {
        if ( !attacker.piece ) {
            return;
        }

        int rank = static_cast< int >( position_attacked.first );
        int file = static_cast< int >( position_attacked.second );

        if ( attacker.piece->colour() ) {
            white_attack_map[rank - 1][file - 1].remove_attacker( position_attacked );
        }
        else {
            black_attack_map[rank - 1][file - 1].remove_attacker( position_attacked );
        }
    }

    // color is the team that is attacking
    bool chess_game::attack_map::has_attackers( game::space const & s, bool color ) const
    {
        return num_attackers( s, color ) > 0;
    }

    int chess_game::attack_map::num_attackers( game::space const & s, bool color ) const
    {
        int rank = static_cast< int >( s.position().first );
        int file = static_cast< int >( s.position().second );

        if ( color ) {
            return white_attack_map[rank - 1][file - 1].num_attackers;
        }
        else {
            return black_attack_map[rank - 1][file - 1].num_attackers;
        }
    }

    std::string chess_game::attack_map::to_string() const
    {
        std::string output = "White Attack Map:\n";
        for ( int i = 0; i < 8; i++ ) {
            for ( int j = 0; j < 8; j++ ) {
                output += white_attack_map[7 - i][j].to_string() + " ";
            }
            output += "\n";
        }
        output += "\nBlack Attack Map\n";
        for ( int i = 0; i < 8; i++ ) {
            for ( int j = 0; j < 8; j++ ) {
                output += black_attack_map[7 - i][j].to_string() + " ";
            }
            output += "\n";
        }
        output += ";\n";

        return output;
    }

    chess_game::chess_game() :
        game_board(),
        white_king( *reinterpret_cast< pieces::king * >(
            game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() ) ),
        black_king(
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() ) )
    {
        start();
    }

    chess_game::chess_game( std::string const & board_state ) :
        game_board(),
        white_king( *reinterpret_cast< pieces::king * >(
            game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() ) ),
        black_king(
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() ) )
    {
        load_from_string( board_state );
    }

    void chess_game::start()
    {
        state = game_state::white_move;
        game_board.reset();
        king_side_castle_white  = true;
        king_side_castle_black  = true;
        queen_side_castle_white = true;
        queen_side_castle_black = true;

        white_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() );
        black_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() );
        update_attack_map();
    }

    bool chess_game::checkmate( bool const colour ) const
    {
        std::vector< game::space > moves;
        moves.reserve( 64 );
        game::board board_copy = game_board;

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & src = board_copy.get( pieces::piece::itopos( i, j ).value() );
                if ( src.piece && src.piece->colour() == colour ) {
                    possible_moves( board_copy, src, moves );
                    if ( !moves.empty() ) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    pieces::move_status chess_game::move( game::space const & src, game::space const & dst )
    {
        if ( !src.piece ) {
            return pieces::move_status::no_piece_to_move;
        }

        // deep copy
        auto src_piece_cpy = src.piece->copy_piece();

        std::vector< game::space > pos;
        auto                       board_cpy = game_board;
        auto                       status    = possible_moves( board_cpy, src, pos );

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        if ( pos.empty() || std::find( pos.begin(), pos.end(), dst ) == pos.end() ) {
            return pieces::move_status::illegal_move;
        }

        auto copy_dst = dst.position();
        status        = game_board.move( src.position(), dst.position() );
        update_attack_map();

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        try {
            if ( white_move() ) {
                if ( game_attack_map.has_attackers( game_board.get( black_king.get().position() ), true ) ) {
                    state = game_state::black_check;
                    if ( checkmate( false ) ) {
                        std::cout << "White Wins\n";
                        state = game_state::white_wins;
                    }
                }
                else {
                    state = game_state::black_move;
                }

                white_castling_rights( src_piece_cpy, src.position() );

                if ( dst == pieces::piece::itopos( 8, 1 ) ) {
                    queen_side_castle_black = false;
                }
                else if ( dst == pieces::piece::itopos( 8, 8 ) ) {
                    king_side_castle_black = false;
                }
            }
            else if ( black_move() ) {
                if ( game_attack_map.has_attackers( game_board.get( white_king.get().position() ), false ) ) {
                    state = game_state::white_check;
                    if ( checkmate( true ) ) {
                        std::cout << "Black Wins\n";
                        state = game_state::black_wins;
                    }
                }
                else {
                    state = game_state::white_move;
                }
                black_castling_rights( src_piece_cpy, src.position() );
                if ( dst == pieces::piece::itopos( 1, 1 ) ) {
                    queen_side_castle_white = false;
                }
                else if ( dst == pieces::piece::itopos( 1, 8 ) ) {
                    king_side_castle_white = false;
                }
            }
            else {
                throw std::logic_error( "Impossible Game State" );
            }
        }
        catch ( const std::exception & e ) {
            std::cerr << "Error Checking Attack Map\n";
        }
        return pieces::move_status::valid;
    }

    void chess_game::white_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                            pieces::position_t const &               src_pos )
    {
        // white castling logic
        if ( ( king_side_castle_white || queen_side_castle_white ) && moved_piece->colour() ) {
            if ( moved_piece->type() == pieces::name_t::king ) {
                king_side_castle_white  = false;
                queen_side_castle_white = false;
            }
            else if ( moved_piece->type() == pieces::name_t::rook ) {
                if ( queen_side_castle_white &&
                     src_pos == pieces::position_t( pieces::rank_t::one, pieces::file_t::a ) ) {
                    queen_side_castle_white = false;
                }
                else if ( king_side_castle_white &&
                          src_pos == pieces::position_t( pieces::rank_t::one, pieces::file_t::h ) ) {
                    king_side_castle_white = false;
                }
            }
        }
    }

    void chess_game::black_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                            pieces::position_t const &               src_pos )
    {
        // black castling logic
        if ( ( king_side_castle_black || queen_side_castle_black ) && !moved_piece->colour() ) {
            if ( moved_piece->type() == pieces::name_t::king ) {
                king_side_castle_black  = false;
                queen_side_castle_black = false;
            }
            else if ( moved_piece->type() == pieces::name_t::rook ) {
                if ( queen_side_castle_black &&
                     src_pos == pieces::position_t( pieces::rank_t::eight, pieces::file_t::a ) ) {
                    queen_side_castle_black = false;
                }
                else if ( king_side_castle_black &&
                          src_pos == pieces::position_t( pieces::rank_t::eight, pieces::file_t::h ) ) {
                    king_side_castle_black = false;
                }
            }
        }
    }

    bool chess_game::can_castle( bool const color ) const
    {
        if ( color ) {
            return ( king_side_castle_white || queen_side_castle_white );
        }
        else {
            return ( king_side_castle_black || queen_side_castle_black );
        }
    }

    void chess_game::remove_piece_at( pieces::position_t position ) { game_board.remove_piece_at( position ); }

    std::vector< std::string > chess_game::get_move_history() const { return game_board.get_move_history(); }

    bool chess_game::white_move() const { return state == game_state::white_move || state == game_state::white_check; }

    bool chess_game::black_move() const { return state == game_state::black_move || state == game_state::black_check; }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    void chess_game::load_from_string( const std::string & game_string )
    {
        std::istringstream iss( game_string );

        // Extract board portion (everything before metadata section)
        std::string board_portion = extract_board_portion( game_string );

        // Load the board using the board class function
        game_board.load_from_string( board_portion );

        // Parse metadata section
        parse_metadata_section( game_string );

        update_attack_map();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & sp = game_board.get( pieces::piece::itopos( i, j ).value() );
                if ( sp.piece && sp.piece->type() == pieces::name_t::king ) {
                    if ( sp.piece->colour() ) {
                        white_king = *reinterpret_cast< pieces::king * >( sp.piece.get() );
                    }
                    else {
                        black_king = *reinterpret_cast< pieces::king * >( sp.piece.get() );
                    }
                }
            }
        }
    }

    // Extract the board portion from the game string
    std::string chess_game::extract_board_portion( const std::string & game_string )
    {
        size_t metadata_pos = game_string.find( "--- Game Metadata ---" );
        if ( metadata_pos == std::string::npos ) {
            throw std::invalid_argument( "Invalid game string format: missing metadata section" );
        }

        return game_string.substr( 0, metadata_pos );
    }

    // Parse the metadata section
    void chess_game::parse_metadata_section( const std::string & game_string )
    {
        size_t metadata_pos = game_string.find( "--- Game Metadata ---" );
        if ( metadata_pos == std::string::npos ) {
            throw std::invalid_argument( "Invalid game string format: missing metadata section" );
        }

        std::string        metadata_section = game_string.substr( metadata_pos );
        std::istringstream iss( metadata_section );
        std::string        line;

        // Skip the header line
        std::getline( iss, line );

        while ( std::getline( iss, line ) ) {
            parse_metadata_line( line );
        }
    }

    // Parse individual metadata lines
    void chess_game::parse_metadata_line( const std::string & line )
    {
        if ( line.empty() )
            return;

        if ( line.find( "State:" ) == 0 ) {
            parse_game_state( line );
        }
        else if ( line.find( "  White King-side:" ) == 0 ) {
            king_side_castle_white = parse_castle_right( line );
        }
        else if ( line.find( "  White Queen-side:" ) == 0 ) {
            queen_side_castle_white = parse_castle_right( line );
        }
        else if ( line.find( "  Black King-side:" ) == 0 ) {
            king_side_castle_black = parse_castle_right( line );
        }
        else if ( line.find( "  Black Queen-side:" ) == 0 ) {
            queen_side_castle_black = parse_castle_right( line );
        }
        // Skip "Castling Rights:" header line
    }

    // Parse game state from line
    void chess_game::parse_game_state( const std::string & line )
    {
        size_t colon_pos = line.find( ':' );
        if ( colon_pos == std::string::npos ) {
            throw std::invalid_argument( "Invalid state line format: " + line );
        }

        std::string state_str = line.substr( colon_pos + 1 );
        // Remove leading/trailing whitespace
        state_str.erase( 0, state_str.find_first_not_of( " \t" ) );
        state_str.erase( state_str.find_last_not_of( " \t" ) + 1 );

        // Assuming game_state has a from_string method or constructor
        // If not, you'll need to implement the conversion based on your enum
        state = parse_game_state_enum( state_str );
    }

    // Parse castle right from line
    bool chess_game::parse_castle_right( const std::string & line )
    {
        return line.find( "Available" ) != std::string::npos;
    }

    // Convert string to game_state enum
    game_state chess_game::parse_game_state_enum( const std::string & state_str )
    {
        if ( state_str == "White to move" )
            return game_state::white_move;
        if ( state_str == "Black to move" )
            return game_state::black_move;
        if ( state_str == "White is in check" )
            return game_state::white_check;
        if ( state_str == "Black is in check" )
            return game_state::black_check;
        if ( state_str == "White wins" )
            return game_state::white_wins;
        if ( state_str == "Black wins" )
            return game_state::black_wins;
        if ( state_str == "White offers a draw" )
            return game_state::white_offers_draw;
        if ( state_str == "Black offers a draw" )
            return game_state::black_offers_draw;
        if ( state_str == "White resigns" )
            return game_state::white_resigns;
        if ( state_str == "Black resigns" )
            return game_state::black_resigns;
        if ( state_str == "Game is a draw" )
            return game_state::draw;
        if ( state_str == "Invalid game state" )
            return game_state::invalid_game_state;

        throw std::invalid_argument( "Unknown game state: " + state_str );
    }

    std::string chess_game::to_string() const
    {
        std::stringstream output;

        // Add the board representation
        output << game_board.to_string();

        // Add metadata section
        output << "\n--- Game Metadata ---\n";

        // Game state
        output << "State: " << chess::to_string( state ) << "\n";

        // Castling rights
        output << "Castling Rights:\n";
        output << "  White King-side:  " << ( king_side_castle_white ? "Available" : "Lost" ) << "\n";
        output << "  White Queen-side: " << ( queen_side_castle_white ? "Available" : "Lost" ) << "\n";
        output << "  Black King-side:  " << ( king_side_castle_black ? "Available" : "Lost" ) << "\n";
        output << "  Black Queen-side: " << ( queen_side_castle_black ? "Available" : "Lost" ) << "\n";
        output << " White King Pos: " << pieces::to_string( white_king.get().position() ) << "\n";
        output << " Black King Pos: " << pieces::to_string( black_king.get().position() ) << "\n";

        return output.str();
    }

    void chess_game::update_attack_map()
    {
        game_attack_map.clear();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = game_board.get( pieces::piece::itopos( i, j ).value() );
                if ( !space.piece ) {
                    continue;
                }
                auto attacks = possible_attacks( space );

                for ( auto m : attacks ) {
                    game_attack_map.add_attacker( space, m );
                }
            }
        }
    }

    chess_game::attack_map const chess_game::generate_attack_map( game::board board ) const
    {
        std::chrono::high_resolution_clock::time_point start, end, startl, endl;
        std::chrono::microseconds                      duration;

        start = std::chrono::high_resolution_clock::now();

        attack_map generated_map;
        generated_map.clear();

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & space = board.get( pieces::piece::itopos( i, j ).value() );

                if ( !space.piece ) {
                    continue;
                }

                auto attacks = possible_attacks( board, space );

                for ( auto m : attacks ) {
                    generated_map.add_attacker( space, m );
                }
            }
        }

        end      = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast< std::chrono::microseconds >( end - start );

        return generated_map;
    }

    std::vector< pieces::position_t > chess_game::possible_attacks( game::space const & src ) const
    {
        return possible_attacks( game_board, src );
    }
    std::vector< pieces::position_t > chess_game::possible_attacks( game::board const & b,
                                                                    game::space const & src ) const
    {
        std::vector< pieces::position_t > attack_squares;

        if ( !src.piece ) {
            return attack_squares;
        }

        auto piece_attack_squares = b.possible_attacks( src );

        return piece_attack_squares;
    }

    std::vector< game::space > chess_game::psuedo_possible_moves( game::board         game_board,
                                                                  game::space const & src ) const
    {
        return game_board.possible_moves( src );
    }

    std::vector< move_t > chess_game::legal_moves() const
    {
        auto                  board_copy = game_board;
        std::vector< move_t > moves;
        moves.reserve( 3000 );

        for ( int i = 1; i <= 8; i++ ) {
            for ( int j = 1; j <= 8; j++ ) {
                auto & src = board_copy.get( pieces::piece::itopos( i, j ).value() );

                std::vector< game::space > dsts;
                dsts.reserve( 100 );
                possible_moves( board_copy, src, dsts );

                for ( auto const & dst : dsts ) {
                    moves.push_back( { src, dst } );
                }
            }
        }
        return moves;
    }
    pieces::move_status chess_game::possible_moves( game::space const & src, std::vector< game::space > & moves ) const
    {
        auto board_copy = game_board;
        return possible_moves( board_copy, src, moves );
    }

    // Helper function to validate turn order
    pieces::move_status chess_game::validate_turn( const game::space & src ) const
    {
        if ( !src.piece ) {
            return pieces::move_status::no_piece_to_move;
        }

        bool is_white_piece = src.piece->colour();
        if ( is_white_piece && !white_move() ) {
            return pieces::move_status::invalid_turn;
        }
        if ( !is_white_piece && !black_move() ) {
            return pieces::move_status::invalid_turn;
        }

        return pieces::move_status::valid;
    }

    // Helper function to check if castling squares are safe and empty
    bool chess_game::are_castle_squares_safe( game::board &                      board_copy,
                                              const std::vector< game::space > & castle_squares,
                                              bool                               piece_colour ) const
    {
        for ( const auto & square : castle_squares ) {
            if ( square.piece ) {
                return false;  // Square is occupied
            }
            if ( game_attack_map.has_attackers( square, !piece_colour ) ) {
                return false;  // Square is under attack
            }
        }
        return true;
    }

    // Helper function to get castling squares for white kingside
    std::vector< game::space > chess_game::get_white_kingside_squares( game::board & board_copy ) const
    {
        std::vector< game::space > castle_squares;
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 6 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 7 ).value() ) );
        return castle_squares;
    }

    // Helper function to get castling squares for white queenside
    std::vector< game::space > chess_game::get_white_queenside_squares( game::board & board_copy ) const
    {
        std::vector< game::space > castle_squares;
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 2 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 3 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 1, 4 ).value() ) );
        return castle_squares;
    }

    // Helper function to get castling squares for black kingside
    std::vector< game::space > chess_game::get_black_kingside_squares( game::board & board_copy ) const
    {
        std::vector< game::space > castle_squares;
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 6 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 7 ).value() ) );
        return castle_squares;
    }

    // Helper function to get castling squares for black queenside
    std::vector< game::space > chess_game::get_black_queenside_squares( game::board & board_copy ) const
    {
        std::vector< game::space > castle_squares;
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 2 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 3 ).value() ) );
        castle_squares.push_back( board_copy.get( pieces::piece::itopos( 8, 4 ).value() ) );
        return castle_squares;
    }

    // Helper function to add white castling moves
    void chess_game::add_white_castling_moves( game::board & board_copy, const game::space & src,
                                               std::vector< game::space > & possible_moves ) const
    {
        bool king_in_check = game_attack_map.has_attackers( src, !src.piece->colour() );

        // Kingside castling
        if ( !king_in_check && king_side_castle_white ) {
            auto castle_squares = get_white_kingside_squares( board_copy );
            if ( are_castle_squares_safe( board_copy, castle_squares, src.piece->colour() ) ) {
                possible_moves.push_back( board_copy.get( { pieces::rank_t::one, pieces::file_t::g } ) );
            }
        }

        // Queenside castling
        if ( !king_in_check && queen_side_castle_white ) {
            auto castle_squares = get_white_queenside_squares( board_copy );
            if ( are_castle_squares_safe( board_copy, castle_squares, src.piece->colour() ) ) {
                possible_moves.push_back( board_copy.get( { pieces::rank_t::one, pieces::file_t::c } ) );
            }
        }
    }

    // Helper function to add black castling moves
    void chess_game::add_black_castling_moves( game::board & board_copy, const game::space & src,
                                               std::vector< game::space > & possible_moves ) const
    {
        bool king_in_check = game_attack_map.has_attackers( src, !src.piece->colour() );

        // Kingside castling
        if ( !king_in_check && king_side_castle_black ) {
            auto castle_squares = get_black_kingside_squares( board_copy );
            if ( are_castle_squares_safe( board_copy, castle_squares, src.piece->colour() ) ) {
                possible_moves.push_back( board_copy.get( { pieces::rank_t::eight, pieces::file_t::g } ) );
            }
        }

        // Queenside castling
        if ( !king_in_check && queen_side_castle_black ) {
            auto castle_squares = get_black_queenside_squares( board_copy );
            if ( are_castle_squares_safe( board_copy, castle_squares, src.piece->colour() ) ) {
                possible_moves.push_back( board_copy.get( { pieces::rank_t::eight, pieces::file_t::c } ) );
            }
        }
    }

    // Helper function to add castling moves for kings
    void chess_game::add_castling_moves( game::board & board_copy, const game::space & src,
                                         std::vector< game::space > & possible_moves ) const
    {
        if ( src.piece->type() != pieces::name_t::king ) {
            return;
        }

        if ( src.piece->colour() ) {
            add_white_castling_moves( board_copy, src, possible_moves );
        }
        else {
            add_black_castling_moves( board_copy, src, possible_moves );
        }
    }

    bool chess_game::is_king_move_safe( game::board & board_copy, const game::space & src,
                                        const game::space & dst ) const
    {
        // Copy the king piece
        std::unique_ptr< pieces::piece > king_piece =
            pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );

        // Save any piece at destination for reversion
        std::unique_ptr< pieces::piece > captured_piece = nullptr;
        if ( board_copy.get( dst.position() ).piece ) {
            captured_piece = pieces::piece::copy_piece( *board_copy.get( dst.position() ).piece );
            board_copy.remove_piece_at( dst.position() );
        }

        // Simulate move
        board_copy.remove_piece_at( src.position() );
        board_copy.add_piece_at( *king_piece, dst.position() );

        // Generate attack map and check if king is safe
        auto temp_attack_map = generate_attack_map( board_copy );
        bool king_safe       = !temp_attack_map.has_attackers( dst, !king_piece->colour() );

        // Revert move
        board_copy.remove_piece_at( dst.position() );
        board_copy.add_piece_at( *king_piece, src.position() );
        if ( captured_piece )
            board_copy.add_piece_at( *captured_piece, dst.position() );

        return king_safe;
    }

    bool chess_game::is_non_king_move_safe( game::board & board_copy, const game::space & src,
                                            const game::space & dst ) const
    {
        // Copy the moving piece
        std::unique_ptr< pieces::piece > moving_piece =
            pieces::piece::copy_piece( *board_copy.get( src.position() ).piece );

        // Save any piece at destination for later reversion
        std::unique_ptr< pieces::piece > captured_piece = nullptr;
        if ( board_copy.get( dst.position() ).piece ) {
            captured_piece = pieces::piece::copy_piece( *board_copy.get( dst.position() ).piece );
            board_copy.remove_piece_at( dst.position() );
        }

        // Perform simulated move
        board_copy.remove_piece_at( src.position() );
        board_copy.add_piece_at( *moving_piece, dst.position() );

        // Get king's current position
        const game::space & king_space = moving_piece->colour() ? board_copy.get( white_king.get().position() )
                                                                : board_copy.get( black_king.get().position() );

        // Generate attack map and check king safety
        auto temp_attack_map = generate_attack_map( board_copy );
        bool king_safe       = !temp_attack_map.has_attackers( king_space, !moving_piece->colour() );

        // Revert move
        board_copy.remove_piece_at( dst.position() );
        board_copy.add_piece_at( *moving_piece, src.position() );
        if ( captured_piece )
            board_copy.add_piece_at( *captured_piece, dst.position() );

        return king_safe;
    }

    // Helper function to validate individual moves
    bool chess_game::is_move_legal( game::board & board_copy, const game::space & src, const game::space & dst ) const
    {
        if ( src.piece->type() == pieces::name_t::king ) {
            return is_king_move_safe( board_copy, src, dst );
        }
        else {
            return is_non_king_move_safe( board_copy, src, dst );
        }
    }

    // Helper function to filter illegal moves
    void chess_game::filter_illegal_moves( game::board & board_copy, const game::space & src,
                                           std::vector< game::space > & possible_moves ) const
    {
        std::erase_if( possible_moves, [this, &src, &board_copy]( const game::space & dst ) {
            return !is_move_legal( board_copy, src, dst );
        } );
    }

    // Main function - now much cleaner and more focused
    pieces::move_status chess_game::possible_moves( game::board & board_copy, const game::space & src,
                                                    std::vector< game::space > & possible_moves ) const
    {
        // Validate turn
        auto turn_status = validate_turn( src );
        if ( turn_status != pieces::move_status::valid ) {
            return turn_status;
        }

        // Get basic possible moves
        possible_moves = game_board.possible_moves( src );

        // Add castling moves if applicable
        add_castling_moves( board_copy, src, possible_moves );

        // Filter out moves that would leave king in check
        filter_illegal_moves( board_copy, src, possible_moves );

        return pieces::move_status::valid;
    }

    std::vector< game::space > chess_game::find_attackers( game::space const & src, bool const victim_color ) const
    {
        std::vector< game::space > attackers;

        auto moves = legal_moves();

        for ( auto m : moves ) {

            if ( m.second == src ) {
                if ( m.first.piece && m.first.piece->colour() && m.first.piece->colour() != victim_color ) {
                    attackers.push_back( m.second );
                }
            }
        }

        return attackers;
    }

}  // namespace chess