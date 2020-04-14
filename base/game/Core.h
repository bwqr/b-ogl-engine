#ifndef B_OGL_ENGINE_CORE_H
#define B_OGL_ENGINE_CORE_H

#include <chrono>
#include <iostream>
#include "../gl/Shader.h"
#include "../window/glfw/GLFWWindowManager.h"
#include "../../objects/Camera.h"
#include "../../defs.h"
#include "../../objects/model/Model.h"
#include "../../objects/model/Cubemap.h"

struct CoreInitInfo {
    std::string simpleSamplerVertPath;
    std::string simpleSamplerFragPath;
    std::string cubemapVertPath;
    std::string cubemapFragPath;
    std::array<std::string, 6> cubemapFaces;
    std::vector<Model> *simpleSamplerModels;
};

class Core {
public:
    double frameTimeDifference = 0;

    void init(const CoreInitInfo &initInfo);

    void mainLoop();

    void destroy();

private:
    struct Program {
        std::vector<Model> *models;
        Shader shader;
    };

    bool initialized = false;

    GLFWWindowManager windowManager;

    Extent2D windowExtent;

    Camera camera;
    bool cameraXPlusMove = false;
    bool cameraXNegMove = false;
    bool cameraYPlusMove = false;
    bool cameraYNegMove = false;

    struct {
        double xpos;
        double ypos;
        double dx;
        double dy;
    } cursor;

    std::chrono::high_resolution_clock::time_point lastFrameTime;

    Program simpleSamplerProgram;

    Shader skyBoxShader;
    Cubemap skyBox;

    void draw();

    static void resizeCallback(GLFWwindow *window, int width, int height);

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    void updateCamera();

    void setupCamera();
};


#endif //B_OGL_ENGINE_CORE_H
