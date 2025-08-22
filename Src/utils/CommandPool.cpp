#include "CommandPool.hpp"
#include <stdexcept>
namespace UT {
    CommandPool::CommandPool(const CommandPoolCreateInfo& createinfo)
        : device(createinfo.device)
    {
        if (!device) throw std::runtime_error("Create CommandPool ERROR:  not set device");
        commandPool = device.createCommandPool(createinfo.commandpoolcreateinfo);
        if (!commandPool) throw std::runtime_error("Create CommandPool ERROR: Failed to create CommandPool");
    }

    vk::CommandBuffer CommandPool::allocateCommandBuffer(vk::CommandBufferLevel level) {
        if (!device) throw std::runtime_error("Create CommandPool ERROR: CommandPool device is null");

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(commandPool)
            .setLevel(level)
            .setCommandBufferCount(1);

        vk::CommandBuffer cmd = device.allocateCommandBuffers(allocInfo)[0];
        commandBuffers.push_back(cmd);
        return cmd;
    }

    std::vector<vk::CommandBuffer> CommandPool::allocateCommandBuffers(size_t count, vk::CommandBufferLevel level) {
        if (!device) throw std::runtime_error("Create CommandPool ERROR: CommandPool device is null");

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(commandPool)
            .setLevel(level)
            .setCommandBufferCount(static_cast<uint32_t>(count));

        std::vector<vk::CommandBuffer> cmds = device.allocateCommandBuffers(allocInfo);
        commandBuffers.insert(commandBuffers.end(), cmds.begin(), cmds.end());
        return cmds;
    }


    void CommandPool::reset(vk::CommandPoolResetFlags flags)
    {
        if (device && commandPool) {
            device.resetCommandPool(commandPool, flags);
            commandBuffers.clear(); // 记录的 CommandBuffer 已经无效
        }
    }


    CommandPool::~CommandPool() {
        if (device && commandPool) {
            if (!commandBuffers.empty()) {
                device.freeCommandBuffers(commandPool, commandBuffers);
            }
            device.destroyCommandPool(commandPool);
            commandPool = nullptr;
        }
        commandBuffers.clear();
        device = nullptr;
    }

}
