#include "hzpch.h"

#include "ImGuiLayer.h"

#include "Hazel/Application.h"
#include "Hazel/Events/Event.h"
#include "Hazel/KeyCodes.h"
#include "Hazel/CursorCodes.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// TODO: Set mouse cursors

namespace Hazel {

ImGuiLayer::ImGuiLayer()
	: Layer("ImGui") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnAttach() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.KeyMap[ImGuiKey_Tab] = HZ_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = HZ_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = HZ_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = HZ_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = HZ_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = HZ_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = HZ_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = HZ_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = HZ_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = HZ_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = HZ_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = HZ_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = HZ_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = HZ_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = HZ_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = HZ_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = HZ_KEY_A;
    io.KeyMap[ImGuiKey_C] = HZ_KEY_C;
    io.KeyMap[ImGuiKey_V] = HZ_KEY_V;
    io.KeyMap[ImGuiKey_X] = HZ_KEY_X;
    io.KeyMap[ImGuiKey_Y] = HZ_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = HZ_KEY_Z;

    ImGui_ImplOpenGL3_Init("#version 410 core");
}

void ImGuiLayer::OnUpdate() {
    Application& app = Application::Get();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

    float time = (float)glfwGetTime();
    io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
    m_Time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    static bool show = true;
    ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleased));
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseMoved));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(ImGuiLayer::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyReleased));
    dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyTyped));
}

bool ImGuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.GetMouseButton()] = true;

    return false;
}

bool ImGuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.GetMouseButton()] = false;

    return false;
}

bool ImGuiLayer::OnMouseMoved(MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.GetX(), event.GetY());

    return false;
}

bool ImGuiLayer::OnMouseScrolled(MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH = event.GetXOffset();
    io.MouseWheel = event.GetYOffset();

    return false;
}

bool ImGuiLayer::OnWindowResize(WindowResizeEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    glViewport(0, 0, event.GetWidth(), event.GetHeight());

    return false;
}

bool ImGuiLayer::OnKeyPressed(KeyPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[event.GetKeyCode()] = true;

    io.KeyCtrl = io.KeysDown[HZ_KEY_LEFT_CONTROL] || io.KeysDown[HZ_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[HZ_KEY_LEFT_SHIFT] || io.KeysDown[HZ_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[HZ_KEY_LEFT_ALT] || io.KeysDown[HZ_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[HZ_KEY_LEFT_SUPER] || io.KeysDown[HZ_KEY_RIGHT_SUPER];

    return false;
}

bool ImGuiLayer::OnKeyReleased(KeyReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[event.GetKeyCode()] = false;

    return false;
}

bool ImGuiLayer::OnKeyTyped(KeyTypedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((unsigned int)event.GetKeyCode());

    return false;
}

}