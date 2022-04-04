#include "common.h"
#include "context.h"

void OnFramebufferSizeChanged(GLFWwindow *window, const int width,
                              const int height) {
    SPDLOG_INFO("Framebuffer size changed: ({} x {})", width, height);
    auto context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

void OnKeyEvent(GLFWwindow *window, const int key, const int scanCode,
                const int action, const int mods) {
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}", key, scanCode,
                action == GLFW_PRESS     ? "Pressed"
                : action == GLFW_RELEASE ? "Released"
                : action == GLFW_REPEAT  ? "Repeat"
                                         : "Unknown",
                mods & GLFW_MOD_CONTROL ? "C" : "-",
                mods & GLFW_MOD_SHIFT ? "S" : "-",
                mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnMouseButton(GLFWwindow *window, int button, int action, int modifer) {
    auto context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCursorPos(GLFWwindow *window, double x, double y) {
    auto context = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
    context->MouseMove(x, y);
}

int main(int argc, const char **argv) {
    SPDLOG_INFO("START PROGRAM");

    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char *descrption = nullptr;
        glfwGetError(&descrption);

        SPDLOG_ERROR("Failed to intialize glfw: {}", descrption);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
                                   nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("Failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("Failed to create context.");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, context.get());

    OnFramebufferSizeChanged(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChanged);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetCursorPosCallback(window, OnCursorPos);

    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        context->ProcessInput(window);
        context->Render();
        glfwSwapBuffers(window);
    }

    context.reset();
    glfwTerminate();

    return 0;
}