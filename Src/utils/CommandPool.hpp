#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>
namespace UT {
    class CommandPoolCreateInfo {
    public:
        CommandPoolCreateInfo& setDevice(const vk::Device& device) {
            this->device = device;
            return *this;

        }
        CommandPoolCreateInfo& setCommandPoolCreateInfo(const vk::CommandPoolCreateInfo& info) {
            this->commandpoolcreateinfo = info;
            return *this;
        }
    private:
        friend class CommandPool;
        vk::Device device = nullptr;
        vk::CommandPoolCreateInfo commandpoolcreateinfo;
    };
    class CommandPool {
    public:

        CommandPool(const CommandPoolCreateInfo& createinfo);

        CommandPool(const CommandPool&) = delete;
        CommandPool& operator=(const CommandPool&) = delete;


        vk::CommandBuffer allocateCommandBuffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

        std::vector<vk::CommandBuffer> allocateCommandBuffers(size_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

        void reset(vk::CommandPoolResetFlags flags = {});

        ~CommandPool();


    private:
        vk::Device device = nullptr;
        vk::CommandPool commandPool;
        std::vector<vk::CommandBuffer> commandBuffers;
    };

}
