#include "Application.h"
#include "defs.h"


Application::Application() {
    GLFWWindowManager windowManager;
    windowManager.init(WIDTH, HEIGHT);

    CoreInitInfo coreInitInfo = {};
    coreInitInfo.windowManager = windowManager;

    coreInitInfo.diffuseShader = Shader(SHADERS_DIR + "simpleDiffuse.vert", SHADERS_DIR + "simpleDiffuse.frag");
    coreInitInfo.diffuseModels = &models;

    coreInitInfo.cubemapShader = Shader(SHADERS_DIR + "scene.vert", SHADERS_DIR + "scene.frag");

    coreInitInfo.singleColorShader = Shader(SHADERS_DIR + "singleColor.vert", SHADERS_DIR + "singleColor.frag");

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
}

Application::~Application() {
    gameCore.destroy();
}

void Application::start() {
    gameCore.mainLoop();
}

void Application::loadModels() {
    std::fstream istream(MODELS_INPUT_PATH);

    std::string name;
    float x, y, z, sx, sy, sz;
    while (!istream.eof()) {

        istream >> name >> x >> y >> z >> sx >> sy >> sz;

        models.emplace_back();
        auto &model = models[models.size() - 1];

        Model::createFromPath(&model, name);

        model.position = {x, y, z};
        model.scale = {sx, sy, sz};
    }
}

