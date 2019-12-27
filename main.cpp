#include "vulkanhelpers.hpp"
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	std::vector<const char*> layers;

	vk::Instance instance = vkh::createInstance(extensions, layers);

	VkSurfaceKHR rawSurface;
	if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr, &rawSurface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
	vk::SurfaceKHR surface = rawSurface;

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	instance.destroySurfaceKHR(surface);
	vkh::destroyInstance(instance);

	return 0;
}