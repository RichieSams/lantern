#include <vulkan/vulkan.h>

// vkCreateDebugReportCallbackEXT self-loader
extern PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT_loader(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback) {
	PFN_vkVoidFunction temp_fp = vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (!temp_fp)
		throw "Failed to load vkCreateDebugReportCallbackEXT";

	fpCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(temp_fp);

	return vkCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT = &vkCreateDebugReportCallbackEXT_loader;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback) {
	return fpCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

// vkDestroyDebugReportCallbackEXT self-loader
extern PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT;

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT_loader(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks *pAllocator) {
	PFN_vkVoidFunction temp_fp = vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (!temp_fp)
		throw "Failed to load vkDestroyDebugReportCallbackEXT";

	fpDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(temp_fp);

	vkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}

PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT = &vkDestroyDebugReportCallbackEXT_loader;

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks *pAllocator) {
	fpDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}
