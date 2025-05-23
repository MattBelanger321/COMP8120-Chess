#include <algorithm>
#include <memory>
#include <piece.hpp>
#include <space.hpp>

namespace chess::game {

    space::space( bool const white, pieces::rank_t const rank, pieces::file_t const file,
                  std::unique_ptr< pieces::piece > && piece ) :
        is_white( white ), file( file ), rank( rank ), piece( std::move( piece ) )
    {
    }

    space::space( space const & src ) : is_white( src.is_white ), file( src.file ), rank( src.rank )
    {
        if ( src.piece ) {
            piece = src.piece->copy_piece();
        }
    }

    space & space::operator=( space const & other )
    {
        if ( this != &other ) {
            is_white = other.is_white;
            file     = other.file;
            rank     = other.rank;

            if ( other.piece ) {
                piece = other.piece->copy_piece();  // deep copy
            }
            else {
                piece.reset();  // clear the pointer if other has no piece
            }
        }
        return *this;
    }

    bool               space::colour() const { return is_white; }
    pieces::position_t space::position() const { return { rank, file }; }

    std::vector< pieces::position_t > space::possible_moves() const
    {
        if ( piece ) {
            auto moves = piece->possible_moves();

            // sort by letter then rank
            std::sort( moves.begin(), moves.end(), []( const pieces::position_t & a, const pieces::position_t & b ) {
                if ( static_cast< int >( a.second ) == static_cast< int >( b.second ) )
                    return static_cast< int >( a.first ) < static_cast< int >( b.first );
                return static_cast< int >( a.second ) < static_cast< int >( b.second );
            } );

            return moves;
        }
        return {};
    }

}  // namespace chess::game
