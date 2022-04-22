#ifndef SWGL
#include "../../SWGL.hpp"
#endif
#include "deps/win32_SetProcessDpiAware.c"

std::string wintest() { 
    return "windows gay";
}

HGLRC glrc;

SWGL::window::window(const char* name, SWGL::rect r, SWGL::color c, uint32_t flag/*=0*/) {
    if (flag & 32) {  }
    else { win32_SetProcessDpiAware(); } //We check if DPI is enabled

    HINSTANCE inh = GetModuleHandle(nullptr); // We get the instance

    WNDCLASSEXA Class;
	ZeroMemory(&Class, sizeof(WNDCLASSEXW));
    Class.cbSize = sizeof(WNDCLASSEXW);
    Class.lpszClassName = TEXT("SWGL Windows (Official)");
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
        TEXT("SWGL Windows (Official)"), 
        name, 
        list, 
        r.x, r.y, r.width, r.length, 
        NULL, 
        NULL, 
        inh, 
        NULL
    );                
    SWGL::window::hwnd = window;
    
    
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

    SWGL::window::enabled_flags=flag;
    SWGL::window::r = r;
    SWGL::window::hwnd = window;
    SWGL::window::c = c;
    SWGL::win.owr = r;

    int show = SW_SHOW;
    if (flag & 8) { show=SW_SHOWMAXIMIZED; } // MAXAMIZED_WINDOW flag
    else if (flag & 16) { show=SW_SHOWMINIMIZED; } // MINIMIZED_WINDOW flag

    if (flag & 1)  {  }; // DEBUG, not implemented trolololol
    if (flag & 4)  { SWGL::window::fullscreen(true);}; // Fullscreen
    if (flag & WINDOW_CENTRALIZED) { SWGL::window::centralize();} //Centralized

    ShowWindow(window, show);
    UpdateWindow(window);
    SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0); // We check for mouse input
}

bool check_hardware_info;
int SWGL::window::checkEvents(){
    event.key=0;
    win.old_menu=0;
    SWGL::window::event.button=0;
    event.type = SWGL::window::ProcMSG();

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hwnd, &cursorPos);
    SWGL::window::event.x = cursorPos.x;
    SWGL::window::event.y = cursorPos.y;

    SWGL::win = {SWGL::window::r, SWGL::window::c, SWGL::window::enabled_flags, SWGL::window::hwnd, SWGL::window::event.type, {SWGL::window::event.x, SWGL::window::event.y}, event.button, win.old_menu, SWGL::win.old_key, SWGL::win.check, SWGL::win.owr};

    if (SWGL::win.enabled_flags & 2) { // We check if xinput is enabled
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        XInputGetState(0, &state);

        SWGL::window::event.pad = {state.Gamepad.sThumbLX, state.Gamepad.sThumbLY}; // L-stick
        SWGL::window::event.r_pad = {state.Gamepad.sThumbRX, state.Gamepad.sThumbRY}; // R-stick
        SWGL::window::event.triggers[0] = state.Gamepad.bLeftTrigger;
        SWGL::window::event.triggers[1] = state.Gamepad.bRightTrigger;
    }
    if (SWGL::win.enabled_flags & 1) {createDebugTable();}


    glClearColor((float)c.r/255, (float)c.g/255, (float)c.b/255, float(c.a)/255); 
    glClear(GL_COLOR_BUFFER_BIT);
    
    Sleep(1);
    return 0;
}

int SWGL::window::clear() {
    glFlush();

    PAINTSTRUCT ps;
    EndPaint(SWGL::window::hwnd, &ps);
    return 0;
}

int SWGL::window::close() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(glrc);
    
    RSAL::quit();
    DestroyWindow(SWGL::window::hwnd);
    PostQuitMessage(0);
    return 0;
}

void SWGL::window::resize(bool value){
    if (value) SetWindowLong(SWGL::window::hwnd, GWL_STYLE, GetWindowLong(SWGL::window::hwnd, GWL_STYLE)|WS_SIZEBOX);
    else SetWindowLong(SWGL::window::hwnd, GWL_STYLE, GetWindowLong(SWGL::window::hwnd, GWL_STYLE)&~WS_SIZEBOX);
}

