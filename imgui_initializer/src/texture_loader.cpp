
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <stdexcept>
#include <texture_loader.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

// Helper function to check OpenGL errors
void check_gl_error( const std::string & operation )
{
    GLenum error = glGetError();
    if ( error != GL_NO_ERROR ) {
        throw std::runtime_error( "OpenGL Error in " + operation + ": " + std::to_string( error ) );
    }
}

bool texture_loader::load_from_memory( std::vector< unsigned char > const & buffer, GLuint & out_texture,
                                       int & out_width, int & out_height )
{
    int image_width  = 0;
    int image_height = 0;
    int channels     = 0;

    // Smart pointer with custom deleter for image memory
    std::unique_ptr< unsigned char, decltype( &stbi_image_free ) > image_data(
        stbi_load_from_memory( reinterpret_cast< const unsigned char * >( buffer.data() ),
                               static_cast< int >( buffer.size() ), &image_width, &image_height, &channels,
                               4  // Force RGBA
                               ),
        stbi_image_free );

    if ( !image_data ) {
        std::string  error_msg  = "Failed to load image from memory: ";
        const char * stbi_error = stbi_failure_reason();
        if ( stbi_error ) {
            error_msg += stbi_error;
        }
        throw std::runtime_error( error_msg );
    }

    std::cout << "Image loaded: " << image_width << "x" << image_height << " (original channels: " << channels << ")"
              << std::endl;

    GLuint texture = 0;
    glGenTextures( 1, &texture );

    check_gl_error( "glGenTextures" );

    glBindTexture( GL_TEXTURE_2D, texture );
    check_gl_error( "glBindTexture" );

    // Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    check_gl_error( "glTexParameteri" );

    // Set pixel store parameters
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
    check_gl_error( "glPixelStorei" );

    // Upload texture data
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  image_data.get() );
    check_gl_error( "glTexImage2D" );

    // Unbind texture to avoid state pollution
    glBindTexture( GL_TEXTURE_2D, 0 );

    std::cout << "Texture created with ID: " << texture << std::endl;

    out_texture = texture;
    out_width   = image_width;
    out_height  = image_height;

    return true;
}

bool texture_loader::load_from_file( const std::string & file_path, GLuint & out_texture, int & out_width,
                                     int & out_height )
{
    std::cout << "Loading " << file_path << std::endl;

    std::ifstream file( file_path, std::ios::binary | std::ios::ate );
    if ( !file ) {
        throw std::runtime_error( "Failed to open file: " + file_path );
    }

    std::streamsize size = file.tellg();
    file.seekg( 0, std::ios::beg );

    if ( size <= 0 ) {
        throw std::runtime_error( "File is empty or error reading: " + file_path );
    }

    std::vector< unsigned char > buffer( static_cast< size_t >( size ) );
    if ( !file.read( reinterpret_cast< char * >( buffer.data() ), size ) ) {
        throw std::runtime_error( "Failed to read file: " + file_path );
    }

    std::cout << "File read successfully, size: " << size << " bytes" << std::endl;

    return load_from_memory( buffer, out_texture, out_width, out_height );
}