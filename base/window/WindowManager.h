#ifndef GL_HELLO_WORLD_WINDOWMANAGER_H
#define GL_HELLO_WORLD_WINDOWMANAGER_H


#include <vector>
#include <glad/glad.h>
#include "../struct/Extent2D.h"

class WindowManager {
public:
    virtual void init(int width, int height) = 0;

    virtual void destroy() = 0;

    virtual std::vector<const char *> getRequiredInstanceExtensions() = 0;

    virtual void setResizeCallback(void *application, void *callback) = 0;

    virtual void setKeyCallback(void *application, void *callback) = 0;

    virtual void setCursorPosCallback(void *application, void *callback) = 0;

    virtual void setMouseButtonCallback(void *application, void *callback) = 0;

    virtual void getCursorPos(double *xpos, double *ypos) = 0;

    virtual void pollEvents() = 0;

    virtual bool shouldClose() = 0;

    virtual void close() = 0;

    virtual Extent2D getWindowExtent() = 0;

    virtual void waitEvents() = 0;
};


#endif //GL_HELLO_WORLD_WINDOWMANAGER_H
