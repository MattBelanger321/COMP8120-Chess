#ifndef __CHESS__CLIENT__GAME__
#define __CHESS__CLIENT__GAME__

#include "piece.hpp"
#include <board.hpp>
#include <client.hpp>
#include <optional>
#include <space.hpp>
#include <vector>

namespace chess::networking {

    const std::string update_board_command( "/get/board/" );
    const std::string possible_moves_command( "/get/possible_moves/" );
    const std::string move_command( "/post/move/" );
    const std::string status_command( "/get/status/" );

    class client_game {
    public:
        client_game( std::string const & url );

        bool                       move( game::space const & sp );
        std::vector< game::space > possible_moves( game::space const & sp );
        void                       select_space( game::space const & sp );
        std::string                get_state();
        game::space                get( pieces::position_t const & pos ) { return game_board.get( pos ); }

        void update_board();

        std::optional< game::space > selected_space;
        game::board                  game_board;
        std::string                  game_state;

    private:
        client cli;

        std::mutex  board_mutex;
        std::mutex  client_mutex;
        std::thread get_board_updates;

        std::vector< game::space > string_to_spaces( std::string const & str );
    };

}  // namespace chess::networking

#endif