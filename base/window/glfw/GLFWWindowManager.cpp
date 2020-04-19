#include "GLFWWindowManager.h"

void GLFWWindowManager::init(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "bwqr", nullptr, nullptr);

    if(window == nullptr) {
        throw std::runtime_error("unable to create window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::error("Failed to initialize GLAD");
    }
}

void GLFWWindowManager::setResizeCallback(void *application, void *callback) {
    glfwSetWindowUserPointer(window, application);

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun) callback);
}

std::vector<const char *> GLFWWindowManager::getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwRequiredInstanceExtensions;

    glfwRequiredInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwRequiredInstanceExtensions,
                                         glfwRequiredInstanceExtensions + glfwExtensionCount);

    return extensions;
}

Extent2D GLFWWindowManager::getWindowExtent() {
    int width, height;

    glfwGetWindowSize(window, &width, &height);

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void GLFWWindowManager::waitEvents() {
    glfwWaitEvents();
}

void GLFWWindowManager::setKeyCallback(void *application, void *callback) {
    glfwSetWindowUserPointer(window, application);

    glfwSetKeyCallback(window, (GLFWkeyfun) callback);
}

void GLFWWindowManager::setCursorPosCallback(void *application, void *callback) {
    glfwSetWindowUserPointer(window, application);

    glfwSetCursorPosCallback(window, (GLFWcursorposfun) callback);
}

void GLFWWindowManager::getCursorPos(double *xpos, double *ypos) {
    glfwGetCursorPos(window, xpos, ypos);
}

void GLFWWindowManager::destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWWindowManager::setMouseButtonCallback(void *application, void *callback) {
    glfwSetWindowUserPointer(window, application);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun) callback);
}
