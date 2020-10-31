#ifndef GL_HELLO_WORLD_GLFWWINDOWMANAGER_H
#define GL_HELLO_WORLD_GLFWWINDOWMANAGER_H

#include "../WindowManager.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <stdexcept>
#include "../../game/Logger.h"

class GLFWWindowManager : public WindowManager {
public:
    GLFWwindow *window;

    void init(int width, int height) override;

    void destroy() override;

    inline bool shouldClose() override {
        return glfwWindowShouldClose(window);
    };

    inline void close() override {
        glfwSetWindowShouldClose(window, true);
    }

    inline void pollEvents() override {
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    void setResizeCallback(void *application, void *callback) override;

    void setKeyCallback(void *application, void *callback) override;

    void setCursorPosCallback(void *application, void *callback) override;

    void setMouseButtonCallback(void *application, void *callback) override;

    void getCursorPos(double *xpos, double *ypos) override;

    std::vector<const char *> getRequiredInstanceExtensions() override;

    Extent2D getWindowExtent() override;

    void waitEvents() override;

private:
    int width;
    int height;
};

#endif //GL_HELLO_WORLD_GLFWWINDOWMANAGER_H
