#include <client.hpp>
#include <iostream>

namespace chess::networking {

    client::client( const std::string & url ) : url( url )
    {
        websocket.setUrl( url );

        websocket.setOnMessageCallback( [this]( const ix::WebSocketMessagePtr & msg ) {
            if ( msg->type == ix::WebSocketMessageType::Message ) {
                {
                    std::lock_guard< std::mutex > lock( message_mutex );
                    last_message = msg->str;
                }
                message_cv.notify_one();
            }
        } );
    }

    client::~client() { close(); }

    bool client::open()
    {
        websocket.start();
        return true;
    }

    void client::close() { websocket.stop(); }

    bool client::write( const std::string & message ) { return websocket.send( message ).success; }

    std::string client::read()
    {
        std::unique_lock< std::mutex > lock( message_mutex );
        message_cv.wait( lock, [this]() { return !last_message.empty(); } );

        std::string message = last_message;
        last_message.clear();
        return message;
    }

}  // namespace chess::networking
