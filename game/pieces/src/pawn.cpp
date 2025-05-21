#include "piece.hpp"
#include <pawn.hpp>

namespace chess::pieces {
    pawn::pawn( bool const white, rank_t const rank, file_t const file ) : piece( white, name_t::pawn, rank, file ) {}

    std::vector< position_t > pawn::possible_moves() const
    {
        std::vector< position_t > moves;

        // pawn on the back rank
        if ( static_cast< int >( rank ) >= static_cast< int >( rank_t::h ) ||
             static_cast< int >( rank ) <= static_cast< int >( rank_t::a ) ) {
            // TODO: log error
            return {};
        }

        if ( is_white ) {
            moves.push_back( itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) ) );

            if ( static_cast< int >( file ) <= static_cast< int >( file_t::seven ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) + 1 ) );
            }

            if ( static_cast< int >( file ) >= static_cast< int >( file_t::two ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) - 1 ) );
            }

            // first move
            if ( static_cast< int >( rank ) == static_cast< int >( rank_t::b ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) + 2, static_cast< int >( file ) ) );
            }
        }
        else {
            moves.push_back( itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) ) );

            if ( static_cast< int >( file ) <= static_cast< int >( file_t::seven ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) + 1 ) );
            }

            if ( static_cast< int >( file ) >= static_cast< int >( file_t::two ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) - 1 ) );
            }

            // first move
            if ( static_cast< int >( rank ) == static_cast< int >( rank_t::g ) ) {
                moves.push_back( itopos( static_cast< int >( rank ) - 2, static_cast< int >( file ) ) );
            }
        }

        return moves;
    }

}  // namespace chess::pieces