void SWGL::window::fullscreen(bool value) {
    DWORD style = GetWindowLong(SWGL::window::hwnd, GWL_STYLE);
    MONITORINFO mi = { sizeof(mi) };
	if (value) {
		RECT rect;
		GetWindowRect(SWGL::window::hwnd, &rect);
        SWGL::window::r = {rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
        SWGL::win.r = SWGL::window::r;

		GetMonitorInfo(MonitorFromWindow(SWGL::window::hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(SWGL::window::hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(SWGL::window::hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
	else {
		GetMonitorInfo(MonitorFromWindow(SWGL::window::hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(SWGL::window::hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPos(SWGL::window::hwnd, HWND_NOTOPMOST, SWGL::window::r.x, SWGL::window::r.y, SWGL::window::r.width, SWGL::window::r.length, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}
void SWGL::window::position(SWGL::rect window){ MoveWindow(SWGL::window::hwnd, window.x, window.y, window.width, window.length ,true); }
void SWGL::window::maximize(bool value) {
    int g;
    if (value) g = SW_SHOWMAXIMIZED;
    else g=SW_SHOWNORMAL;
    ShowWindow(SWGL::window::hwnd, g);
}

void SWGL::window::centralize() {
	MONITORINFO mi = { sizeof(mi) };

	GetMonitorInfo(MonitorFromWindow(SWGL::window::hwnd, MONITOR_DEFAULTTONEAREST), &mi);
	int x = (mi.rcMonitor.right - mi.rcMonitor.left - SWGL::window::r.width) / 2;
	int y = (mi.rcMonitor.bottom - mi.rcMonitor.top - SWGL::window::r.length) / 2;

	SetWindowPos(SWGL::window::hwnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

int SWGL::window::changeIcon(const char* filename) {
    if (std::string(filename).find(".exe") != std::string::npos) {
        HICON hIcon;
        ExtractIconEx(filename,0,&hIcon,NULL,3);
        SendMessage(SWGL::window::hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

        return 0;
    }
    else {
        HANDLE hIcon = LoadImage(NULL, filename, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
        if (hIcon) {
            SendMessage(SWGL::window::hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            return 0;
        }
    }
    return -1;
}

int SWGL::window::changeName(const char* name) { SetWindowText(SWGL::window::hwnd, name); return 0; }



bool SWGL::window::isPressed(int key, int port/*=0*/) {
    if (SWGL::win.enabled_flags & 2) {
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
    return ( win.type == SWGL::KeyPressed && GetKeyState(key) & 0x1000);
}

bool SWGL::window::isReleased(int key, int port/*=0*/) {
    if (SWGL::win.enabled_flags & 2) {
        /*Xinput needs to be implemented for isReleased.*/
    }
    int g = GetKeyState(key) & 0x1000; // We check if `key` is being pressed`
    if (g == 4096) { SWGL::win.old_key=key; return false; }  // If it is being pressed, set `SWGL::win.old_key` as `key` and return false
    else if (key==SWGL::win.old_key && g != 4096) {SWGL::win.old_key=695; return true;} // If `SWGL::win.old_key` is `key` but `key` isn't being pressed, then it means `key` was released recently. Set `SWGL::win.old_key` to anything to make sure it doesn't spam true
    return false;
}

bool SWGL::window::isClicked(int key, int port/*=0*/) {
    if (SWGL::win.enabled_flags & 2) {
        /*Xinput needs to be implemented for isClicked.*/
    }
    int g = GetKeyState(key) & 0x1000; 
    if (g == 4096 && key!=SWGL::win.old_key) { SWGL::win.old_key=key; return true; } 
    else if (g != 4096 && key==SWGL::win.old_key) {SWGL::win.old_key=635;}
    return false;
}

int SWGL::messageBox(std::string title, std::string message, int option/*=-1*/, UINT flags/*=0*/) {
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

    return MessageBox(SWGL::win.hwnd, message.c_str(), title.c_str(), opt);
}

std::string SWGL::fileDialog(const char* title, bool multiple/*=false*/, bool save/*=false*/, bool directory/*=false*/) {
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

std::string SWGL::getUsername() {
    char username[257];
    DWORD username_len = 257;
    GetUserName(username, &username_len);
    
    return (std::string)username;
}

int SWGL::getLanguage() {
    return PRIMARYLANGID(GetUserDefaultLCID());
}
std::string SWGL::getClipboardText() {
    OpenClipboard(nullptr);
    HANDLE hData = GetClipboardData(CF_TEXT);
    char* text = static_cast<char*>(GlobalLock(hData));
    std::string txt(text);
    GlobalUnlock(hData);
    CloseClipboard();

    return txt;
}

int SWGL::setClipboardText(std::string txt) {
    HANDLE global = GlobalAlloc(GMEM_FIXED, 32);
    memcpy(global, txt.c_str(), txt.size());

    OpenClipboard(SWGL::win.hwnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, global);
    CloseClipboard();

    return 0;
}

int fps_lasttime=0;
int fps_current; 
int fps_frames = 0; 
auto start = std::chrono::system_clock::now();

int SWGL::getFPS() {
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-start;
    fps_frames++;
    if (fps_lasttime < elapsed_seconds.count() - 1.0){
        fps_lasttime = elapsed_seconds.count();
        fps_current = fps_frames;
        fps_frames = 0;
    }
    return fps_current;
}

SWGL::debug SWGL::window::createDebugTable() {
    if (!check_hardware_info) {
        device_info info = SWGL::getGPUDeviceInfo();
        SWGL::window::debug.gpu = info.gpu;
        SWGL::window::debug.vram_available = info.vram;
        SWGL::window::debug.driver = info.driver_version;
        info = SWGL::getCPUDeviceInfo(true, false, false, false);
        SWGL::window::debug.cpu = info.cpu;
        info = SWGL::getOSDeviceInfo(true, false, false, false, false);
        SWGL::window::debug.os = info.os;
        check_hardware_info=true;
    }
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SWGL::window::debug.fps = SWGL::getFPS();
    SWGL::window::debug.vram_used =  pmc.PeakWorkingSetSize/1048576/2*100;

    return SWGL::window::debug;
}
int g_scrollY=0;
LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
        case WM_COMMAND:{
            SWGL::win.old_menu = LOWORD(param);
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

            SWGL::win.owr.x = -pos;

            return 0;
        }
        case WM_SIZE: {
            RECT rc = { 0 };
            GetClientRect(h, &rc);
		    if (!(SWGL::win.enabled_flags & 256))  glViewport(SWGL::win.r.x, SWGL::win.r.y, LOWORD(lparam), HIWORD(lparam));

            SWGL::win.r = {SWGL::win.r.x, SWGL::win.r.y, (int)LOWORD(lparam), (int)HIWORD(lparam)};
            SCROLLINFO si = { 0 };
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask = SIF_ALL;
            si.nMin = SWGL::win.owr.width;
            si.nMax = SWGL::win.owr.length-40;
            si.nPage = (rc.bottom - rc.top);
            si.nPos = SWGL::win.owr.x;
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

int SWGL::window::ProcMSG() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT:
            return SWGL::quit;

            case WM_KEYDOWN:
            SWGL::window::event.key = msg.wParam;
            return SWGL::KeyPressed;

            case WM_KEYUP:
            return SWGL::KeyReleased;

            case WM_MOUSEMOVE:
            return SWGL::MousePosChanged;

            case WM_LBUTTONDOWN:
            SWGL::window::event.button=1;
            return SWGL::MouseButtonPressed; 
            
            case WM_LBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_RBUTTONDOWN:
            SWGL::window::event.button=2;
            return SWGL::MouseButtonPressed;

            case WM_RBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_MBUTTONDOWN:
            SWGL::window::event.button=3;
            return SWGL::MouseButtonPressed;

            case WM_MBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_NCMOUSEMOVE:
            return SWGL::MousePosChanged;

            case WM_NCLBUTTONDOWN:
            SWGL::window::event.button=1;
            return SWGL::MouseButtonPressed;
            
            case WM_NCLBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_NCRBUTTONDOWN:
            SWGL::window::event.button=2;
            return SWGL::MouseButtonPressed;

            case WM_NCRBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_NCMBUTTONDOWN:
            SWGL::window::event.button=3;
            return SWGL::MouseButtonPressed;

            case WM_NCMBUTTONUP:
            return SWGL::MouseButtonReleased;

            case WM_MOUSEWHEEL: 
            SWGL::window::event.button=16-win.check;
            if (SWGL::win.check == 11 || SWGL::win.check == 12) return SWGL::MouseWheelScrolling;
                
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
        if (zdelta > 0) SWGL::win.check= 12;
        else SWGL::win.check = 11;
    }
  }
  return CallNextHookEx(0, nCode, wParam, lParam);
}

namespace SWGL{SWGL::drawable win=win;};