#ifndef __CHESS__CLIENT__GAME__
#define __CHESS__CLIENT__GAME__

#include "piece.hpp"
#include <board.hpp>
#include <client.hpp>
#include <optional>
#include <space.hpp>
#include <vector>

namespace chess::networking {

    class client_game {
    public:
        client_game( std::string const & url );

        bool                       move( game::space const & sp );
        std::vector< game::space > possible_moves( game::space const & sp );
        void                       select_space( game::space const & sp );
        std::string                get_state();
        game::space                get( pieces::position_t const & pos );

        std::optional< game::space > selected_space;

    private:
        game::board game_board;
        client      cli;
    };

}  // namespace chess::networking

#endif