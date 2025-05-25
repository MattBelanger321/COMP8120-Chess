#ifndef __CHESS__CONTROLLER__CONTROLLER__
#define __CHESS__CONTROLLER__CONTROLLER__

#include <game.hpp>
#include <optional>
#include <piece.hpp>
#include <space.hpp>

namespace chess::controller {
    class controller {
    public:
        void select_space( pieces::position_t const & pos );
        void move( game::space const & dst );

    protected:
        static std::mutex            game_mutex;
        static chess_game            game;
        std::optional< game::space > selected_space;
    };
}  // namespace chess::controller

#endif