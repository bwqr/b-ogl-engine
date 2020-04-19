#include "ImGuiOverlay.h"

void ImGuiOverlay::init(const ImGuiOverlayInitInfo &initInfo) {
    if (initialized) {
        Logger::error("ImGuiOverlay is already initialized!");

        throw std::runtime_error("");
    }

    window = initInfo.window;


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void ImGuiOverlay::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiOverlay::draw() {


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiOverlay::drawModelsList(bool *p_open, const std::vector<std::string> &models,
                                  const std::function<void(size_t)> &callback) {
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Example: Simple layout", p_open, ImGuiWindowFlags_MenuBar)) {
        // left
        static int selected = 0;
        ImGui::BeginChild("Models", ImVec2(150, 0), true);
        for (size_t i = 0; i < models.size(); i++) {
            if (ImGui::Selectable(models[i].c_str(), selected == i)) {
                selected = i;
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();

        if (ImGui::Button("Click")) {
            callback(selected);
        }
    }
    ImGui::End();
}

void ImGuiOverlay::prepare() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}

void ImGuiOverlay::drawDemoWindow(bool *p_open) {
    ImGui::ShowDemoWindow(p_open);
}
