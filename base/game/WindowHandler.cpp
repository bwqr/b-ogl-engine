#include "Core.h"

void Core::WindowHandler::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    auto &cursor = app->cursor;

    cursor.dx = xpos - cursor.xpos;
    cursor.dy = ypos - cursor.ypos;

    cursor.xpos = xpos;
    cursor.ypos = ypos;

    if (app->options.scene.update) {
//        auto prevOrientedViewVector = app->camera.orientedViewPlaneVector;

        app->camera.rotate(cursor.dx / app->windowExtent.width, cursor.dy / app->windowExtent.height);

//        if (app->options.highlightModelSelect && app->highlightModel != nullptr) {
//            app->highlightModel->translate(app->camera.orientedViewPlaneVector - prevOrientedViewVector);
//        }
    }
}


void Core::WindowHandler::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!app->options.scene.update) {
            if (!app->modelHighlight.selected && app->modelHighlight.highlighted()) {
                app->singleColorShader.setVec4("color", app->modelHighlight.selectionColor);
                app->modelHighlight.selected = true;
            } else if (app->modelHighlight.selected) {
                Ray ray = app->camera.generateRay(app->cursor.xpos / app->windowExtent.width,
                                                  (app->windowExtent.height - app->cursor.ypos) /
                                                  app->windowExtent.height);

                IntersectionRecord record = {};

                if (app->modelHighlight.model->intersect(&record, ray, FAR_VIEW)) {
                    app->modelHighlight.selected = false;
                    app->modelHighlight.model = nullptr;
                }
            }
        }
    }
}


void Core::WindowHandler::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
        if (app->options.scene.update) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            app->options.scene.update = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            app->options.scene.update = true;
            app->modelHighlight.clear();
        }
    } else if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
        app->windowManager.close();
    } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        app->camera.toggleFreeLook();
    } else if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
        std::ofstream ostream(MODELS_INPUT_PATH);
        ostream << app->diffuseProgram.models->size() << std::endl;
        for (const auto &model: *app->diffuseProgram.models) {
            ostream << model.dump();
        }
    } else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
        if (app->modelHighlight.selected) {
            for (auto it = app->diffuseProgram.models->begin(); it != app->diffuseProgram.models->end(); it++) {
                if (it->id == app->modelHighlight.model->id) {
                    app->diffuseProgram.models->erase(it);
                    app->modelHighlight.clear();
                    break;
                }
            }
        }
    }
}


void Core::WindowHandler::resizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

    app->windowExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    app->camera.resizeCallback(app->windowExtent);

    glViewport(0, 0, width, height);
}
