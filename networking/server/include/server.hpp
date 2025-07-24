#ifndef CHESS_NETWORKING_SERVER_HPP
#define CHESS_NETWORKING_SERVER_HPP

#include <condition_variable>
#include <ixwebsocket/IXWebSocketServer.h>
#include <mutex>
#include <string>

namespace chess::networking {

    const std::string update_board_command( "/get/board/" );
    const std::string possible_moves_command( "/get/possible_moves/" );
    const std::string move_command( "/post/move/" );
    const std::string status_command( "/get/status/" );

    class server {
    public:
        server( int port );
        ~server();

        bool        open();
        void        close();
        bool        write( const std::string & message );
        std::string read();  // Blocking read from any client

    private:
        int                              port;
        ix::WebSocketServer              websocket_server;
        std::shared_ptr< ix::WebSocket > client_socket;
        std::string                      last_message;
        std::mutex                       message_mutex;
        std::condition_variable          message_cv;
    };

}  // namespace chess::networking

#endif  // CHESS_NETWORKING_SERVER_HPP