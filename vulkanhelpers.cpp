#include "vulkanhelpers.hpp"
#include <map>

std::map<vk::Instance, VkDebugUtilsMessengerEXT> callbacks;

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	LOG << "Debug Messenger Created" << ENDL;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}

	LOG << "Debug Messenger Destroyed" << ENDL;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	LOG << "validation layer: " << pCallbackData->pMessage << ENDL;

	return VK_FALSE;
}

vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() {
	return vk::DebugUtilsMessengerCreateInfoEXT(
		vk::DebugUtilsMessengerCreateFlagsEXT(),
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback,
		nullptr
	);
}

bool validate_extensions(const std::vector<const char *>& required, const std::vector<vk::ExtensionProperties>& available)
{
	for (auto extension : required)
	{
		bool found = false;
		for (auto &available_extension : available)
		{
			if (strcmp(available_extension.extensionName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}

bool validate_layers(const std::vector<const char *>& required, const std::vector<vk::LayerProperties>& available)
{
	for (auto extension : required)
	{
		bool found = false;
		for (auto &available_extension : available)
		{
			if (strcmp(available_extension.layerName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}

vk::Instance vkh::createInstance(const std::vector<const char *>& required_instance_extensions, const std::vector<const char *>& required_instance_layers)
{
	LOG << "Initializing vulkan instance." << ENDL;

	std::vector<vk::ExtensionProperties> instance_extensions = vk::enumerateInstanceExtensionProperties(nullptr);

	std::vector<const char *> active_instance_extensions(required_instance_extensions);

	active_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	if (!validate_extensions(active_instance_extensions, instance_extensions))
	{
		throw std::runtime_error("Required instance extensions are missing.");
	}

	LOG << "Extensions:" << ENDL;
	for(auto ext : active_instance_extensions) {
		LOG << "\t" << ext << ENDL;
	}

	std::vector<vk::LayerProperties> instance_layers = vk::enumerateInstanceLayerProperties();

	std::vector<const char *> active_instance_layers(required_instance_layers);

	active_instance_layers.push_back("VK_LAYER_GOOGLE_threading");
	active_instance_layers.push_back("VK_LAYER_LUNARG_parameter_validation");
	active_instance_layers.push_back("VK_LAYER_LUNARG_object_tracker");
	active_instance_layers.push_back("VK_LAYER_LUNARG_core_validation");
	active_instance_layers.push_back("VK_LAYER_GOOGLE_unique_objects");

	if (!validate_layers(active_instance_layers, instance_layers))
	{
		throw std::runtime_error("Required instance layers are missing.");
	}

	LOG << "Layers:" << ENDL;
	for(auto lay : active_instance_layers) {
		LOG << "\t" << lay << ENDL;
	}

	vk::ApplicationInfo app(
		"UnNamed", 0U,
		"KW3D", 0U,
		VK_MAKE_VERSION(1, 0, 0)
	);
	
	vk::InstanceCreateInfo instance_info(
		vk::InstanceCreateFlags(),
		&app,
		active_instance_layers.size(),
		active_instance_layers.data(),
		active_instance_extensions.size(),
		active_instance_extensions.data()
	);

	vk::DebugUtilsMessengerCreateInfoEXT messenger_info(
		vk::DebugUtilsMessengerCreateFlagsEXT(),
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback,
		nullptr
	);

	instance_info.setPNext(&messenger_info);

	// Create the Vulkan instance
	vk::Instance instance = vk::createInstance(instance_info);

	LOG << "Vulkan Instance Created" << ENDL;

	VkDebugUtilsMessengerEXT callback;
	if (CreateDebugUtilsMessengerEXT(instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&messenger_info), nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
	callbacks[instance] = callback;

	return instance;
}

void vkh::destroyInstance(vk::Instance instance) {
	DestroyDebugUtilsMessengerEXT(instance, callbacks[instance], nullptr);
	instance.destroy();
	LOG << "Vulkan Instance Destroyed" << ENDL;
	Log::destroy();
}