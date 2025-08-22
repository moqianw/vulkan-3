#include "Scene.hpp"

namespace GM {
    Scene::Scene()
    {

        
    }
    void Scene::init() {
        ///create material
        materialmanager.begin(device);
        GM::ShaderCreateInfo shadercreateinfo;

        shadercreateinfo.setDevice(device)
            .setShaderInfos(
                { GM::ShaderCreateInfo::ShaderInfo(
                    "./shaders/shader.vert.spv",vk::ShaderStageFlagBits::eVertex,"main"),
                     GM::ShaderCreateInfo::ShaderInfo(
                    "./shaders/shader.frag.spv",vk::ShaderStageFlagBits::eFragment,"main")
                }
            );
        auto shader = shadermanager.loadShader(shadercreateinfo);
        auto material = materialmanager.createMaterial(shader);
        auto materialinstance = std::make_shared<GM::MaterialInstance>();
        materialinstance->parent = material;
        auto rhimaterial = materialmanager.createRHIMaterial(materialinstance);
        std::vector<vk::DescriptorSetLayout> setlayouts;
        vk::DescriptorSetLayoutCreateInfo setlayoutcreateinfo;
        //descriptorset
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bindings.push_back(vk::DescriptorSetLayoutBinding()
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex).setDescriptorCount(1));


        setlayoutcreateinfo.setBindings(bindings)
            .setBindingCount(1);

        setlayouts.push_back(vk::DescriptorSetLayout(device.createDescriptorSetLayout(setlayoutcreateinfo)));
        std::vector<vk::PushConstantRange> pushconstants;
        pushconstants.push_back(vk::PushConstantRange()
            .setOffset(0)
            .setSize(sizeof(glm::mat4))
            .setStageFlags(vk::ShaderStageFlagBits::eVertex));
        auto pipelinelayout = materialmanager.createPipelineLayout(setlayouts, pushconstants);
        rhimaterial->pipelinelayout = pipelinelayout;
        auto pipeline = materialmanager.createGraphPipeline(renderpass, pipelinelayout, shader->getPipelineShaderStageCreateInfos());
        rhimaterial->pipeline = pipeline;
        ///set mesh
        auto commandbuffer = commandpool->allocateCommandBuffer();
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandbuffer.begin(begininfo);
        auto gameobject = std::make_shared<GameObject>();
        gameobjects.push_back(gameobject);
        auto meshrender = gameobject->addComponent<GM::MeshRender>();
        meshrender->mesh = std::make_shared<GM::Mesh>();
        GM::MeshLoadInfo loadinfo;
        auto& mesh = meshrender->mesh;
        mesh->vertices = {
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // 0
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // 1
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f,  0.0f,  1.0f}}, // 2
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f,  0.0f,  1.0f}}, // 3

    // 后面
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // 4
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // 5
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f,  0.0f, -1.0f}}, // 6
    {{-0.5f,  0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f,  0.0f, -1.0f}}, // 7
        };
        mesh->indices = {
            // 前面
    0, 1, 2,  2, 3, 0,
    // 右面
    1, 5, 6,  6, 2, 1,
    // 后面
    5, 4, 7,  7, 6, 5,
    // 左面
    4, 0, 3,  3, 7, 4,
    // 上面
    3, 2, 6,  6, 7, 3,
    // 下面
    4, 5, 1,  1, 0, 4
        };
        loadinfo.setDevice(device)
            .setPhysicalDevice(physicaldevice)
            .setCommandBuffer(commandbuffer)
            .setQueueFamilyIndices(queuefamilyindex.value());
        mesh->load(loadinfo);

        commandbuffer.end();
        vk::SubmitInfo submits;
        submits.setCommandBuffers({ commandbuffer });
        vk::FenceCreateInfo fenceinfo;
        auto fence = device.createFence(fenceinfo);
        presentqueue.submit(submits, fence);
        device.waitForFences(fence, true, std::numeric_limits<uint64_t>::max());
        device.destroyFence(fence);
        meshrender->material = rhimaterial;

        cameras.push_back(GM::Camera());
        auto& camera = cameras[0];
        
        auto sets = materialmanager.createDescriptorSet(setlayouts);
        camera.descriptorset = sets[0];
        UT::BufferCreateInfo bufferinfo;
        vk::BufferCreateInfo buffercreateinfo;
        buffercreateinfo.setQueueFamilyIndices(queuefamilyindex.value())
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(2 * sizeof(glm::mat4))
            .setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
        bufferinfo.setDevice(device)
            .setPhysicalDevice(physicaldevice)
            .setBufferCreateInfo(buffercreateinfo)
            .setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
            .allocateMemoryEnable(true)
            .mapMemoryEnable(true);

        camera.uniformbuffer = std::make_shared<UT::Buffer>(bufferinfo);
        auto ptr = camera.uniformbuffer->ptr;
        memcpy(ptr, &camera.mat, sizeof(camera.mat));
        vk::DescriptorBufferInfo descriptorbufferinfo;
        descriptorbufferinfo.setBuffer(camera.uniformbuffer->buffer)
            .setOffset(0)
            .setRange(2 * sizeof(glm::mat4));
        vk::WriteDescriptorSet setwrite;
        setwrite.setBufferInfo(descriptorbufferinfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstArrayElement(0)
            .setDstBinding(0)
            .setDstSet(camera.descriptorset);
        device.updateDescriptorSets({ setwrite }, {});
    }
    void Scene::destroy() {

        commandpool.reset();
        cameras.clear();
        gameobjects.clear();
        materialmanager.destroy();
        shadermanager.destroy();

    }
    Scene::~Scene()
    {
        destroy();
    }
}