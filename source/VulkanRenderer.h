#pragma once
#include "BaseRenderer.h"
#include "Mesh.h"
#include "VulkanMesh.h"
#include <vulkan/vulkan.h>
#include "Utils.h"

#define MAX_FRAME_DRAWS 3
#define MAX_OBJECTS 20

class VulkanRenderer : public BaseRenderer
{
public:
	VulkanRenderer(SDL_Window* pWindow, Camera* pCamera, std::vector<MeshData*> pMeshes);
	virtual ~VulkanRenderer() override;

	virtual void Update(Timer* pTimer) override;
	virtual void Render() override;
	void Cleanup();

private:
	int Init(std::vector<MeshData*>& pMeshes);

	std::vector<VulkanMesh*> m_pMeshes{};
	bool m_IsInitialized{ false };

	uint32_t m_CurrentFrame{};

	// Scene settings
	struct UboViewProjection {
		Matrix projection;
		Matrix view;
	} m_UboViewProjection;

	// Vulkan components
	VkInstance m_Instance;
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} m_MainDevice;
	VkQueue m_GraphicsQueue{};
	VkQueue m_PresentationQueue{};
	VkSurfaceKHR m_Surface{};
	VkSwapchainKHR m_Swapchain{};
	VkSampler m_TextureSampler{};

	struct SwapchainImage
	{
		VkImage image;
		VkImageView imageView;
	};

	// These 3 will ALWAYS use the same index.
	// So getting a command at index 0 will get the frame buffer at index 0 and the swapchain at index 0
	std::vector<SwapchainImage> m_SwapchainImages{};
	std::vector<VkFramebuffer> m_SwapchainFramebuffers{};
	std::vector<VkCommandBuffer> m_CommandBuffers{};

	// Depth stencil
	VkImage m_DepthBufferImage{};
	VkDeviceMemory m_DepthBufferImageMemory{};
	VkImageView m_DepthBufferImageView{};
	VkFormat m_DepthFormat{};

	// - Descriptor
	VkDescriptorSetLayout m_DescriptorSetLayout{};
	VkDescriptorSetLayout m_SamplerSetLayout{};

	VkPushConstantRange m_PushConstantRange{};

	VkDescriptorPool m_DescriptorPool{};
	VkDescriptorPool m_SamplerDescriptorPool{};
	std::vector<VkDescriptorSet> m_DescriptorSets{};
	std::vector<VkDescriptorSet> m_SamplerDescriptorSets{};

	std::vector<VkBuffer> m_VPUniformBuffer{};
	std::vector<VkDeviceMemory> m_VPUniformBufferMemory{};

	std::vector<VkBuffer> m_ModelDynamicUniformBuffer{};
	std::vector<VkDeviceMemory> m_ModelDynamicUniformBufferMemory{};


	// VkDeviceSize m_MinUniformBufferOffset{};
	// size_t m_ModelUniformAllignment{};
	// Model* m_ModelTransferSpace{};

	// - Assets
	std::vector<VkImage> m_TextureImages{};
	std::vector<VkDeviceMemory> m_TextureImageMemory{};
	std::vector<VkImageView> m_TextureImageViews{};
	std::vector<VulkanMesh> m_ModelList{};

	// - Pipeline
	VkPipeline m_GraphicsPipeline{};
	VkPipelineLayout m_PipelineLayout{};
	VkRenderPass m_RenderPass{};

	// - Pools
	VkCommandPool m_GraphicsCommandPool{};

	// - Utility
	VkFormat m_SwapchainImageFormat{};
	VkExtent2D m_SwapchainExtent{};

	// - Synchronization
	std::vector<VkSemaphore> m_ImagesAvailable{};
	std::vector<VkSemaphore> m_RendersFinished{};
	std::vector<VkFence> m_DrawFences{};

	const std::vector<const char*> m_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	bool m_EnableValidationLayers{ false };


	// Vulkan functions
	// - Create functions
	void CreateInstance();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateDebugMessenger();
	void CreateSwapchain();
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreatePushConstantRange();
	void CreateGraphicsPipeline();
	void CreateDepthBufferImage();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSynchronization();
	void CreateTextureSampler();

	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();

	void UpdateUniformBuffers(uint32_t imageIndex);

	// - Record functions
	void RecordCommands(uint32_t currentImage);

	// - Allocate functions
	//void AllocateDynamicBufferTransferSpace();

	// - Destroy functions
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	// - Get functions
	void GetPhysicalDevice();
	std::vector<const char*> GetRequiredExtensions();

	// - Support functions
	bool CheckValidationEnabled();

	// -- Checker functions
	bool CheckValidationLayerSupport();
	bool CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool CheckDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	// -- Choose functions
	VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
	VkFormat ChooseSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);

	// -- Populate functions
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

	// -- Getter functions
	QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
	SwapchainDetails GetSwapChainDetails(VkPhysicalDevice device);

	// - Create functions
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	VkImage CreateImage(
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags useFlags,
		VkMemoryPropertyFlags propFlags,
		VkDeviceMemory* imageMemory
	);

	int CreateTextureImage(std::string filename);
	int CreateTexture(std::string filename);
	int CreateTextureDescriptor(VkImageView textureImage);

	// -- Loader functions
	SDL_Surface* LoadTextureFile(std::string& filename, int* width, int* height, VkDeviceSize* imageSize);

	// Validation layer stuff
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};

