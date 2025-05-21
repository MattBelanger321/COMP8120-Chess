#ifndef __CHESS__GAME__SPACE__
#define __CHESS__GAME__SPACE__

#include <memory>
#include <piece.hpp>

namespace chess::game {

    class space {

    private:
        const pieces::rank_t rank;
        const pieces::file_t file;
        bool                 is_white;

    public:
        space( bool const white, pieces::rank_t const rank, pieces::file_t const file,
               std::unique_ptr< pieces::piece > && piece = {} );

        std::vector< pieces::position_t >        possible_moves() const;
        bool                                     colour() const;
        std::unique_ptr< pieces::piece > const & get_piece() const;
        pieces::position_t                       position() const;

        std::unique_ptr< pieces::piece > piece;
    };
}  // namespace chess::game

#endif