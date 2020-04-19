#ifndef B_OGL_ENGINE_APPLICATION_H
#define B_OGL_ENGINE_APPLICATION_H


#include <fstream>
#include "base/game/Core.h"
#include "objects/model/Model.h"

class Application {
public:
    Application();
    ~Application();
    void start();
private:
    GLFWWindowManager windowManager;
    Core gameCore;

    std::vector<Model> models;

    void loadModels();
};


#endif //B_OGL_ENGINE_APPLICATION_H
