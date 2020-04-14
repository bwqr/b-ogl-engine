#include "Shader.h"

Shader::Shader() {}

Shader::Shader(const std::string &vertPath, const std::string &fragPath) {
    std::ifstream vertStream(vertPath, std::ios::binary | std::ios::ate);
    std::ifstream fragStream(fragPath, std::ios::binary | std::ios::ate);

    std::string vertBuff;
    std::string fragBuff;

    if (!vertStream.is_open() || !fragStream.is_open()) {
        Logger::error("phase 1", Logger::SHADER_LOAD_FAILED);

        throw std::runtime_error("");
    }

    vertBuff.resize(vertStream.tellg());
    fragBuff.resize(fragStream.tellg());

    vertStream.seekg(0);
    fragStream.seekg(0);
    vertStream.read(vertBuff.data(), vertBuff.size());
    fragStream.read(fragBuff.data(), fragBuff.size());

    if (!vertStream.good() || !fragStream.good()) {
        Logger::error("phase 2", Logger::SHADER_LOAD_FAILED);

        throw std::runtime_error("");
    }

    uint vertShader = glCreateShader(GL_VERTEX_SHADER);
    auto vertCode = vertBuff.c_str();
    glShaderSource(vertShader, 1, &vertCode, nullptr);
    glCompileShader(vertShader);
    checkCompileErrors(vertShader, CHECK_SHADER);

    uint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragCode = fragBuff.c_str();
    glShaderSource(fragShader, 1, &fragCode, nullptr);
    glCompileShader(fragShader);
    checkCompileErrors(fragShader, CHECK_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertShader);
    glAttachShader(id, fragShader);

    glLinkProgram(id);
    checkCompileErrors(id, CHECK_PROGRAM);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Shader::checkCompileErrors(uint shader, CheckTypes type) {
    GLint success;
    GLchar log[1024];

    if (type == CHECK_SHADER) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, log);
            Logger::error(std::string(log), Logger::SHADER_LOAD_FAILED);
        }
    } else if (type == CHECK_PROGRAM) {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (success != GL_TRUE) {
            glGetProgramInfoLog(shader, 1024, nullptr, log);
            Logger::error(std::string(log), Logger::SHADER_LOAD_FAILED);
        }
    }
}

void Shader::useShader() {
    glUseProgram(id);
}

void Shader::setMat4(const std::string &var, const glm::mat4 &mat4) const {
    auto location = glGetUniformLocation(id, var.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat4[0][0]);
}

void Shader::setInt(const std::string &var, const uint val) const {
    auto location = glGetUniformLocation(id, var.c_str());
    glUniform1i(location, val);
}

void Shader::setVec4(const std::string &var, const glm::vec4 val) const {
    auto location = glGetUniformLocation(id, var.c_str());
    glUniform4f(location, val.x, val.y, val.z, val.w);
}
