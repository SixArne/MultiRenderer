#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
#include <vulkan/vulkan.h>

const std::vector<const char*> g_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//#define DISABLE_OBJ

namespace Utils
{
	//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
	static bool ParseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
	{
#ifdef DISABLE_OBJ

		//TODO: Enable the code below after uncommenting all the vertex attributes of DataTypes::Vertex
		// >> Comment/Remove '#define DISABLE_OBJ'
		assert(false && "OBJ PARSER not enabled! Check the comments in Utils::ParseOBJ");

#else

		std::ifstream file(filename);
		if (!file)
			return false;

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<Vector2> UVs{};

		vertices.clear();
		indices.clear();

		std::string sCommand;
		// start a while iteration ending when the end of file is reached (ios::eof)
		while (!file.eof())
		{
			//read the first word of the string, use the >> operator (istream::operator>>) 
			file >> sCommand;
			//use conditional statements to process the different commands	
			if (sCommand == "#")
			{
				// Ignore Comment
			}
			else if (sCommand == "v")
			{
				//Vertex
				float x, y, z;
				file >> x >> y >> z;

				positions.emplace_back(x, y, z);
			}
			else if (sCommand == "vt")
			{
				// Vertex TexCoord
				float u, v;
				file >> u >> v;
				UVs.emplace_back(u, 1 - v);
			}
			else if (sCommand == "vn")
			{
				// Vertex Normal
				float x, y, z;
				file >> x >> y >> z;

				normals.emplace_back(x, y, z);
			}
			else if (sCommand == "f")
			{
				//if a face is read:
				//construct the 3 vertices, add them to the vertex array
				//add three indices to the index array
				//add the material index as attibute to the attribute array
				//
				// Faces or triangles
				Vertex vertex{};
				size_t iPosition, iTexCoord, iNormal;

				uint32_t tempIndices[3];
				for (size_t iFace = 0; iFace < 3; iFace++)
				{
					// OBJ format uses 1-based arrays
					file >> iPosition;
					vertex.position = positions[iPosition - 1];

					if ('/' == file.peek())//is next in buffer ==  '/' ?
					{
						file.ignore();//read and ignore one element ('/')

						if ('/' != file.peek())
						{
							// Optional texture coordinate
							file >> iTexCoord;
							vertex.uv = UVs[iTexCoord - 1];
						}

						if ('/' == file.peek())
						{
							file.ignore();

							// Optional vertex normal
							file >> iNormal;
							vertex.normal = normals[iNormal - 1];
						}
					}

					vertices.push_back(vertex);
					tempIndices[iFace] = uint32_t(vertices.size()) - 1;
					//indices.push_back(uint32_t(vertices.size()) - 1);
				}

				indices.push_back(tempIndices[0]);
				if (flipAxisAndWinding) 
				{
					indices.push_back(tempIndices[2]);
					indices.push_back(tempIndices[1]);
				}
				else
				{
					indices.push_back(tempIndices[1]);
					indices.push_back(tempIndices[2]);
				}
			}
			//read till end of line and ignore all remaining chars
			file.ignore(1000, '\n');
		}

		//Cheap Tangent Calculations
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t index0 = indices[i];
			uint32_t index1 = indices[size_t(i) + 1];
			uint32_t index2 = indices[size_t(i) + 2];

			const Vector3& p0 = vertices[index0].position;
			const Vector3& p1 = vertices[index1].position;
			const Vector3& p2 = vertices[index2].position;
			const Vector2& uv0 = vertices[index0].uv;
			const Vector2& uv1 = vertices[index1].uv;
			const Vector2& uv2 = vertices[index2].uv;

			const Vector3 edge0 = p1 - p0;
			const Vector3 edge1 = p2 - p0;
			const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
			const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
			float r = 1.f / Vector2::Cross(diffX, diffY);

			Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
			vertices[index0].tangent += tangent;
			vertices[index1].tangent += tangent;
			vertices[index2].tangent += tangent;
		}

		//Fix the tangents per vertex now because we accumulated
		for (auto& v : vertices)
		{
			v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();

			if(flipAxisAndWinding)
			{
				v.position.z *= -1.f;
				v.normal.z *= -1.f;
				v.tangent.z *= -1.f;
			}

		}

		return true;
#endif
	}
