#include "server.hpp"
#include <iostream>

namespace chess::networking {

    server::server( int port ) : port( port ), websocket_server( port )
    {
        websocket_server.setOnConnectionCallback(
            [this]( std::weak_ptr< ix::WebSocket > weak_socket, std::shared_ptr< ix::ConnectionState > state ) {
                if ( auto socket = weak_socket.lock() ) {
                    client_socket = socket;

                    socket->setOnMessageCallback( [this]( const ix::WebSocketMessagePtr & msg ) {
                        if ( msg->type == ix::WebSocketMessageType::Message ) {
                            {
                                std::lock_guard< std::mutex > lock( message_mutex );
                                last_message = msg->str;
                            }
                            message_cv.notify_one();
                        }
                    } );
                }
            } );
    }

    server::~server() { close(); }

    bool server::open()
    {
        auto result = websocket_server.listen();
        if ( !result.first ) {
            std::cerr << "Server listen failed: " << result.second << std::endl;
            return false;
        }

        websocket_server.start();
        return true;
    }

    void server::close() { websocket_server.stop(); }

    bool server::write( const std::string & message )
    {
        if ( client_socket ) {
            return client_socket->send( message ).success;
        }
        return false;
    }

    std::string server::read()
    {
        std::unique_lock< std::mutex > lock( message_mutex );
        message_cv.wait( lock, [this]() { return !last_message.empty(); } );

        std::string message = last_message;
        last_message.clear();
        return message;
    }

}  // namespace chess::networking