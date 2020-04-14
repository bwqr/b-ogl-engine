#ifndef B_OGL_ENGINE_GLHELPER_H
#define B_OGL_ENGINE_GLHELPER_H


#include <glad/glad.h>
#include <iostream>

#define GL_CHECK_RESULT(f)                                                                                                             \
{                                                                                                                                        \
    int res = (f);                                                                                                                       \
    if(res != GLEW_OK)                                                                                                                   \
    {                                                                                                                                    \
        std::cerr << "Fatal : VkResult is \"" << glh::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl;  \
        throw std::runtime_error("");                                                                                                    \
    }                                                                                                                                    \
}                                                                                                                                        \

namespace glh {
    static std::string errorString(int errorCode) {
        switch (errorCode) {
//#define STR(r) case GLAD_ ##r: return #r
//            STR(ERROR_NO_GL_VERSION);
//#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }
}

#endif //B_OGL_ENGINE_GLHELPER_H
