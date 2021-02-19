#include "texture.h"

GLuint loadTexture(const char* texImagePath)
{
    GLuint textureID = SOIL_load_OGL_texture(texImagePath,
                                             SOIL_LOAD_AUTO,
                                             SOIL_CREATE_NEW_ID,
                                             SOIL_FLAG_INVERT_Y);
    if (textureID == 0)
        printf("[GLError] could not find texture file\n");

    return textureID;
}
