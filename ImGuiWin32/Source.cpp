#include <windows.h>
#include <iostream>
#include <d3d9.h>
#include "Functions.h"
#pragma comment(lib, "d3d9.lib")

bool ShowMenu = true;
bool InitPos = false;

void Render() {
    ImVec2 ScreenRes{ 0, 0 };
    ImVec2 WindowPos{ 0, 0 };
    ImVec2 WindowSize{ 600, 400 };
    if (InitPos == false) {
        RECT ScreenRect;
        GetWindowRect(GetDesktopWindow(), &ScreenRect);
        ScreenRes = ImVec2(float(ScreenRect.right), float(ScreenRect.bottom));
        WindowPos.x = (ScreenRes.x - WindowSize.x) * 0.5f;
        WindowPos.y = (ScreenRes.y - WindowSize.y) * 0.5f;
        InitPos = true;
    }

    ImGui::SetNextWindowPos(ImVec2(WindowPos.x, WindowPos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(WindowSize.x, WindowSize.y));
    ImGui::Begin("ImGui", &ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
    ImGui::End();
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Overlay.Name = RandomString(10).c_str();
    Overlay.WindowClass = {
        sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, Overlay.Name, NULL
    };

    RegisterClassEx(&Overlay.WindowClass);
    Overlay.Hwnd = CreateWindow(Overlay.Name, Overlay.Name, WS_POPUP, 0, 0, 5, 5, NULL, NULL, Overlay.WindowClass.hInstance, NULL);
    if (!CreateDeviceD3D(Overlay.Hwnd)) {
        ClearAll();
        return 1;
    }

    ShowWindow(Overlay.Hwnd, SW_HIDE);
    UpdateWindow(Overlay.Hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(Overlay.Hwnd);
    ImGui_ImplDX9_Init(DirectX9.pDevice);
    ZeroMemory(&DirectX9.Message, sizeof(DirectX9.Message));
    while (DirectX9.Message.message != WM_QUIT) {

        if (PeekMessage(&DirectX9.Message, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&DirectX9.Message);
            DispatchMessage(&DirectX9.Message);
            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            Render();
        }
        ImGui::EndFrame();

        DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (DirectX9.pDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            DirectX9.pDevice->EndScene();
        }

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT Result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
        if (Result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!ShowMenu) {
            DirectX9.Message.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    DestroyWindow(Overlay.Hwnd);
    ClearAll();
    return 0;
}