#ifndef B_OGL_ENGINE_CORE_H
#define B_OGL_ENGINE_CORE_H

#include <chrono>
#include <iostream>
#include <filesystem>
#include "../gl/Shader.h"
#include "../window/glfw/GLFWWindowManager.h"
#include "../../objects/Camera.h"
#include "../../defs.h"
#include "../../objects/model/Model.h"
#include "../../objects/model/Cubemap.h"
#include "../struct/IntersectionRecord.h"
#include "../overlay/ImGuiOverlay.h"
#include "../../objects/model/Bezier.h"

struct CoreInitInfo {
    GLFWWindowManager windowManager;
    Shader diffuseShader;
    std::vector<Model> *diffuseModels;
    std::vector<Bezier> *bezierModels;
    Shader singleColorShader;
    Shader cubemapShader;
    std::array<std::string, 6> cubemapFaces;
};

class Core {
public:

    void init(const CoreInitInfo &initInfo);

    void mainLoop();

    void destroy();

private:
    struct WindowHandler {
        static void resizeCallback(GLFWwindow *window, int width, int height);

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    } windowHandler;

    struct CameraOptions {
        bool cameraXPlusMove = false;
        bool cameraXNegMove = false;
        bool cameraYPlusMove = false;
        bool cameraYNegMove = false;
    };

    struct SceneOptions {
        bool update = false;
    };

    struct OverlayOptions {
        bool drawModelsListWindow = false;
        bool drawDemoWindow = false;
        bool drawEditModelWindow = true;
        bool drawOptionsWindow = false;
    };

    struct {
        Shader shader;
        Model *model = nullptr;
        bool selected = false;
        float t;
        const glm::vec4 selectionColor = {0.2, 1, 0.2, 1.0};
        const glm::vec4 highlightColor = {1, 0.2, 0.2, 1.0};

        bool highlighted() { return model != nullptr; };

        void select(Model *_model) {
            model = _model;
            shader.setVec4("color", selectionColor);
            selected = true;
        }

        void clear() {
            model = nullptr;
            selected = false;
            t = FAR_VIEW;
            shader.setVec4("color", highlightColor);
        }
    } modelHighlight;

    struct Program {
        std::vector<Model> *models;
        Shader shader;
    };

    struct {
        CameraOptions camera;
        SceneOptions scene;
        OverlayOptions overlay;
    } options;

    bool initialized = false;

    GLFWWindowManager windowManager;

    Extent2D windowExtent;

    Camera camera;

    struct {
        double xpos;
        double ypos;
        double dx;
        double dy;
    } cursor;

    struct {
        std::chrono::high_resolution_clock::time_point prevTime;
        std::chrono::high_resolution_clock::time_point startTime;
        double timeDiff = 0;
        int fps = 0;
    } frameTiming;

    Program diffuseProgram;

    std::vector<Bezier> *bezierModels;

    Shader skyBoxShader;
    Cubemap skyBox;


    ImGuiOverlay overlay;
    std::vector<std::string> overlayModels;

    void findAllModelsInPath();

    void draw();


    void updateScene();

    void setupCamera();

    void checkSelection(Model &model, const Ray &ray);

    void moveSelection();

    void processKeyInputs();

    void calculateFrameTiming();

    void drawOptionsWindow(bool *p_open);
};


#endif //B_OGL_ENGINE_CORE_H
