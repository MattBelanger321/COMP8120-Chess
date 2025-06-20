#include "controller.hpp"
#include "piece.hpp"
#include "space.hpp"
#include <ai_controller.hpp>
#include <iostream>
#include <thread>

namespace chess::controller {
    ai_controller::ai_controller( chromosome_t chromie ) : controller(), chromosome( chromie ) {}

    void ai_controller::play()
    {
        using namespace std::chrono_literals;

        while ( !should_close ) {
            if ( game.checkmate( true ) || game.checkmate( false ) ) {
                return;
            }

            if ( game.black_move() ) {
                std::this_thread::sleep_for( 1s );
                continue;
            }

            auto selected_move = select_best_move();

            select_space( selected_move.first.position() );
            if ( move( selected_move.second ) != pieces::move_status::valid ) {
                std::cout << "Error: AI Selected Invalid Move, AI Offline\n";
                return;
            }
        }
    }

    void ai_controller::activate()
    {
        should_close = false;
        runner       = std::thread( [this]() { play(); } );
    }

    ai_controller::~ai_controller()
    {
        should_close = true;
        if ( runner.joinable() ) {
            runner.join();
        }
    }
}  // namespace chess::controller