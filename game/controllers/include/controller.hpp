#ifndef __CHESS__CONTROLLER__CONTROLLER__
#define __CHESS__CONTROLLER__CONTROLLER__

#include <game.hpp>
#include <optional>
#include <piece.hpp>
#include <space.hpp>

#include <mutex>

namespace chess::controller {
    class controller {
    public:
        void select_space( pieces::position_t const & pos );
        bool move( game::space const & dst );

        virtual ~controller() {}

    protected:
        static std::mutex            game_mutex;
        static chess_game            game;
        std::optional< game::space > selected_space;
    };
}  // namespace chess::controller

#endif