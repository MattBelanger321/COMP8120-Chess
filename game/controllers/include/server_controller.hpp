#ifndef __CHESS__CONTROLLER__SERVER_CONTROLLER__
#define __CHESS__CONTROLLER__SERVER_CONTROLLER__

#include <atomic>
#include <controller.hpp>
#include <server.hpp>

namespace chess::controller {
    class server_controller : public controller {
    public:
        server_controller( int port );
        ~server_controller();

        void start_listening();
        void close();

    private:
        networking::server server;
        std::atomic_flag   should_close;

        std::thread listener_thread;
        std::thread sender_thread;

        void listen();

        // client handlers
        void update_board_handler();
        void possible_moves_handler( std::string const & move );
        void move( std::string const & payload );
    };
}  // namespace chess::controller
#endif