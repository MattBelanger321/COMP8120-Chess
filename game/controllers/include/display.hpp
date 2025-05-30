#ifndef __CHESS__CONTROLLER__DISPLAY_CONTROLLER__
#define __CHESS__CONTROLLER__DISPLAY_CONTROLLER__

#include <controller.hpp>
#include <imgui.h>

#include <dear_imgui_chessboard.hpp>

namespace chess::controller {
    class display : public controller {
    public:
        display( unsigned int width, unsigned int height );

        void render();

    private:
        unsigned int width;
        unsigned int height;

        imgui_chessboard board;
    };
}  // namespace chess::controller

#endif