#ifndef RSGL
#include "../../include/include/windows/rsgl.hpp"
#endif
#include "deps/win32_SetProcessDpiAware.c"
#include "../../include/include/windows/keys.hpp"

std::string wintest() { 
    return "windows gay";
}

HGLRC glrc;

RSGL::window::window(const char* name, RSGL::rect r, RSGL::color c, uint32_t flag/*=0*/) {
    if (flag & 32) {  }
    else { win32_SetProcessDpiAware(); } //We check if DPI is enabled

    HINSTANCE inh = GetModuleHandle(nullptr); // We get the instance

    WNDCLASSEXA Class;
	ZeroMemory(&Class, sizeof(WNDCLASSEXW));
    Class.cbSize = sizeof(WNDCLASSEXW);
    Class.lpszClassName = TEXT("RSGL Windows (Official)");
    Class.hInstance = inh;
    Class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    Class.hCursor = LoadCursor(NULL, IDC_ARROW);
    Class.lpfnWndProc = WindowProc;
    Class.hbrBackground = CreateSolidBrush(RGB(c.r, c.g, c.b));
    Class.cbClsExtra = 0;
	Class.cbWndExtra = 0;
    Class.lpszMenuName = NULL;
    Class.hIconSm = NULL;
    Class.style = CS_OWNDC;
    RegisterClassExA(&Class);

    RECT Rect = {r.x, r.y, r.length, r.width}; 
    DWORD list = WS_CLIPCHILDREN;

    if (flag & REMOVE_WINDOW_MARGINS)  list |= WS_POPUP;
    else list |= WS_CAPTION | WS_SYSMENU | WS_BORDER;
    if (!(flag & DISABLE_RESIZING)) list |= WS_SIZEBOX;
    if (!(flag & DISABLE_MAXIMIZING)) list |= WS_MAXIMIZEBOX;
    if (!(flag & DISABLE_MINIMIZING)) list |= WS_MINIMIZEBOX;


    AdjustWindowRect(&Rect, list, false);
    HWND window = CreateWindowEx(0, 
        TEXT("RSGL Windows (Official)"), 
        name, 
        list, 
        r.x, r.y, r.width, r.length, 
        NULL, 
        NULL, 
        inh, 
        NULL
    );                
    RSGL::window::hwnd = window;
    
    
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    
    HDC hdc = GetDC(hwnd);
    int format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, format, &pfd);
    glrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, glrc);
    
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

    RSGL::window::enabled_flags=flag;
    RSGL::window::r = r;
    RSGL::window::hwnd = window;
    RSGL::window::c = c;
    RSGL::win.owr = r;

    int show = SW_SHOW;
    if (flag & 8) { show=SW_SHOWMAXIMIZED; } // MAXAMIZED_WINDOW flag
    else if (flag & 16) { show=SW_SHOWMINIMIZED; } // MINIMIZED_WINDOW flag

    if (flag & 1)  {  }; // DEBUG, not implemented trolololol
    if (flag & 4)  { RSGL::window::fullscreen(true);}; // Fullscreen
    if (flag & WINDOW_CENTRALIZED) { RSGL::window::centralize();} //Centralized

    ShowWindow(window, show);
    UpdateWindow(window);
    SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0); // We check for mouse input
}

