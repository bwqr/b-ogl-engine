#ifndef B_OGL_ENGINE_LOGGER_H
#define B_OGL_ENGINE_LOGGER_H

#include <iostream>


namespace Logger {
    enum Error {
        MODEL_LOAD_FAILED,
        SHADER_LOAD_FAILED,
        CORE_INIT_FAILED,
        TEXTURE_LOAD_ERROR,
        UNKNOWN_ERROR
    };


    inline static void error(const std::string &s, const Error &error = UNKNOWN_ERROR) {
        std::cerr << "Error " << error << ", message: " << s << std::endl;
    }

    inline static void info(const std::string &s) {
        std::cout << "Info, message: " << s << std::endl;
    }

    inline static void info(const int &i) {
        std::cout << "Info, message: " << i << std::endl;
    }

}

#endif //B_OGL_ENGINE_LOGGER_H
