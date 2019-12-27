#pragma once

#include <vulkan/vulkan.hpp>
#include "Log.h"

namespace vkh {
	vk::Instance createInstance(const std::vector<const char *>& required_instance_extensions, const std::vector<const char *>& required_instance_layers);

	void destroyInstance(vk::Instance instance);
}