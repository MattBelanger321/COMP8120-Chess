#include "bishop.hpp"
#include "king.hpp"
#include "knight.hpp"
#include "pawn.hpp"
#include "queen.hpp"
#include <memory>
#include <optional>
#include <piece.hpp>
#include <stdexcept>
#include <string>

#include <rook.hpp>
namespace chess::pieces {

    piece::piece( bool const white, name_t const name, rank_t const rank, file_t const file )
    {
        is_white   = white;
        this->name = name;
        this->file = file;
        this->rank = rank;
    }

    piece::piece( piece const & src ) {}

    std::unique_ptr< piece > piece::copy_piece() const { return copy_piece( *this ); }

    std::unique_ptr< piece > piece::copy_piece( piece const & src )
    {
        std::unique_ptr< piece > cpy;
        switch ( src.type() ) {
        case name_t::rook:
            cpy = std::make_unique< rook >( src.colour(), src.rank, src.file );
            break;
        case name_t::knight:
            cpy = std::make_unique< knight >( src.colour(), src.rank, src.file );
            break;
        case name_t::bishop:
            cpy = std::make_unique< bishop >( src.colour(), src.rank, src.file );
            break;
        case name_t::king:
            cpy = std::make_unique< king >( src.colour(), src.rank, src.file );
            break;
        case name_t::queen:
            cpy = std::make_unique< queen >( src.colour(), src.rank, src.file );
            break;
        case name_t::pawn:
            cpy = std::make_unique< pawn >( src.colour(), src.rank, src.file );
            break;
        }

        return cpy;
    }

    bool       piece::colour() const { return is_white; }
    position_t piece::position() const { return { rank, file }; }
    name_t     piece::type() const { return name; }

    void piece::move( position_t const pos )
    {
        rank = pos.first;
        file = pos.second;
    }

    std::optional< position_t > piece::itopos( int const rank, int const file )
    {

        if ( rank > 8 ) {
            return {};
        }

        if ( file > 8 ) {
            return {};
        }

        if ( rank < 1 ) {
            return {};
        }

        if ( file < 1 ) {
            return {};
        }

        return std::make_optional< position_t >( { static_cast< rank_t >( rank ), static_cast< file_t >( file ) } );
    }
}  // namespace chess::pieces
