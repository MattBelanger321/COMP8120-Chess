#ifndef __CHESS__CONTROLLER__CONTROLLER__
#define __CHESS__CONTROLLER__CONTROLLER__

#include <game.hpp>
#include <iostream>
#include <optional>
#include <piece.hpp>
#include <space.hpp>

#include <mutex>

namespace chess::controller {
    class controller {
    public:
        void                select_space( pieces::position_t const & pos );
        pieces::move_status move( game::space const & dst );

        virtual ~controller() {}

    protected:
        controller() {}
        controller( std::string const & board_state )
        {
            std::lock_guard guard( game_mutex );
            game.load_from_string( board_state );
        }

        static std::mutex            game_mutex;
        static chess_game            game;
        std::optional< game::space > selected_space;
    };
}  // namespace chess::controller

#endif