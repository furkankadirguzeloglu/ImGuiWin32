#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

struct OverlayWindow {
    WNDCLASSEX WindowClass;
    HWND Hwnd;
    LPCSTR Name;
}Overlay;

struct DirectX9Interface {
    LPDIRECT3D9 IDirect3D9 = NULL;
    LPDIRECT3DDEVICE9 pDevice = NULL;
    D3DPRESENT_PARAMETERS pParameters = { NULL };
    MSG Message = { NULL };    
}DirectX9;

bool CreateDeviceD3D(HWND hWnd) {
    if ((DirectX9.IDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
        return false;
    }
    ZeroMemory(&DirectX9.pParameters, sizeof(DirectX9.pParameters));
    DirectX9.pParameters.Windowed = TRUE;
    DirectX9.pParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    DirectX9.pParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    DirectX9.pParameters.EnableAutoDepthStencil = TRUE;
    DirectX9.pParameters.AutoDepthStencilFormat = D3DFMT_D16;
    DirectX9.pParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (DirectX9.IDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX9.pParameters, &DirectX9.pDevice) < 0) {
        return false;
    }
    return true;
}

void ClearD3D() {
    if (DirectX9.pDevice) {
        DirectX9.pDevice->Release();
        DirectX9.pDevice = NULL;
    }

    if (DirectX9.IDirect3D9) {
        DirectX9.IDirect3D9->Release();
        DirectX9.IDirect3D9 = NULL;
    }
}

void ClearAll() {
    ClearD3D();
    UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = DirectX9.pDevice->Reset(&DirectX9.pParameters);
    if (hr == D3DERR_INVALIDCALL) {
        IM_ASSERT(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (DirectX9.pDevice != NULL && wParam != SIZE_MINIMIZED) {
            DirectX9.pParameters.BackBufferWidth = LOWORD(lParam);
            DirectX9.pParameters.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::string RandomString(int len) {
    srand(time(NULL));
    std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string newstr;
    int pos;
    while (newstr.size() != len) {
        pos = ((rand() % (str.size() - 1)));
        newstr += str.substr(pos, 1);
    }
    return newstr;
}