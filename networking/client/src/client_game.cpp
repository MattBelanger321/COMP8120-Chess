#include <client_game.hpp>

namespace chess::networking {

    client_game::client_game( std::string const & url ) : cli( url ) { cli.open(); }
}  // namespace chess::networking