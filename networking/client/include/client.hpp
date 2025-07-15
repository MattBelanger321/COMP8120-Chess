#ifndef CHESS_NETWORKING_CLIENT_HPP
#define CHESS_NETWORKING_CLIENT_HPP

#include <condition_variable>
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include <string>

namespace chess::networking {

    class client {
    public:
        client( const std::string & url );
        ~client();

        bool        open();
        void        close();
        bool        write( const std::string & message );
        std::string read();  // Blocking read

    private:
        std::string             url;
        ix::WebSocket           websocket;
        std::string             last_message;
        std::mutex              message_mutex;
        std::condition_variable message_cv;
    };

}  // namespace chess::networking

#endif  // CHESS_NETWORKING_CLIENT_HPP