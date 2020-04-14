#ifndef B_OGL_ENGINE_SHADER_H
#define B_OGL_ENGINE_SHADER_H

#include <string>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../game/Logger.h"

class Shader {
public:
    uint id;

    Shader();

    explicit Shader(const std::string &vertPath, const std::string &fragPath);

    void useShader();

    void setInt(const std::string &var, const uint val) const;

    void setVec4(const std::string &var, const glm::vec4 val) const;

    void setMat4(const std::string &var, const glm::mat4 &mat4) const;

private:
    enum CheckTypes {
        CHECK_PROGRAM,
        CHECK_SHADER
    } checkType;

    static void checkCompileErrors(uint shader, CheckTypes type);
};


#endif //B_OGL_ENGINE_SHADER_H
