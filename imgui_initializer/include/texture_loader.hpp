#ifndef __CHESS__IMGUI_INIT__TEXTURE_LOADER__HPP__
#define __CHESS__IMGUI_INIT__TEXTURE_LOADER__HPP__

#include <GL/gl.h>  // or <GL/glew.h> / <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <vector>

class texture_loader {
public:
    // Load texture from raw memory buffer
    static bool load_from_memory( std::vector< unsigned char > const & buffer, GLuint & out_texture, int & out_width,
                                  int & out_height );

    // Load texture directly from a file
    static bool load_from_file( const std::string & file_path, GLuint & out_texture, int & out_width,
                                int & out_height );
};

#endif