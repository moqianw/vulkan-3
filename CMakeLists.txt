cmake_minimum_required(VERSION 3.10)
project(VulkanWindow)
set(CMAKE_CXX_STANDARD 20)
add_executable(mvulkan 
	"Src/main.cpp"
    "Src/Contaxt.cpp"

    "Src/utils/File.cpp" 
    "Src/utils/ShaderStage.cpp" 
    "Src/utils/Vertex.cpp"
    "Src/utils/CommandPool.cpp"
    "Src/utils/Buffer.cpp"
    
    "Src/utils/Image.cpp" 
    "Src/GameObject/GameObject.cpp"
    "Src/GameObject/Mesh.cpp" 
    "Src/GameObject/Scene.cpp"
    "Src/GameObject/Material.cpp" 
    "Src/utils/DescriptorPool.cpp" 
    "Src/Render.cpp" 
    "Src/GameObject/Shader.cpp" 
    "Src/GameObject/Camera.cpp" "Src/GameObject/Pipeline.cpp" "Src/utils/Resource/Allocation.cpp")
option(USE_LOCAL_VULKAN "Use Vulkan from local SDK path" ON)
if (USE_LOCAL_VULKAN)
    set(VULKAN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/Include")
    set(VULKAN_LIB_DIR     "${CMAKE_SOURCE_DIR}/thirdparty/Lib")
else()
    find_package(Vulkan REQUIRED)
    find_package(SDL REQUIRED)
    find_package(glm REQUIRED)
endif()

target_include_directories(mvulkan PRIVATE ${VULKAN_INCLUDE_DIR})
target_link_directories(mvulkan PRIVATE ${VULKAN_LIB_DIR})
target_link_libraries(mvulkan PRIVATE vulkan-1 SDL3 glm)


add_custom_command(TARGET mvulkan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/thirdparty/Bin/SDL3.dll"
        "$<TARGET_FILE_DIR:mvulkan>"
)

set(VULKAN_RESOURCES "${CMAKE_SOURCE_DIR}/Resources")
# -----------------------
# GLSL 编译成 SPIR-V
# -----------------------

# 找 glslangValidator
find_program(GLSLANG_VALIDATOR NAMES glslangValidator
    HINTS ${VULKAN_RESOURCES}
)
if (NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! Install Vulkan SDK.")
endif()
set(VULKAN_SHADER_SPV_DIR "${VULKAN_RESOURCES}/shaders")
# 查找 shaders 文件夹下的着色器
file(GLOB_RECURSE SHADER_FILES
    "${VULKAN_SHADER_SPV_DIR}/*.vert"
    "${VULKAN_SHADER_SPV_DIR}/*.frag"
    "${VULKAN_SHADER_SPV_DIR}/*.comp"
)

set(SPV_FILES "")
foreach(SHADER ${SHADER_FILES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPV "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPV}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SPV}
        DEPENDS ${SHADER}                      # 自动检测修改
        COMMENT "Compiling GLSL shader ${FILE_NAME}"
        VERBATIM
    )

    list(APPEND SPV_FILES ${SPV})
endforeach()

# 统一 shader 编译目标
add_custom_target(ShadersTarget ALL DEPENDS ${SPV_FILES})

# 让 mvulkan 依赖 shader
add_dependencies(mvulkan ShadersTarget)


target_compile_definitions(mvulkan PRIVATE GLM_ENABLE_EXPERIMENTAL)
