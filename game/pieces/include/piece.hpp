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

        void move( position_t const dst );

        virtual std::vector< position_t > possible_moves() const = 0;

        std::unique_ptr< piece >        copy_piece() const;
        static std::unique_ptr< piece > copy_piece( piece const & piece );

        virtual ~piece() {};

    protected:
        piece( bool const white, name_t const name, rank_t const rank, file_t const file );
        name_t name;
        rank_t rank;
        file_t file;
        bool   is_white;

        // this helper function takes in the index of a rank and file and returns a position_t of it
        // returns the coordiantes that are theoretically reachable from this square
        static std::optional< position_t > itopos( int const rank, int const file );
    };
}  // namespace chess::pieces

#endif