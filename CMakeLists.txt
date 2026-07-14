cmake_minimum_required(VERSION 3.28)

cmake_path(CONVERT "$ENV{VCPKG_ROOT}" TO_CMAKE_PATH_LIST VCPKG_ROOT)

# ---- Project ----

project(
	ParticleSimulator
	VERSION 1.0.0
	LANGUAGES CXX
)

# ---- Include guards ----

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif ()

# ---- Dependency: glfw3 ----

find_package(glfw3 CONFIG REQUIRED)

# ---- Dependency: Cyphall/CyphGPU ----

find_package(CyphGPU CONFIG REQUIRED)

# ---- Executable ----

add_executable(ParticleSimulator)

target_sources(ParticleSimulator PRIVATE
	"src/cpp/ParticleSimulator/Camera.cpp"
	"src/cpp/ParticleSimulator/Main.cpp"
	"src/cpp/ParticleSimulator/Helper/MathHelper.cpp"
	"src/cpp/ParticleSimulator/Rendering/Renderer.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ComputePass.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/GravityViewPass.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.cpp"
)

target_sources(ParticleSimulator PRIVATE FILE_SET HEADERS BASE_DIRS "src/cpp" FILES
	"src/cpp/ParticleSimulator/Camera.h"
	"src/cpp/ParticleSimulator/Helper/MathHelper.h"
	"src/cpp/ParticleSimulator/Rendering/ParticleData.h"
	"src/cpp/ParticleSimulator/Rendering/Renderer.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ComputePass.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/GravityViewPass.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h"
)

cgpu_target_shader_pipelines(ParticleSimulator shaders "src/slang"
	"src/slang/gravityView.slang"
	"src/slang/particleView.slang"
	"src/slang/simulateParticles.slang"
)

set_target_properties(ParticleSimulator PROPERTIES CXX_STANDARD 23)
set_target_properties(ParticleSimulator PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(ParticleSimulator PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(ParticleSimulator PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
set_target_properties(ParticleSimulator PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set_target_properties(ParticleSimulator PROPERTIES CXX_SCAN_FOR_MODULES OFF)

# ---- Dependency: glfw3 ----

target_link_libraries(ParticleSimulator PRIVATE glfw)

target_compile_definitions(ParticleSimulator PRIVATE GLFW_INCLUDE_NONE)

# ---- Dependency: Cyphall/CyphGPU ----

target_link_libraries(ParticleSimulator PRIVATE CyphGPU)
