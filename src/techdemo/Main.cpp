#define GLFW_INCLUDE_NONE
#include "engine/WaterleafEngine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <optional>
#include <spdlog/spdlog.h>


std::optional<GLFWwindow *> InitializeWindow() {
   glfwInit(asdf);
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
   if(window == NULL) {
      spdlog::info("Failed to create GLFW window");
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
}

void TeardownWindow() {
   glfwTerminate();
}

bool InitializeOpenGl() {
   if(auto err = gladInit(); err != GLAD_OK) {
      spdlog::error("Failed to initialize GLAD");
      return false;
   }
   return true
}

void TeardownOpenGl() {}

void InitializeGui(const GLFWwindow *window) {
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init();
   ImGui::StyleColorsDark();
}

void RunGuiLoop(const GLFWwindow *window) {
   while(!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT);

      // feed inputs to dear imgui, start new frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // rendering our geometries
      // triangle_shader.use();
      // glBindVertexArray(vao);
      // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
      // glBindVertexArray(0);

      // render your GUI
      ImGui::Begin("Demo window");
      ImGui::Button("Hello!");
      ImGui::End();

      // Render dear imgui into screen
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glfwSwapBuffers(window);
   }
}

int main() {
   spdlog::warn("Hello world {:03f}", render::ExampleSin(3.14159f / 2));
   if(InitializeOpenGl()) {
      if(auto window_opt = InitializeWindow()) {
         InitializeGui(*window_opt);
         RunGuiLoop(*window_opt);
         TeardownWindow();
      }
      TeardownOpenGl();
   }
}