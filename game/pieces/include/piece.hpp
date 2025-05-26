#ifndef __CHESS__PIECES__PIECE__
#define __CHESS__PIECES__PIECE__

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
namespace chess::pieces {

    enum class name_t {
        rook,
        knight,
        bishop,
        king,
        queen,
        pawn,
    };

    enum class file_t {
        a = 1,
        b = 2,
        c = 3,
        d = 4,
        e = 5,
        f = 6,
        g = 7,
        h = 8,
    };

    enum class rank_t {
        one   = 1,
        two   = 2,
        three = 3,
        four  = 4,
        five  = 5,
        six   = 6,
        seven = 7,
        eight = 8,
    };

    enum class move_status {
        valid,
        piece_error,            // the piece doesnt move like that
        board_state_error,      // the board state doesnt allow for that (ex piece in the way)
        invalid_turn,           // not your turn
        doesnt_leave_check,     // you were in check and did not avoid it
        enters_check,           // this moves puts you in check
        no_piece_to_move,       // there is no piece to move
        no_space_to_move_from,  // no src space provided
        illegal_move,           // this move was not provided by the possible moves list
    };

    inline std::string to_string( move_status const & status )
    {

        switch ( status ) {

        case move_status::valid:
            return "VALID";
        case move_status::piece_error:
            return "PIECE DOES NOT MOVE LIKE THAT";
        case move_status::board_state_error:
            return "THE CURRENT BOARD STATE DOES NOT ALLOW FOR THIS MOVE";
        case move_status::invalid_turn:
            return "IT IS NOT YOUR TURN";
        case move_status::doesnt_leave_check:
            return "THIS MOVE FAILS TO LEAVE CHECK";
        case move_status::enters_check:
            return "THIS MOVE CAUSES YOU TO ENTER CHECK";
        case move_status::no_piece_to_move:
            return "THERE IS NO PIECE AT THIS SPACE TO MOVE";
        case move_status::no_space_to_move_from:
            return "YOU NEED TO SELECT A SPACE FIRST";
        }

        return "INVALID STATUS CODE";
    }

    using position_t = std::pair< rank_t, file_t >;

    inline std::string to_string( position_t position )
    {
        std::stringstream ss;
        switch ( position.second ) {

        case file_t::a:
            ss << "A";
            break;
        case file_t::b:
            ss << "B";
            break;
        case file_t::c:
            ss << "C";
            break;
        case file_t::d:
            ss << "D";
            break;
        case file_t::e:
            ss << "E";
            break;
        case file_t::f:
            ss << "F";
            break;
        case file_t::g:
            ss << "G";
            break;
        case file_t::h:
            ss << "H";
            break;
        }

        switch ( position.first ) {
        case rank_t::one:
            ss << "1";
            break;
        case rank_t::two:
            ss << "2";
            break;
        case rank_t::three:
            ss << "3";
            break;
        case rank_t::four:
            ss << "4";
            break;
        case rank_t::five:
            ss << "5";
            break;
        case rank_t::six:
            ss << "6";
            break;
        case rank_t::seven:
            ss << "7";
            break;
        case rank_t::eight:
            ss << "8";
            break;
        }

        return ss.str();
    }

    class piece {
    public:
        piece( piece const & src );

        bool       colour() const;
        name_t     type() const;
        position_t position() const;

        // this function never fails
        void place( position_t const pos );

        // this function will fail if this move is not vaild
        move_status move( position_t const dst );

        virtual std::vector< position_t > possible_moves() const = 0;

        std::unique_ptr< piece >        copy_piece() const;
        static std::unique_ptr< piece > copy_piece( piece const & piece );

        // this helper function takes in the index of a rank and file and returns a position_t of it
        // returns the coordiantes that are theoretically reachable from this square
        static std::optional< position_t > itopos( int const rank, int const file );

        virtual ~piece() {};

    protected:
        piece( bool const white, name_t const name, rank_t const rank, file_t const file );
        name_t name;
        rank_t rank;
        file_t file;
        bool   is_white;
    };
}  // namespace chess::pieces

#endif