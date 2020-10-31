#ifndef B_OGL_ENGINE_IMGUIOVERLAY_H
#define B_OGL_ENGINE_IMGUIOVERLAY_H


#include <vector>
#include <functional>
#include "../../objects/model/Model.h"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include "../game/Logger.h"

struct ImGuiOverlayInitInfo {
    GLFWwindow *window;

};

class ImGuiOverlay {
public:
    void init(const ImGuiOverlayInitInfo &initInfo);

    void
    drawModelsList(bool *p_open, const std::vector<std::string> &models, const std::function<void(size_t)> &callback);

    void drawDemoWindow(bool *p_open);

    void destroy();

    void prepare();

    void draw();

private:
    GLFWwindow *window;

    bool initialized = false;

    ImGuiIO *io;
};


#endif //B_OGL_ENGINE_IMGUIOVERLAY_H
