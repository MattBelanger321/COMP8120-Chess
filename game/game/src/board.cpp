#include <board.hpp>
#include <sstream>

namespace chess::game {

    board::board( bool const empty )
    {
        for ( int i = 1; i <= 8; i++ ) {
            file_t file_map;
            auto   rank = static_cast< pieces::rank_t >( i );
            for ( int j = 1; j <= 8; j++ ) {
                auto file = static_cast< pieces::file_t >( j );

                file_map.emplace( file, space( ( i + j ) % 2 == 1, rank, file ) );
            }
            game_board[rank] = std::move( file_map );
        }
    }

    std::string board::to_string()
    {
        std::stringstream serialized;

        for ( int i = 8; i > 0; i++ ) {
            serialized << "|";
            for ( int j = 1; j <= 8; j++ ) {
                auto rank = static_cast< pieces::rank_t >( i );
                auto file = static_cast< pieces::file_t >( j );

                if ( game_board.at( rank ).at( file ).colour() ) {
                    serialized << "w|";
                }
                else {
                    serialized << "b|";
                }
            }
            serialized << "\n";
        }
        return serialized.str();
    }

}  // namespace chess::game