#include <imgui_initializer.hpp>
#include <string>
#include <texture_loader.hpp>

#include <iostream>
#include <stdexcept>

namespace chess::display {

    imgui_initializer::imgui_initializer( std::string window_name, int width, int height ) :
        window( nullptr ),
        glsl_version( "#version 130" ),
        window_name( std::move( window_name ) ),
        initialized( false ),
        window_width( width ),
        window_height( height )
    {
    }

    bool imgui_initializer::init()
    {
        try {
            // Setup window
            glfwSetErrorCallback( error_callback );
            if ( !glfwInit() ) {
                throw std::runtime_error( "Failed to initialize GLFW" );
            }

            glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
            glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
            glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
#if __APPLE__
            glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );  // Required on Mac
#endif

            // Create window with graphics context
            window = glfwCreateWindow( window_width, window_height, window_name.c_str(), nullptr, nullptr );
            if ( window == nullptr ) {
                glfwTerminate();
                throw std::runtime_error( "Failed to create GLFW window" );
            }

            glfwMakeContextCurrent( window );
            glfwSwapInterval( vsync_enabled ? 1 : 0 );

            // Set up window resize callback
            glfwSetWindowUserPointer( window, this );
            glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            if ( !ImGui::GetCurrentContext() ) {
                throw std::runtime_error( "Failed to create ImGui context" );
            }

            ImGuiIO & io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer bindings
            if ( !ImGui_ImplGlfw_InitForOpenGL( window, true ) ) {
                throw std::runtime_error( "Failed to initialize ImGui GLFW implementation" );
            }

            if ( !ImGui_ImplOpenGL3_Init( glsl_version ) ) {
                throw std::runtime_error( "Failed to initialize ImGui OpenGL3 implementation" );
            }

            initialized = true;
            return true;
        }
        catch ( const std::exception & e ) {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            cleanup();
            return false;
        }
    }

    void imgui_initializer::cleanup()
    {
        // Check if ImGui context exists before destroying it
        if ( ImGui::GetCurrentContext() ) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        // Safely destroy GLFW window
        if ( window ) {
            glfwDestroyWindow( window );
            window = nullptr;
        }

        // Terminate GLFW
        if ( initialized ) {
            glfwTerminate();
            initialized = false;
        }
    }

    void imgui_initializer::stop() { cleanup(); }

    void imgui_initializer::main_loop( std::function< void() > render_function )
    {
        if ( !render_function ) {
            std::cerr << "Error: Render function is null!" << std::endl;
            return;
        }

        if ( !window || !initialized ) {
            std::cerr << "Error: ImGui not properly initialized!" << std::endl;
            return;
        }

        while ( !glfwWindowShouldClose( window ) ) {
            try {
                // Poll and handle events (inputs, window resize, etc.)
                glfwPollEvents();

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                // Execute user's render function
                render_function();

                // Rendering
                ImGui::Render();

                GLenum error = glGetError();
                if ( error != GL_NO_ERROR ) {
                    std::cout << "OpenGL Error: " << error << std::endl;
                }

                int display_w, display_h;
                glfwGetFramebufferSize( window, &display_w, &display_h );
                glViewport( 0, 0, display_w, display_h );
                glClearColor( 0.45f, 0.55f, 0.60f, 1.00f );
                glClear( GL_COLOR_BUFFER_BIT );
                ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

                glfwSwapBuffers( window );
            }
            catch ( const std::exception & e ) {
                std::cerr << "Render loop error: " << e.what() << std::endl;
                break;
            }
        }
    }

    void imgui_initializer::error_callback( int error, const char * description )
    {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

    void imgui_initializer::framebuffer_size_callback( GLFWwindow * window, int width, int height )
    {
        // Update viewport when window is resized
        glViewport( 0, 0, width, height );

        // Get the imgui_initializer instance and update its stored dimensions
        imgui_initializer * instance = static_cast< imgui_initializer * >( glfwGetWindowUserPointer( window ) );
        if ( instance ) {
            instance->window_width  = width;
            instance->window_height = height;
        }
    }

    void imgui_initializer::run( std::function< void() > render_function )
    {
        if ( !render_function ) {
            std::cerr << "Error: Cannot run with null render function!" << std::endl;
            return;
        }

        if ( !init() ) {
            std::cerr << "Failed to initialize ImGui!" << std::endl;
            return;
        }

        try {
            main_loop( render_function );
        }
        catch ( const std::exception & e ) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }

        cleanup();
    }

    // Utility methods for runtime configuration
    void imgui_initializer::set_vsync( bool enabled )
    {
        if ( window && initialized ) {
            vsync_enabled = enabled;
            glfwSwapInterval( vsync_enabled ? 1 : 0 );
        }
    }

    void imgui_initializer::set_window_title( const std::string & title )
    {
        if ( window && initialized ) {
            window_name = title;
            glfwSetWindowTitle( window, window_name.c_str() );
        }
    }

    void imgui_initializer::set_clear_color( float r, float g, float b, float a )
    {
        clear_color[0] = r;
        clear_color[1] = g;
        clear_color[2] = b;
        clear_color[3] = a;
    }

    std::pair< int, int > imgui_initializer::get_window_size() const { return { window_width, window_height }; }

    bool imgui_initializer::is_window_open() const { return window && !glfwWindowShouldClose( window ); }

    imgui_initializer::~imgui_initializer() { cleanup(); }

}  // namespace chess::display