bool check_hardware_info;
int RSGL::window::checkEvents(){
    RSGL::window::event.button=-9;
    RSGL::window::event.key=0;
    RSGL::win.old_menu=0;
    RSGL::window::event.type = RSGL::window::ProcMSG();

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hwnd, &cursorPos);
    RSGL::window::event.x = cursorPos.x;
    RSGL::window::event.y = cursorPos.y;

    RSGL::win = {RSGL::window::r, RSGL::window::c, RSGL::window::enabled_flags, RSGL::window::hwnd, RSGL::window::event.type, {RSGL::window::event.x, RSGL::window::event.y}, event.button, win.old_menu, RSGL::win.old_key, RSGL::win.check, RSGL::win.owr};

    if (RSGL::win.enabled_flags & 2) { // We check if xinput is enabled
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        XInputGetState(0, &state);

        RSGL::window::event.pad = {state.Gamepad.sThumbLX, state.Gamepad.sThumbLY}; // L-stick
        RSGL::window::event.r_pad = {state.Gamepad.sThumbRX, state.Gamepad.sThumbRY}; // R-stick
        RSGL::window::event.triggers[0] = state.Gamepad.bLeftTrigger;
        RSGL::window::event.triggers[1] = state.Gamepad.bRightTrigger;
    }
    if (RSGL::win.enabled_flags & 1) {createDebugTable();}


    glClearColor((float)c.r/255, (float)c.g/255, (float)c.b/255, float(c.a)/255); 
    glClear(GL_COLOR_BUFFER_BIT);
    
    Sleep(1);
    return 0;
}

int RSGL::window::clear() {
    glFlush();

    PAINTSTRUCT ps;
    EndPaint(RSGL::window::hwnd, &ps);
    return 0;
}

int RSGL::window::close() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(glrc);
    
    RSAL::quit();
    DestroyWindow(RSGL::window::hwnd);
    PostQuitMessage(0);
    return 0;
}

void RSGL::window::resize(bool value){
    if (value) SetWindowLong(RSGL::window::hwnd, GWL_STYLE, GetWindowLong(RSGL::window::hwnd, GWL_STYLE)|WS_SIZEBOX);
    else SetWindowLong(RSGL::window::hwnd, GWL_STYLE, GetWindowLong(RSGL::window::hwnd, GWL_STYLE)&~WS_SIZEBOX);
}

