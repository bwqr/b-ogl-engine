#include "Core.h"


void Core::init(const CoreInitInfo &initInfo) {
    if (initialized) {
        Logger::error("core is already initialized", Logger::CORE_INIT_FAILED);

        throw std::runtime_error("");
    }

    initialized = true;

    //WindowManager configuration
    windowManager = initInfo.windowManager;

    windowManager.setResizeCallback(this, (void *) WindowHandler::resizeCallback);
    windowManager.setKeyCallback(this, (void *) WindowHandler::keyCallback);
    windowManager.getCursorPos(&cursor.xpos, &cursor.ypos);
    windowManager.setCursorPosCallback(this, (void *) WindowHandler::cursorPosCallback);
    windowManager.setMouseButtonCallback(this, (void *) WindowHandler::mouseButtonCallback);

    // Camera initialization
    setupCamera();

    //Programs configuration
    diffuseProgram.shader = initInfo.diffuseShader;
    diffuseProgram.models = initInfo.diffuseModels;

    modelHighlight.shader = initInfo.singleColorShader;
    modelHighlight.shader.useShader();
    modelHighlight.clear();

    skyBoxShader = initInfo.cubemapShader;
    skyBox = Cubemap(initInfo.cubemapFaces);

    //GL configuration
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

    //FrameTiming initialization
    frameTiming.startTime = std::chrono::high_resolution_clock::now();

    //Overlay Initialization
    ImGuiOverlayInitInfo overlayInitInfo = {};
    overlayInitInfo.window = windowManager.window;
    overlay.init(overlayInitInfo);
    findAllModelsInPath();
}

void Core::mainLoop() {
    while (!windowManager.shouldClose()) {
        processKeyInputs();

        calculateFrameTiming();

        if (options.scene.update) {
            updateScene();
        }

        draw();

        windowManager.pollEvents();
    }
}

void Core::draw() {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilMask(0x00);

    diffuseProgram.shader.useShader();
    diffuseProgram.shader.setMat4("camera", camera.getViewProjectionMatrix());

    if (diffuseProgram.models != nullptr) {
        if (!modelHighlight.selected) {
            modelHighlight.clear();

            Ray ray = camera.generateRay(cursor.xpos / windowExtent.width,
                                         (windowExtent.height - cursor.ypos) / windowExtent.height);
            for (auto &model: *diffuseProgram.models) {
                checkSelection(model, ray);
            }
        }

        for (auto &model: *diffuseProgram.models) {
            if (modelHighlight.model == &model) {
//                glStencilMask(0xFF);

                model.draw(diffuseProgram.shader);

//                glStencilMask(0x00);
            } else {
                model.draw(diffuseProgram.shader);
            }
        }
    }

    if (modelHighlight.highlighted()) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        modelHighlight.shader.useShader();
        modelHighlight.shader.setMat4("camera", camera.getViewProjectionMatrix());

        modelHighlight.model->drawCollisionBox(modelHighlight.shader);
    }

    //Overlay
    overlay.prepare();

    if (options.overlay.drawModelsListWindow) {
        overlay.drawModelsList(&options.overlay.drawModelsListWindow, overlayModels, [=](size_t selected) {
            diffuseProgram.models->emplace_back();
            auto &model = (*diffuseProgram.models)[diffuseProgram.models->size() - 1];
            Model::ModelHandler::loadFromPath(&model, overlayModels[selected]);
            model.rotate(0, 0, 0);
        });
    }

    if (modelHighlight.selected && options.overlay.drawEditModelWindow) {
        modelHighlight.model->createEditOverlay(&options.overlay.drawEditModelWindow);
    }

    if (options.overlay.drawDemoWindow) {
        overlay.drawDemoWindow(&options.overlay.drawDemoWindow);
    }

    if (options.overlay.drawOptionsWindow) {
        drawOptionsWindow(&options.overlay.drawOptionsWindow);
    }

    overlay.draw();
}

void Core::destroy() {
    overlay.destroy();
}

void Core::setupCamera() {
    camera = Camera({0, 0, 10.0}, {0, 0, -10});
    camera.rotate(0, 0);
}

void Core::updateScene() {
    float tx = 0, ty = 0;
    float k = 2.f * frameTiming.timeDiff;
    if (options.camera.cameraXPlusMove) {
        tx = k;
    } else if (options.camera.cameraXNegMove) {
        tx = -k;
    }

    if (options.camera.cameraYPlusMove) {
        ty = k;
    } else if (options.camera.cameraYNegMove) {
        ty = -k;
    }

    camera.translate(tx, ty);

    if (modelHighlight.selected) {
        modelHighlight.model->translate(camera.getTranslateDirection(tx, ty));
    }
}

void Core::checkSelection(Model &model, const Ray &ray) {
    IntersectionRecord record = {};

    if (model.intersect(&record, ray, modelHighlight.t)) {
        modelHighlight.model = &model;
        modelHighlight.t = record.t;
    }
}

void Core::processKeyInputs() {

    options.camera.cameraXPlusMove = glfwGetKey(windowManager.window, GLFW_KEY_D) == GLFW_PRESS;
    options.camera.cameraXNegMove = glfwGetKey(windowManager.window, GLFW_KEY_A) == GLFW_PRESS;
    options.camera.cameraYPlusMove = glfwGetKey(windowManager.window, GLFW_KEY_W) == GLFW_PRESS;
    options.camera.cameraYNegMove = glfwGetKey(windowManager.window, GLFW_KEY_S) == GLFW_PRESS;
}

void Core::findAllModelsInPath() {
    for (const auto &entry: std::filesystem::directory_iterator(MODELS_DIR)) {
        if (entry.is_directory()) {
            overlayModels.push_back(entry.path().filename());
        }
    }
}

void Core::calculateFrameTiming() {
    auto currentTime = std::chrono::high_resolution_clock::now();

    if (std::chrono::duration<float, std::chrono::seconds::period>
                (currentTime - frameTiming.startTime).count() > 1) {

        Logger::info(std::string("fps: ") + std::to_string(frameTiming.fps));
        frameTiming.startTime = currentTime;
        frameTiming.fps = 0;
    }

    frameTiming.fps++;

    frameTiming.timeDiff = std::chrono::duration<float, std::chrono::seconds::period>(
            currentTime - frameTiming.prevTime).count();

    frameTiming.prevTime = currentTime;
}

void Core::drawOptionsWindow(bool *p_open) {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Options", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::BeginChild("Scene", ImVec2(300, 100), true);
        ImGui::Checkbox("Scene Update", &options.scene.update);
        ImGui::EndChild();

        ImGui::BeginChild("Overlay", ImVec2(300, 100), true);
        ImGui::Checkbox("Draw Models List", &options.overlay.drawModelsListWindow);
        ImGui::Checkbox("Draw Demo", &options.overlay.drawDemoWindow);
        ImGui::Checkbox("Draw Options", &options.overlay.drawOptionsWindow);
        ImGui::Checkbox("Draw Edit Model", &options.overlay.drawEditModelWindow);
        ImGui::EndChild();
    }

    ImGui::End();
}
