#include "../inc/first_app.hpp"

#include "../inc/lve_camera.hpp"
#include "../inc/keyboard_movement_controller.hpp"
#include "../inc/simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <iostream>

namespace lve {

  /*
   * Constructor
   */
  FirstApp::FirstApp() {
    loadGameObjects();
  }

  FirstApp::~FirstApp() {}

  /*
   * run
   */
  void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
    LveCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = LveGameObject::createGameObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!lveWindow.shouldClose()) {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      currentTime = newTime;
      cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
      std::cout << frameTime << "\n";
      float aspect = lveRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

      if (auto commandBuffer = lveRenderer.beginFrame()) {
        lveRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
        lveRenderer.endSwapChainRenderPass(commandBuffer);
        lveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(lveDevice.device());
  }

  /*
   * loadGameObjects
   */
  void FirstApp::loadGameObjects() {
    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "../models/flat_vase.obj");

    //generate vec
    auto gen = [](glm::vec3 offset, glm::vec3 start, std::size_t count) {
      std::vector<glm::vec3> pos;
      glm::vec3 offseto;
      offseto[0] = offset[0];
      offseto[1] = offset[1];
      offseto[2] = offset[2];
      for(int i = 0; i < count; i++) {
        offset[0] = (float)i * offseto[0] + offseto[0];
        offset[1] = (float)i * offseto[1] + offseto[1];
        offset[2] = (float)i * offseto[2] + offseto[2];
        pos.push_back(offset + start);
      }
      return pos;
    };
    auto genRow = [&gen](glm::vec3 start, std::size_t count) {
      auto left = gen({1.0f, 0.0f, 0.0f}, start, count);  
      auto right = gen({1.0f, 0.0f, 0.0f}, start, count);  
      right.insert(right.end(), left.begin(), left.end());
      return right;
    };
    auto gen2 = [&genRow](auto count, float z) {
      std::vector<glm::vec3> rows;  
      for(int i = 0; i <= count/2; i++) {
        auto row = genRow({1.0f, float(i), z}, count);
        rows.insert(rows.end(), row.begin(), row.end());
      }
      for(int i = 0; i < count/2; i++) {
        auto row = genRow({1.0f, -float(i), z}, count);
        rows.insert(rows.end(), row.begin(), row.end());
      }
      return rows;
    };
    auto gen3 = [&gen2](auto count) {
      std::vector<glm::vec3> faces;
      for(int i = 0; i <= count/2; i++)  {
        auto face = gen2(count, (2.5*(float)i)+2.5);
        faces.insert(faces.end(), face.begin(), face.end());
      }
      for(int i = 0; i < count/2; i++)  {
        auto face = gen2(count, (-2.5*(float)i)+2.5);
        faces.insert(faces.end(), face.begin(), face.end());
      }
      return faces;
    };

    for(auto& vec : gen3(40)) {
      auto gameObj = LveGameObject::createGameObject();
      gameObj.model = lveModel;
      gameObj.transform.translation = vec;
      gameObj.transform.scale = glm::vec3(3.f);
      gameObjects.push_back(std::move(gameObj));
    }    
  }
}
