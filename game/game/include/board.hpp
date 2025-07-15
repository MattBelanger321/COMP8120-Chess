#ifndef __CHESS__GAME__BOARD__
#define __CHESS__GAME__BOARD__

#include <map>
#include <piece.hpp>
#include <space.hpp>

namespace chess::game {

    using file_t = std::map< pieces::file_t, space >;   // cols
    using rank_t = std::map< pieces::rank_t, file_t >;  // rows

    class board {

    private:
        // NOTE: do to the nature of the entries of this map the [] operator will cause a compile error
        // please use the at() function
        rank_t game_board;

        std::vector< std::string > move_history;

        // an init function to place all the pieces on the board
        void place_pieces();

        // these functions implement collision detection into other pieces
        // does not handle castling and pawn moves, nor does it handle checks
        void filter_pawn_moves( space const & current, std::vector< space > & moves ) const;
        void filter_knight_moves( space const & current, std::vector< space > & moves ) const;
        void filter_bishop_moves( space const & current, std::vector< space > & moves ) const;
        void filter_rook_moves( space const & current, std::vector< space > & moves ) const;
        void filter_queen_moves( space const & current, std::vector< space > & moves ) const;
        void filter_king_moves( space const & current, std::vector< space > & moves ) const;

        void add_pawn_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        void add_knight_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        void add_bishop_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        void add_rook_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        void add_queen_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        void add_king_attacks( space const & current, std::vector< pieces::position_t > & moves ) const;
        // Parse the entire board string
        void parse_board_string( const std::string & board_string );

        // Parse a single rank line
        void parse_rank_line( const std::string & line, int rank );

        // Find the first '|' separator after rank number
        size_t find_first_separator( const std::string & line, int rank );

        // Extract symbol at current position
        char extract_symbol_at_position( const std::string & line, size_t pos, int rank, int file );

        // Find next '|' separator
        size_t find_next_separator( const std::string & line, size_t current_pos, int file );

        // Process a single board symbol (piece or empty space)
        void process_board_symbol( char symbol, int rank, int file );

        // Check if symbol represents empty space
        bool is_empty_space( char symbol );

        // Create piece from symbol and place it on board
        void create_piece_from_symbol( char symbol, pieces::rank_t rank, pieces::file_t file );

    public:
        // empty is set if there should be no pieces put on the board
        board( bool const empty = false );
        // contstruct board from frozen string encoding
        board( const std::string & board_string );

        void load_from_string( std::string const & board_string );

        std::vector< space >              possible_moves( space const & src ) const;
        std::vector< pieces::position_t > possible_attacks( space const & src ) const;

        // this function will perform illegal moves (or legal ones) if src contains a piece
        pieces::move_status move_force( pieces::position_t const & src, pieces::position_t const & dst );
        // this function checks for logic then moves if valid
        pieces::move_status move( pieces::position_t const & src, pieces::position_t const & dst );

        std::vector< std::string > get_move_history() const;

        std::string to_string() const;

        // returns a reference to the space at the given position
        space const & get( pieces::position_t pos ) const;

        void        remove_piece_at( pieces::position_t const position );
        static void remove_piece_at( rank_t & board, pieces::position_t position );

        void        add_piece_at( pieces::piece const & p, pieces::position_t const position );
        static void add_piece_at( rank_t & board, pieces::piece const & p, pieces::position_t const position );

        void reset( bool const empty = false );

        // returns true if after a hypothetical move from src to dst, target is under attack by the
        // not victims team, returns false otherwise, or if src has no piece
        // victim colour is true if black is the threat, and false if white is the threat
        // this function returns true even if target is under threat regardless
        bool determine_threat( space const & src, space const & dst, space const & target,
                               bool const victim_colour ) const;

        file_t const & operator[]( pieces::rank_t rank ) const { return game_board.at( rank ); }

        file_t & operator[]( pieces::rank_t rank ) { return game_board.at( rank ); }
    };
}  // namespace chess::game

#endif
