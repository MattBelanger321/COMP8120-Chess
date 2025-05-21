#include <knight.hpp>

namespace chess::pieces {
    knight::knight( bool const white, rank_t const rank, file_t const file ) :
        piece( white, name_t::knight, rank, file )
    {
    }

    std::vector< position_t > knight::possible_moves() const
    {
        std::vector< position_t > moves;

        if ( auto pos = itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) + 2 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) + 2, static_cast< int >( file ) + 1 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) + 2 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) - 2, static_cast< int >( file ) + 1 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) - 2 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) + 2, static_cast< int >( file ) - 1 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) - 2 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        if ( auto pos = itopos( static_cast< int >( rank ) - 2, static_cast< int >( file ) - 1 ); pos.has_value() ) {
            moves.push_back( *pos );
        }

        return moves;
    }

}  // namespace chess::pieces