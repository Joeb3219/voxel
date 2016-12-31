#include <iostream>
#include <SFML/OpenGL.hpp>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "fileIO.h"

/**
 *  Converts a bitmap file into a Texture.
 *  The Bitmap must be wellformed, with the following criteria:
 *  0x0: 'B', 0x1, 'M'
 *  0x0A: position of where image data begins.
 *  0x12: width of image, 0x16: height of image.
 *  Will return 0 if any errors occur.
 */
GLuint VOX_FileIO::loadBitmapTexture(const char *fileName){
    GLuint texture;
    int width = 0, height = 0, dataPos = 0;     // dataPos is used to track where data begins, width/height are width/height in header image.
    unsigned char * data;                       // Each segment of data is a single byte.
    FILE * file = fopen(fileName, "r");

    if ( file == NULL ){
        std::cout << "Attempting to load texture: " << fileName << ", but it does not exist." << std::endl;
        return 0;
    }

    // Load the image's header to get data we need about the image.
    data = new unsigned char[54];               // malloc(54 * sizeof(unsigned char));  // We malloc 54 bytes of memory so that we can read the header.
    fread(data, 1, 54, file);                   // Read the first 54 bytes of the bitmap.
    if(data[0] != 'B' && data[1] != 'M'){
        std::cout << "Attempting to load texture: " << fileName << ", but it has an invalid header." << std::endl;
        return 0;
    }

    // The following three calls assume we're working with 32-bit+ integers.
    // Starting at their indication positions, will convert 4 bytes into an integer.
    // TODO: Make this safe for systems that don't use 32-bit+ integers.
    dataPos = *(int*)&(data[0x0A]);
    width = *(int*)&(data[0x12]);
    height = *(int*)&(data[0x16]);

    delete [] data;

    // Load the image body at the dataPos position (and onward) to get actual color data.
    fseek(file, dataPos, SEEK_SET);         // Jump to position of color data.

    std::cout << "Image: " << fileName << ", Height: " << height << ", width: " << width << ", DataPos: " << dataPos << std::endl;

    data = new unsigned char[width * height * 3];
    fread(data, 1, width * height * 3, file);
    fclose( file );                       // We no longer need the file, close it.

    // Now that we have an array of colors, have OpenGL map them to a texture.
    glGenTextures( 1, &texture );
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    delete [] data;

    return texture;
}
