#include "Scene.hpp"

namespace GM {
    Scene::Scene()
    {

        
    }
    void Scene::init() {
        ///resourcemanager

        resourcemanager.setDevice(device)
            .setPhysicalDevice(physicaldevice)
            .setQueueFamilyIndex(queuefamilyindex.value());
        resourcemanager.init();


        //pipeline

        pipelinemanager.setDevice(device);
        pipelinemanager.init();
        ///create material
        materialmanager.begin(device);
    }
    void Scene::initData()
    {
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

        materialmanager.addDescriptorSetLayouts(setlayouts);
        std::vector<vk::PushConstantRange> pushconstants;
        pushconstants.push_back(vk::PushConstantRange()
            .setOffset(0)
            .setSize(sizeof(glm::mat4))
            .setStageFlags(vk::ShaderStageFlagBits::eVertex));
        auto pipelinelayout = pipelinemanager.createPipelineLayout(setlayouts, pushconstants);
        rhimaterial->pipelinelayout = pipelinelayout;
        auto pipeline = pipelinemanager.createGraphPipeline(renderpass, pipelinelayout, shader->getPipelineShaderStageCreateInfos());
        rhimaterial->pipeline = pipeline;
        ///set mesh
        auto commandbuffer = commandpool->allocateCommandBuffer();
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandbuffer.begin(begininfo);
        auto gameobject = std::make_shared<GameObject>();
        gameobjects.push_back(gameobject);
        auto meshrender = gameobject->addComponent<GM::MeshRender>();


        std::vector<UT::Vertex4> vertices = {
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
        std::vector<uint32_t> indices = {
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

        GM::MeshCreateInfo meshcreateinfo;
        meshcreateinfo.setIndicesCount(indices.size())
            .setIndicesSize(sizeof(uint32_t) * indices.size())
            .setVerticesCount(vertices.size())
            .setVerticesSize(sizeof(UT::Vertex4) * vertices.size());
        meshrender->mesh = resourcemanager.createMesh(meshcreateinfo);


        GM::MeshLoadInfo loadinfo;
        loadinfo
            .setCommandBuffer(commandbuffer)
            .setIndices(indices)
            .setVectices(vertices);
        resourcemanager.loadData(meshrender->mesh, loadinfo);

        auto& mesh = meshrender->mesh;

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
        bufferinfo
            .setBufferCreateInfo(buffercreateinfo)
            .setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
            .setAllocateMemoryEnable(true)
            .setMapMemoryEnable(true);

        camera.uniformbuffer = resourcemanager.createBuffer(bufferinfo);
        auto ptr = camera.uniformbuffer.ptr;
        memcpy(ptr, &camera.mat, sizeof(camera.mat));
        vk::DescriptorBufferInfo descriptorbufferinfo;
        descriptorbufferinfo.setBuffer(camera.uniformbuffer.buffer)
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
        resourcemanager.destroy();
        pipelinemanager.destroy();
        materialmanager.destroy();
        shadermanager.destroy();

    }
    Scene::~Scene()
    {
        destroy();
    }
}