#pragma warning(pop)


	static std::vector<char> ReadFile(const std::string& filename)
	{
		// Read as binary and put read pointer to end.
		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		// Check if file stream successfully opened
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open a file");
		}

		// Returns position of read pointer
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> fileBuffer(fileSize);

		// Move read position to start of file
		file.seekg(0);

		// Read file data into buffer stream file size in total
		file.read(fileBuffer.data(), fileSize);

		// close
		file.close();

		return fileBuffer;
	}

	static uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
	{
		// Get properties of physical device memory
		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i{}; i < memProperties.memoryTypeCount; i++)
		{
			// Index of memory type must match corresponding bit in allowed types
			// Desired property bit flags are part of memory bit flags
			if ((allowedTypes & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				// memory type is valid
				return i;
			}
		}
	}

	static void CreateBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkDeviceSize bufferSize,
		VkBufferUsageFlags bufferUsage,
		VkMemoryPropertyFlags bufferProperties,
		VkBuffer* buffer,
		VkDeviceMemory* bufferMemory)
	{
		// CREATE VERTEX BUFFER
		// just info about buffer no memory included
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;								// Size of buffer
		bufferInfo.usage = bufferUsage;								// What the buffer will be used for
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// Similar to swapchain images can share vertex buffers

		VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create vertex buffer");
		}

		// GET BUFFER MEMORY REQUIREMENTS
		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

		// ALLOCATE MEMORY TO BUFFER
		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = FindMemoryTypeIndex(
			physicalDevice,
			memRequirements.memoryTypeBits,
			bufferProperties
		);

		// Allocate memory to VkDeviceMemory
		result = vkAllocateMemory(device, &memAllocInfo, nullptr, bufferMemory);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate vertex buffer memory");
		}

		// Allocate memory to given vertex buffer
		vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
	}


	static VkCommandBuffer BeginCommandBuffer(VkDevice device, VkCommandPool commandPool)
	{
		// Command buffer to hold transfer commands
		VkCommandBuffer commandBuffer{};

		// Command buffer details
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		// Allocate command buffer from pool
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		// Information to begin command buffer record
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Only using the command buffer once

		// Begin recording transfer commands
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	static void EndAndSubmitCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer)
	{
		// End recording
		vkEndCommandBuffer(commandBuffer);

		// Queue submission information
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Submit transfer command to transfer queue and wait until it finishes
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		// Free temporary command buffer back to pool
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	static void CopyBuffer(
		VkDevice device,
		VkQueue transferQueue,
		VkCommandPool transferCommandPool,
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize bufferSize)
	{
		VkCommandBuffer transferCommandBuffer = BeginCommandBuffer(device, transferCommandPool);

		{
			// Region of data to copy from and to
			VkBufferCopy bufferCopyRegion{};
			bufferCopyRegion.srcOffset = 0;
			bufferCopyRegion.dstOffset = 0;
			bufferCopyRegion.size = bufferSize;

			// Command to copy src buffer to dst buffer
			vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);
		}

		EndAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
	}

	static void CopyImageBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
		VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer transferCommandBuffer = BeginCommandBuffer(device, transferCommandPool);

		{
			VkBufferImageCopy imageRegion{};
			imageRegion.bufferOffset = 0;
			imageRegion.bufferRowLength = 0;
			imageRegion.bufferImageHeight = 0;
			imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageRegion.imageSubresource.mipLevel = 0;
			imageRegion.imageSubresource.baseArrayLayer = 0;
			imageRegion.imageSubresource.layerCount = 1;
			imageRegion.imageOffset = { 0,0,0 };
			imageRegion.imageExtent = { width, height, 1 };

			vkCmdCopyBufferToImage(transferCommandBuffer, srcBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageRegion);
		}

		EndAndSubmitCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
	}

	static void TransitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginCommandBuffer(device, commandPool);

		{
			VkImageMemoryBarrier memoryBarrier{};

			memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier.oldLayout = oldLayout;
			memoryBarrier.newLayout = newLayout;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.image = image;
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier.subresourceRange.baseMipLevel = 0;
			memoryBarrier.subresourceRange.layerCount = 1;
			memoryBarrier.subresourceRange.levelCount = 1;
			memoryBarrier.subresourceRange.baseArrayLayer = 0;

			VkPipelineStageFlags srcStage{};
			VkPipelineStageFlags dstStage{};

			// if transitioning from new image to image ready to receive data
			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = 0;								// Memory access stage transition must happen after...
				memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;		// Memory access stage transition must happen before...

				srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			// If transitioning from transfer destination to shader readable...
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}

			vkCmdPipelineBarrier(
				commandBuffer,
				srcStage, dstStage,				// Match to src and dst access masks
				0,											// Dependency flags
				0, nullptr,					// Memory barrier count + data
				0, nullptr,		// Buffer memory barrier count + data
				1, &memoryBarrier
			);
		}

		EndAndSubmitCommandBuffer(device, commandPool, queue, commandBuffer);
	}
}


// Indices (locations) of Queue families (if they exist at all)
struct QueueFamilyIndices
{
	int32_t graphicsFamily = -1;			// Location of graphics queue family on GPU (-1 means non-existent)
	int32_t presentationFamily = -1;		// Location of presentation queue family

	// Check if queue families are valid
	bool IsValid() const
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapchainDetails
{
	VkSurfaceCapabilitiesKHR capabilities{};				// Surface properties e.g. image size/extent
	std::vector<VkSurfaceFormatKHR> formats{};				// Surface image formats e.g. RGBA and size of each color
	std::vector<VkPresentModeKHR> presentationModes{};		// Surface presentationModes: e.g. Mailbox
};