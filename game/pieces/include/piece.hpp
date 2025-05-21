#ifndef __CHESS__PIECES__PIECE__
#define __CHESS__PIECES__PIECE__

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
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
    };

    enum class file_t {
        one,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
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

    protected:
        piece( bool const white, name_t const name, rank_t const rank, file_t const file );

    private:
        name_t name;
        rank_t rank;
        file_t file;
        bool   is_white;
    };
}  // namespace chess::pieces

#endif