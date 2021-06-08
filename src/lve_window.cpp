#include "../inc/lve_window.hpp"

// std
#include <stdexcept>

namespace lve {

  /*
   * Constructor
   */
  LveWindow::LveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
  }

  LveWindow::~LveWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  /*
   * initWindow
   */
  void LveWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  /*
   * createWindowSurface
   */
  void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface");
    }
  }

  /*
   * framebufferResizeCallback
   */
  void LveWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto lveWindow = reinterpret_cast<LveWindow *>(glfwGetWindowUserPointer(window));
    lveWindow->framebufferResized = true;
    lveWindow->width = width;
    lveWindow->height = height;
  }
}
