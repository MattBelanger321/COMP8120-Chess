#include <memory>
#include <space.hpp>

namespace chess::game {

    space::space( bool const white, pieces::rank_t const rank, pieces::file_t const file,
                  std::unique_ptr< pieces::piece > && piece ) :
        is_white( white ), file( file ), rank( rank ), piece( std::move( piece ) )
    {
    }

    bool               space::colour() const { return is_white; }
    pieces::position_t space::position() const { return { rank, file }; }

    std::vector< pieces::position_t > space::possible_moves() const
    {
        if ( piece ) {
            return piece->possible_moves();
        }
        return {};
    }

}  // namespace chess::game
