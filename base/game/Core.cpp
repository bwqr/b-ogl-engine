#include "Core.h"

void Core::init(const CoreInitInfo &initInfo) {
    if (initialized) {
        Logger::error("core is already initialized", Logger::CORE_INIT_FAILED);

        throw std::runtime_error("");
    }

    initialized = true;

    windowManager.init(WIDTH, HEIGHT);

    windowManager.setResizeCallback(this, (void *) resizeCallback);
    windowManager.setKeyCallback(this, (void *) keyCallback);
    windowManager.getCursorPos(&cursor.xpos, &cursor.ypos);
    windowManager.setCursorPosCallback(this, (void *) cursorPosCallback);

    setupCamera();

    simpleSamplerProgram.shader = Shader(initInfo.simpleSamplerVertPath, initInfo.simpleSamplerFragPath);
    simpleSamplerProgram.models = initInfo.simpleSamplerModels;

    skyBoxShader = Shader(initInfo.cubemapVertPath, initInfo.cubemapFragPath);
    skyBox = Cubemap(initInfo.cubemapFaces);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
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
    glClearColor(0.3f, 0.7f, 0.9f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleSamplerProgram.shader.useShader();
    simpleSamplerProgram.shader.setMat4("camera.view", camera.view);
    simpleSamplerProgram.shader.setMat4("camera.proj", camera.proj);

    if (simpleSamplerProgram.models != nullptr) {
        for (const auto &model: *simpleSamplerProgram.models) {
            model.draw(simpleSamplerProgram.shader);
        }
    }

//    // change depth function so depth test passes when values are equal to depth buffer's content
//    glDepthFunc(GL_LEQUAL);
//    skyBoxShader.useShader();
//    // remove translation from the view matrix
//    skyBoxShader.setMat4("camera.view", glm::mat4(glm::mat3(camera.view)));
//    skyBoxShader.setMat4("camera.proj", camera.proj);
//    skyBox.draw(skyBoxShader);
//    glDepthFunc(GL_LESS);

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
