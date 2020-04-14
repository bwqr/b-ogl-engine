#include "Core.h"

void Core::init(const CoreInitInfo &initInfo) {
    if (initialized) {
        Logger::error("core is already initialized", Logger::CORE_INIT_FAILED);

        throw std::runtime_error("");
    }

    initialized = true;

    windowManager = initInfo.windowManager;

    windowManager.setResizeCallback(this, (void *) resizeCallback);
    windowManager.setKeyCallback(this, (void *) keyCallback);
    windowManager.getCursorPos(&cursor.xpos, &cursor.ypos);
    windowManager.setCursorPosCallback(this, (void *) cursorPosCallback);

    setupCamera();

    diffuseProgram.shader = initInfo.diffuseShader;
    diffuseProgram.models = initInfo.diffuseModels;

    singleColorShader = initInfo.singleColorShader;

    skyBoxShader = initInfo.cubemapShader;
    skyBox = Cubemap(initInfo.cubemapFaces);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glClearColor(0.3f, 0.7f, 0.9f, 1.0f);
    glClearDepth(1.0);
    glClearStencil(0);
}

void Core::mainLoop() {
    while (!windowManager.shouldClose()) {
        windowManager.pollEvents();

        static int fps = 0;

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();

        if (std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() > 1) {
            std::cout << fps << std::endl;
            startTime = currentTime;
            fps = 0;
        }

        fps++;

        updateCamera();

        draw();

        frameTimeDifference = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - lastFrameTime).count();

        lastFrameTime = currentTime;
    }
}

void Core::draw() {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilMask(0x00);

    diffuseProgram.shader.useShader();
    diffuseProgram.shader.setMat4("camera.view", camera.view);
    diffuseProgram.shader.setMat4("camera.proj", camera.proj);

    selectedModelZ = FAR_VIEW;
    selectedModel = nullptr;

    if (diffuseProgram.models != nullptr) {
        for (auto &model: *diffuseProgram.models) {
            checkSelection(model, selectedModelZ);
        }

        for (auto &model: *diffuseProgram.models) {
            if(selectedModel == &model) {
                glStencilMask(0xFF);

                model.draw(diffuseProgram.shader);

                glStencilMask(0x00);
            } else {
                model.draw(diffuseProgram.shader);
            }
        }
    }

    if (selectedModel != nullptr) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        singleColorShader.useShader();
        singleColorShader.setMat4("camera.view", camera.view);
        singleColorShader.setMat4("camera.proj", camera.proj);

        selectedModel->drawSelection(singleColorShader);
    }
}

void Core::destroy() {
    windowManager.destroy();
}

void Core::resizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    app->windowExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    app->camera.resizeCallback(app->windowExtent);

    glViewport(0, 0, width, height);
}

void Core::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app->windowManager.close();
    } else if (key == GLFW_KEY_D) {
        app->cameraXPlusMove = (action & (GLFW_PRESS | GLFW_REPEAT)) != 0;
    } else if (key == GLFW_KEY_A) {
        app->cameraXNegMove = (action & (GLFW_PRESS | GLFW_REPEAT)) != 0;
    } else if (key == GLFW_KEY_W) {
        app->cameraYPlusMove = (action & (GLFW_PRESS | GLFW_REPEAT)) != 0;
    } else if (key == GLFW_KEY_S) {
        app->cameraYNegMove = (action & (GLFW_PRESS | GLFW_REPEAT)) != 0;
    } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        app->camera.toggleFreeLook();
    }
}

void Core::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    auto &cursor = app->cursor;

    cursor.dx = xpos - cursor.xpos;
    cursor.dy = ypos - cursor.ypos;

    cursor.xpos = xpos;
    cursor.ypos = ypos;

    app->camera.rotate(cursor.dx / app->windowExtent.width, cursor.dy / app->windowExtent.height);
}

void Core::setupCamera() {
    camera = Camera({0, 5, 10.0}, {0, -5, -10});
    camera.rotate(0, 0);
}

void Core::updateCamera() {
    float tx = 0, ty = 0;
    float k = 2. * frameTimeDifference;
    if (cameraXPlusMove) {
        tx = k;
    } else if (cameraXNegMove) {
        tx = -k;
    }

    if (cameraYPlusMove) {
        ty = k;
    } else if (cameraYNegMove) {
        ty = -k;
    }

    camera.translate(tx, ty);
}

void Core::checkSelection(Model &model, const float &tBest) {
    if (selectedModel == nullptr) {
        selectedModel = &model;
    }
}
