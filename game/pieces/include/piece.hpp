#ifndef __CHESS__PIECES__PIECE__
#define __CHESS__PIECES__PIECE__

#include <optional>
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

    enum class rank_t {
        a = 1,
        b = 2,
        c = 3,
        d = 4,
        e = 5,
        f = 6,
        g = 7,
        h = 8,
    };

    enum class file_t {
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

    class piece {
    public:
        bool       colour() const;
        name_t     type() const;
        position_t position() const;

        void move( position_t const dst );

        virtual std::vector< position_t > possible_moves() const = 0;

        virtual ~piece();

        // this helper function takes in the index of a rank and file and returns a position_t of it
        // returns the coordiantes that are theoretically reachable from this square
        static std::optional< position_t > itopos( int const rank, int const file );

    protected:
        piece( bool const white, name_t const name, rank_t const rank, file_t const file );
        name_t name;
        rank_t rank;
        file_t file;
        bool   is_white;
    };
}  // namespace chess::pieces

#endif