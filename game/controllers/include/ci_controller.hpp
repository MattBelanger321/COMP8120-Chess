#ifndef __CHESS__CONTROLLER__CI_CONTROLLER__
#define __CHESS__CONTROLLER__CI_CONTROLLER__

#include <controller.hpp>

#include <thread>

namespace chess::controller {
    class ci_controller : public controller {
    private:
        bool        should_close;
        std::thread runner;

        void render();

    public:
        ci_controller() : controller() {}

        void run();
        void close();

        ~ci_controller();
    };
}  // namespace chess::controller

#endif