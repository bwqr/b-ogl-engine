#include "Application.h"
#include "defs.h"


Application::Application() {
    windowManager.init(WIDTH, HEIGHT);

    CoreInitInfo coreInitInfo = {};
    coreInitInfo.windowManager = windowManager;

    coreInitInfo.diffuseShader = Shader(SHADERS_DIR + "simpleDiffuse.vert", SHADERS_DIR + "simpleDiffuse.frag");
    coreInitInfo.diffuseModels = &models;

    coreInitInfo.bezierModels = &bezierModels;

    coreInitInfo.cubemapShader = Shader(SHADERS_DIR + "scene.vert", SHADERS_DIR + "scene.frag");

    coreInitInfo.singleColorShader = Shader(SHADERS_DIR + "debug.vert", SHADERS_DIR + "debug.frag");

    coreInitInfo.cubemapFaces = {
            std::string(TEXTURES_DIR) + "skybox/right.jpg",
            std::string(TEXTURES_DIR) + "skybox/left.jpg",
            std::string(TEXTURES_DIR) + "skybox/top.jpg",
            std::string(TEXTURES_DIR) + "skybox/bottom.jpg",
            std::string(TEXTURES_DIR) + "skybox/front.jpg",
            std::string(TEXTURES_DIR) + "skybox/back.jpg"
    };

    gameCore.init(coreInitInfo);

    loadModels();

    loadBezierModels();
}

Application::~Application() {
    gameCore.destroy();
    windowManager.destroy();
}

void Application::start() {
    gameCore.mainLoop();
}

void Application::loadModels() {
    std::fstream istream(MODELS_INPUT_PATH);

    if (!istream.is_open()) {
        Logger::error("model input file cannot be open", Logger::MODEL_LOAD_FAILED);
        return;
    }

    std::string name;
    float x, y, z, sx, sy, sz, yaw, pitch, roll;
    int num;
    istream >> num;
    for (size_t i = 0; i < num; i++) {

        istream >> name >> x >> y >> z >> sx >> sy >> sz >> roll >> yaw >> pitch;

        models.emplace_back();
        auto &model = models[models.size() - 1];
        Model::ModelHandler::loadFromPath(&model, name);

        model.translate({x, y, z});
        model.scale({sx, sy, sz});
        model.rotate(roll, yaw, pitch);
    }
}

void Application::loadBezierModels() {
    // Just put one model for teapot
    bezierModels.emplace_back();
    auto &model = bezierModels[bezierModels.size() - 1];
    // Default Resolution is set to 4x4
    model.setResolution({10, 10});
    model.build();

    model.translate({0, 0, 0});
    model.scale({.2, .2, .2});
    model.rotate(-90, 0, 0);
}

