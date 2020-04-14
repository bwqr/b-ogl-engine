#ifndef B_OGL_ENGINE_TEXTURE_H
#define B_OGL_ENGINE_TEXTURE_H

#include <string>

struct Texture {
    enum Type {
        DIFFUSE_TEXTURE,
        SPECULAR_TEXTURE,
        NORMAL_TEXTURE,
        HEIGHT_MAP_TEXTURE,
        BASE_COLOR,
    };

    uint id;
    Type type;
    std::string path;

    bool operator< (const Texture &t) const {
       return path < t.path;
    };
};
#endif //B_OGL_ENGINE_TEXTURE_H
