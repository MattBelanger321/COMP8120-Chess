#ifndef __CHESS__IMGUI_INIT__INITIALIZER__HPP__
#define __CHESS__IMGUI_INIT__INITIALIZER__HPP__

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <functional>
#include <string>
#include <utility>  // for std::pair

namespace chess::display {

    class imgui_initializer {
    public:
        imgui_initializer( std::string window_name );
        ~imgui_initializer();

        bool init();
        void run( std::function< void() > render_function );
        void stop();

        void                  set_vsync( bool enabled );
        void                  set_window_title( const std::string & title );
        void                  set_clear_color( float r, float g, float b, float a );
        std::pair< int, int > get_window_size() const;
        bool                  is_window_open() const;

        static void error_callback( int error, const char * description );
        static void framebuffer_size_callback( GLFWwindow * window, int width, int height );

        GLFWwindow * window;
        const char * glsl_version;

    private:
        std::string window_name;
        bool        initialized    = false;
        bool        vsync_enabled  = true;
        int         window_width   = 1280;
        int         window_height  = 720;
        float       clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

        void main_loop( std::function< void() > render_function );
        void cleanup();
    };

}  // namespace chess::display

#endif  // __CHESS__IMGUI_INIT__INITIALIZER__HPP__
