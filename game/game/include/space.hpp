#ifndef __CHESS__GAME__SPACE__
#define __CHESS__GAME__SPACE__

#include <memory>
#include <piece.hpp>

namespace chess::game {

    class space {

    private:
        pieces::rank_t rank;
        pieces::file_t file;
        bool           is_white;

    public:
        space( bool const white, pieces::rank_t const rank, pieces::file_t const file,
               std::unique_ptr< pieces::piece > && piece = {} );

        space( space const & src );

        space & operator=( space const & other );

        std::vector< pieces::position_t >        possible_moves() const;
        bool                                     colour() const;
        std::unique_ptr< pieces::piece > const & get_piece() const;
        pieces::position_t                       position() const;

        std::unique_ptr< pieces::piece > piece;

        friend bool operator==( const space & lhs, const space & rhs )
        {
            return lhs.rank == rhs.rank && lhs.file == rhs.file;
        }

        friend bool operator==( const pieces::position_t & lhs, const space & rhs )
        {
            return lhs.first == rhs.rank && lhs.second == rhs.file;
        }

        friend bool operator==( const space & lhs, const pieces::position_t & rhs )
        {
            return lhs.rank == rhs.first && lhs.file == rhs.second;
        }
    };
}  // namespace chess::game

#endif