void RSGL::window::fullscreen(bool value) {
    DWORD style = GetWindowLong(RSGL::window::hwnd, GWL_STYLE);
    MONITORINFO mi = { sizeof(mi) };
	if (value) {
		RECT rect;
		GetWindowRect(RSGL::window::hwnd, &rect);
        RSGL::window::r = {rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
        RSGL::win.r = RSGL::window::r;

		GetMonitorInfo(MonitorFromWindow(RSGL::window::hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(RSGL::window::hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(RSGL::window::hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
	else {
		GetMonitorInfo(MonitorFromWindow(RSGL::window::hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(RSGL::window::hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPos(RSGL::window::hwnd, HWND_NOTOPMOST, RSGL::window::r.x, RSGL::window::r.y, RSGL::window::r.width, RSGL::window::r.length, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}
void RSGL::window::position(RSGL::rect window){ MoveWindow(RSGL::window::hwnd, window.x, window.y, window.width, window.length ,true); }
void RSGL::window::maximize(bool value) {
    int g;
    if (value) g = SW_SHOWMAXIMIZED;
    else g=SW_SHOWNORMAL;
    ShowWindow(RSGL::window::hwnd, g);
}

void RSGL::window::centralize() {
	MONITORINFO mi = { sizeof(mi) };

	GetMonitorInfo(MonitorFromWindow(RSGL::window::hwnd, MONITOR_DEFAULTTONEAREST), &mi);
	int x = (mi.rcMonitor.right - mi.rcMonitor.left - RSGL::window::r.width) / 2;
	int y = (mi.rcMonitor.bottom - mi.rcMonitor.top - RSGL::window::r.length) / 2;

	SetWindowPos(RSGL::window::hwnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

int RSGL::window::changeIcon(const char* filename) {
    if (std::string(filename).find(".exe") != std::string::npos) {
        HICON hIcon;
        ExtractIconEx(filename,0,&hIcon,NULL,3);
        SendMessage(RSGL::window::hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

        return 0;
    }
    else {
        HANDLE hIcon = LoadImage(NULL, filename, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
        if (hIcon) {
            SendMessage(RSGL::window::hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            return 0;
        }
    }
    return -1;
}

int RSGL::window::changeName(const char* name) { SetWindowText(RSGL::window::hwnd, name); return 0; }



bool RSGL::window::isPressed(int key, int port/*=0*/) {
    if (RSGL::win.enabled_flags & 2) {
        key-=100;
        if ((key == XINPUT_GAMEPAD_A || key == XINPUT_GAMEPAD_B || key == XINPUT_GAMEPAD_X || key == XINPUT_GAMEPAD_Y) || 
            (key == XINPUT_GAMEPAD_LEFT_THUMB || key == XINPUT_GAMEPAD_RIGHT_THUMB) || 
            (key == XINPUT_GAMEPAD_LEFT_SHOULDER || key == XINPUT_GAMEPAD_RIGHT_THUMB) ||
            (key == XINPUT_GAMEPAD_START || key == XINPUT_GAMEPAD_BACK) ||
            (key == XINPUT_GAMEPAD_DPAD_UP || key == XINPUT_GAMEPAD_DPAD_DOWN || key == XINPUT_GAMEPAD_DPAD_LEFT || key == XINPUT_GAMEPAD_DPAD_RIGHT) 
        ) {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            
            XInputGetState(port, &state);
            return state.Gamepad.wButtons & key;
        }
        else if ((key == 0x5820 || key == 0x5821 || key == 0x5822 || key == 0x5823) || 
            (key == 0x5824 || key == 0x5825 || key == 0x5826 || key == 0x5827) || 
            (key == 0x5828 || key == 0x5829 || key == 0x5830 || key == 0x5831) || 
            (key == 0x5832 || key == 0x5833 || key == 0x5834 || key == 0x5835)) {
                XINPUT_STATE state;
                ZeroMemory(&state, sizeof(XINPUT_STATE));
                
                XInputGetState(port, &state);
                if (key == 0x5820 || key == 0x5821 || key == 0x5822 || key == 0x5823) {
                    int left[2] = {state.Gamepad.sThumbLX, state.Gamepad.sThumbLY};
                    return (left[0] >= 32767 && key == 0x5822) || (left[0] <= -32767 && key == 0x5823) || (left[1] >= 32767 && key == 0x5820) || (left[1] <= -32767 && key == 0x5821);
                }
                if (key == 0x5830 || key == 0x5831 || key == 0x5832 || key == 0x5833) {
                    int right[2] = {state.Gamepad.sThumbRX, state.Gamepad.sThumbRY};
                    return (right[0] >= 32767 && key == 0x5832) || (right[0] <= -32767 && key == 0x5833) || (right[1] >= 32767 && key == 0x5830) || (right[1] <= -32767 && key == 0x5831);
                }
            }
        else if (key == 0x5806 || key == 0x5807) { // RT and LT
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            XInputGetState(port, &state);
            return (state.Gamepad.bLeftTrigger > 0 || state.Gamepad.bRightTrigger > 0);
        }
        key+=100;
    }
    return GetKeyState(key) & 0x1000;
}

bool RSGL::window::isReleased(int key, int port/*=0*/) {
    if (RSGL::win.enabled_flags & 2) {
        /*Xinput needs to be implemented for isReleased.*/
    }
    int g = GetKeyState(key) & 0x1000; // We check if `key` is being pressed`
    if (g == 4096) { RSGL::win.old_key=key; return false; }  // If it is being pressed, set `RSGL::win.old_key` as `key` and return false
    else if (key==RSGL::win.old_key && g != 4096) {RSGL::win.old_key=695; return true;} // If `RSGL::win.old_key` is `key` but `key` isn't being pressed, then it means `key` was released recently. Set `RSGL::win.old_key` to anything to make sure it doesn't spam true
    return false;
}

bool RSGL::window::isClicked(int key, int port/*=0*/) {
    if (RSGL::win.enabled_flags & 2) {
        /*Xinput needs to be implemented for isClicked.*/
    }
    int g = GetKeyState(key) & 0x1000; 
    if (g == 4096 && key!=RSGL::win.old_key) { RSGL::win.old_key=key; return true; } 
    else if (g != 4096 && key==RSGL::win.old_key) {RSGL::win.old_key=635;}
    return false;
}

int RSGL::messageBox(std::string title, std::string message, int option/*=-1*/, UINT flags/*=0*/) {
    /* 
    -1 - Nothing
    0  - Error
    1  - Warning
    2  - Question
    */
    UINT opt=0;
    if (option == 0) opt = MB_ICONERROR | flags;
    else if (option == 1) opt = MB_ICONWARNING | flags;
    else if (option == 2) opt = MB_ICONQUESTION | flags;
    else if (option == -1) opt = flags;

    return MessageBox(RSGL::win.hwnd, message.c_str(), title.c_str(), opt);
}

std::string RSGL::fileDialog(const char* title, bool multiple/*=false*/, bool save/*=false*/, bool directory/*=false*/) {
    if (!directory) { 
        OPENFILENAME ofn;
        char szFile[260];
        
        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = 1024;
        ofn.lpstrFilter = title; 
        ofn.nFilterIndex = 0;//"All\0*.*\0Text\0*.TXT\0";
        if (multiple) ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER ;
        else ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        GetOpenFileNameA(&ofn);
        if (multiple) {
            char* ptr = ofn.lpstrFile;
            ptr[ofn.nFileOffset-1] = 0;
            ptr += ofn.nFileOffset;
            std::string list = ofn.lpstrFile;
            while (*ptr) {    
                list += " ";
                list += ptr;
                ptr += (lstrlen(ptr)+1);
            }
            return list;
        }
        return ofn.lpstrFile;
    }
    else {
        char szFolder[260];
        BROWSEINFO bi = { 0 };
        bi.pszDisplayName = szFolder;
        bi.lpszTitle  = title;
        bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_SHAREABLE;
        LPITEMIDLIST pidl = SHBrowseForFolder (&bi);
        if (pidl && SHGetPathFromIDList(pidl, szFolder))
        return bi.pszDisplayName;
    }
    return "";
}

std::string RSGL::getUsername() {
    char username[257];
    DWORD username_len = 257;
    GetUserName(username, &username_len);
    
    return (std::string)username;
}

int RSGL::getLanguage() {
    return PRIMARYLANGID(GetUserDefaultLCID());
}
std::string RSGL::getClipboardText() {
    OpenClipboard(nullptr);
    HANDLE hData = GetClipboardData(CF_TEXT);
    char* text = static_cast<char*>(GlobalLock(hData));
    std::string txt(text);
    GlobalUnlock(hData);
    CloseClipboard();

    return txt;
}

int RSGL::setClipboardText(std::string txt) {
    HANDLE global = GlobalAlloc(GMEM_FIXED, 32);
    memcpy(global, txt.c_str(), txt.size());

    OpenClipboard(RSGL::win.hwnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, global);
    CloseClipboard();

    return 0;
}

int fps_lasttime=0;
int fps_current; 
int fps_frames = 0; 
auto start = std::chrono::system_clock::now();

int RSGL::getFPS() {
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-start;
    fps_frames++;
    if (fps_lasttime < elapsed_seconds.count() - 1.0){
        fps_lasttime = elapsed_seconds.count();
        fps_current = fps_frames;
        fps_frames = 0;
    }
    return fps_current;
}

RSGL::debug RSGL::window::createDebugTable() {
    if (!check_hardware_info) {
        device_info info = RSGL::getGPUDeviceInfo();
        RSGL::window::debug.gpu = info.gpu;
        RSGL::window::debug.vram_available = info.vram;
        RSGL::window::debug.driver = info.driver_version;
        info = RSGL::getCPUDeviceInfo(true, false, false, false);
        RSGL::window::debug.cpu = info.cpu;
        info = RSGL::getOSDeviceInfo(true, false, false, false, false);
        RSGL::window::debug.os = info.os;
        check_hardware_info=true;
    }
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    RSGL::window::debug.fps = RSGL::getFPS();
    RSGL::window::debug.vram_used =  pmc.PeakWorkingSetSize/1048576/2*100;

    return RSGL::window::debug;
}
int g_scrollY=0;
LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
        case WM_COMMAND:{
            RSGL::win.old_menu = LOWORD(param);
            break;
        }
        case WM_VSCROLL: {
            auto action = LOWORD(param);
            HWND hScroll = (HWND)lparam;
            int pos = -1;
            if (action == SB_THUMBPOSITION || action == SB_THUMBTRACK) {
                pos = HIWORD(param);
            } else if (action == SB_LINEDOWN) {
                pos = g_scrollY + 5;
            } else if (action == SB_LINEUP) {
                pos = g_scrollY - 5;
            } 
            else break;
                
            SCROLLINFO si = { 0 };
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask = SIF_POS;
            si.nPos = pos;
            si.nTrackPos = 0;
            SetScrollInfo(h, SB_VERT, &si, true);

            RSGL::win.owr.x = -pos;

            return 0;
        }
        case WM_SIZE: {
            RECT rc = { 0 };
            GetClientRect(h, &rc);
		    if (!(RSGL::win.enabled_flags & 256))  glViewport(RSGL::win.r.x, RSGL::win.r.y, LOWORD(lparam), HIWORD(lparam));

            RSGL::win.r = {RSGL::win.r.x, RSGL::win.r.y, (int)LOWORD(lparam), (int)HIWORD(lparam)};
            SCROLLINFO si = { 0 };
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask = SIF_ALL;
            si.nMin = RSGL::win.owr.width;
            si.nMax = RSGL::win.owr.length-40;
            si.nPage = (rc.bottom - rc.top);
            si.nPos = RSGL::win.owr.x;
            si.nTrackPos = 0;
            SetScrollInfo(h, SB_VERT, &si, true);
            break;
        }
        case WM_NCHITTEST:
        return DefWindowProc(h, msg, param, lparam) == HTCLIENT ? HTCAPTION : DefWindowProc(h, msg, param, lparam) ? HTLEFT : DefWindowProc(h, msg, param, lparam);

        case WM_CLOSE:
		    wglMakeCurrent(NULL, NULL);
		    wglDeleteContext(glrc);
            
		    DestroyWindow(h);
		    PostQuitMessage(0);
            break;
    }
    return DefWindowProc(h, msg, param, lparam);
}

int RSGL::window::ProcMSG() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT:
            return RSGL::quit;

            case WM_KEYDOWN:
            RSGL::window::event.key = msg.wParam;
            return RSGL::KeyPressed;

            case WM_KEYUP:
            return RSGL::KeyReleased;

            case WM_MOUSEMOVE:
            return RSGL::MousePosChanged;

            case WM_LBUTTONDOWN:
            RSGL::window::event.button=1;
            return RSGL::MouseButtonPressed; 
            
            case WM_NCLBUTTONDOWN:
            RSGL::window::event.button=1;
            return RSGL::MouseButtonPressed;
            
            case WM_LBUTTONUP:
            return RSGL::MouseButtonReleased;

            case WM_RBUTTONDOWN:
            RSGL::window::event.button=2;
            return RSGL::MouseButtonPressed;

            case WM_RBUTTONUP:
            return RSGL::MouseButtonReleased;

            case WM_MBUTTONDOWN:
            RSGL::window::event.button=3;
            return RSGL::MouseButtonPressed;

            case WM_MBUTTONUP:
            return RSGL::MouseButtonReleased;
            
            case WM_MOUSEWHEEL:
            return RSGL::win.check;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}


LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode >= 0) {
    if (wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT *pMhs = (MSLLHOOKSTRUCT *)lParam;
        short zdelta = HIWORD(pMhs->mouseData);
        if (zdelta > 0) RSGL::win.check= 11;
        else RSGL::win.check = 12;
    }
  }
  return CallNextHookEx(0, nCode, wParam, lParam);
}

namespace RSGL{RSGL::drawable win=win;};