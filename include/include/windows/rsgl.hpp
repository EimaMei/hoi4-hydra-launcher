#pragma once
#ifndef WIN_NOEXCEPT
#define WIN_NOEXCEPT
#endif

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <windows.h>
#include <psapi.h>
#include <shlobj.h>
#include <vector>
#include <chrono>
#include "deps/win32/win32_SetProcessDpiAware.h"
#include "deps/win32/no_sal2.h"
#include "deps/win32/Xinput.h"
#include "deps/DiscordSDK/discord.h"
#include "deps/gl.h" // OpenGL Rendering 
#include "rsal.hpp"
#include <cmath>
#define BYTESTOGB(memory) round((double)memory/1073741824)  // Converts bytes to GB and then rounds it up. Used for converting RAM bytes into GBs
#define BYTESTOMB(memory) round((long long)memory/1048576) // Converts bytes to MB and then rounds it up. Used for converting VRAM bytes into MBs.
#define RSGLRGBTOHEX(r, g, b) ((r << 16) + (g << 8) + b) // Convert RGB to hex
#define RSGLHEXTORGB(hex) { (hex>>16&0xFF), (hex>>8&0xFF), (hex&0xFF) }

/* Important Windows functions */
std::string wintest();
LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

namespace RSGL{ /* It all starts with this, the namespace containing the Ultimate Power.*/
    using namespace RSAL; /* Include RSAL to RSGL.*/
    const int KeyPressed=2; // a key has been pressed
    const int KeyReleased=3; // a key has been released
    const int MouseButtonPressed=4; // a mouse button has been pressed (left,middle,right)
    const int MouseButtonReleased=5; // a mouse button has been released (left,middle,right)
    const int MousePosChanged=6; // the position of the mouse has been changed
    const int quit = 33; // the user clicked the quit button
    const int dnd = 34; // a file has been dropped into the window
    const int MouseWheelScrolling = 11; /* Event type, indicating the mousewheel is scrolling up*/

    struct rect{ int x, y; int width, length; }; /*Rectangle structure.*/
    struct point{ int x, y;}; /* Point structure.*/
    struct circle { int x, y; int radius; }; /*Circle structure.*/
    struct triangle {  int x, y; int width, length; }; /*Triangle structure.*/
    struct color { int r, g, b; int a=255; }; /*Color structure in RGBA.*/
    struct image { 
        const char* filename; 
        rect r; 

        private: GLuint tex; unsigned char * data; point original_size; bool loaded=false;
        public:
        /* Loads the image.*/
        image(const char * Filename, rect R, bool load=true);
        void load();
        /* Draws the image.*/
        int draw();
        /* Gets the pixel color in RGB.*/
        color getPixelColor(point pos, bool og_size=false);

    }; /*Image structure.*/
    /*struct pixels {
        
        point r;
        GLuint tex=-1;
        image(const char * Filename, rect R, std::vector<RSGL::color> pixels={});
        int draw();
    }; /*pixels structure.*/
    struct text{
      RSGL::circle rect;
      RSGL::color c; 
      std::string text; 
      const char* f;
    }; /*Text structure.*/
    struct resolution {int x,y,hz;};
    struct device_info {
        std::string os, model, name, serial_number; 
        int hdd_total, hdd_used, hdd_free;
        int ram_total, ram_used, ram_free, ram_percentage; 
        std::string cpu; int cpu_cores, cpu_percentage;  std::string cpu_architecture;
        std::string gpu, driver_version; int vram; 
        int architecture;
    }; /* The device information structure. */
    struct timer { 
        int h, min, s, ms; 
        std::chrono::time_point<std::chrono::_V2::system_clock> id;
        double past;
    }; /* The timer structure.*/
    struct date {
        int year, month, day;
        int hour, minute, seconds;
    }; /* The date structure.*/
    struct debug {
        int fps, vram_used, vram_available;
        std::string os, cpu, gpu, driver;
    }; /*The debug table structure.*/
    enum shape{RECTANGLE, TRIANGLE, CIRCLE, IMAGE}; /* Defines what the button is (a rectangle, triangle etc.)*/
    struct button {
        /* Checks if the button has been mouse clicked.*/
        bool clicked();
        /* Checks if the mouse is hovering the button.*/
        bool hovered();
        /* Checks if the button is being held/pressed.*/
        bool held(); bool pressed();
        /* Checks if the mouse was released from the button.*/
        bool released();

        int mouse=0; /* Checks which mouse button clicked the button.*/
        shape ID;

        private: bool m=false; RSGL::rect rr; RSGL::circle cirr; color cc; RSGL::image i={"", {}};
        public:
        /* Init the button.*/
        button(shape id, rect r, color c) { rr={r.x, r.y, r.width, r.length}; cc=c;ID=id; }; 
        button(shape id, circle cir, color c){cirr=cir; cc=c;ID=id;};
        button(shape id, const char* filename, rect r) { rr=r; ID=id; i.filename=filename; i.r=r; i.load(); }; 
        /* Draws the button.*/
        void draw(RSGL::rect r={});
        /* Load the image.*/
        void load(const char* filename, rect r, bool load_regardless=false);
    }; /* Button structure.*/
    struct menu{
        std::vector<int> id_list;
    
        menu addSection(const char* text);
        int removeMenuSection(const char* text); int removeMenuSection(int id);
        bool clickedSection(const char* text); bool clickedSection(int id);
        
        HMENU sMenubar;
        std::vector<std::pair<const char*, int>> list;
        int count=1;
        HWND window;
    }; /* Menubar structure.*/
    struct drawable {rect r; color c; uint32_t enabled_flags; HWND hwnd; int type; point mouse; int button; int old_menu; int old_key; int check; rect owr;}; /* Window information*/
    
    struct window : drawable { /* The `window` structure. Stores the important information about the window and program.*/
        public:
            const char* name; /* Name of the window.*/
            rect r;
            color c; /* RGB color of the window.*/
            uint32_t enabled_flags; /* Checks the windows flags that are enabled.*/
            HWND hwnd; /* The window */

            struct Event {
                int x; /*X position of the mouse.*/
                int y; /*Y position of the mouse.*/
                int type; /* Event type variable, telling what and which action happened.*/
                int button; /* Value indicating which mouse button was used.*/
                point pad; /*The X and Y positions of the L-stick.*/ 
                point r_pad; /*The X and Y positions of the R-stick.*/
                int triggers[2] = {0,0}; /*The values of the trigger buttons from 0 to 255. `trigger[0]` is LT, while `trigger[1]` is RT.*/
                int key; /* The current pressed key.*/
            };
            Event event; // The Event variable.
            RSGL::debug debug;

            /* Inits and creates a window.*/
            window(const char* name, rect r, color c, uint32_t flag=0);
            /* Updates everything what happened this exact frame. Required in every RSGL project.*/
            int checkEvents();
            /* Refreshes the screen. Reccomended to put at the end of the loop as you cannot draw on the screen after using this function before it jumps back to the start of the loop.*/
            int clear();
            /* Quits the program.*/
            int close();
            /* Creates a debug table for debugging purposes*/
            RSGL::debug createDebugTable();
            /*Adds a flag to the window if to make it resizable or not depending on the `value` parem.*/
            void resize(bool value);
            /*Adds a flag to the window if to make it fullscreen or not depending on the `value` parem.*/
            void fullscreen(bool value);
            /* Adds a flag to the window if to maximize the window.*/
            void maximize(bool value);
            /*Adds a flag to the window to change the resolution and position of the window.*/
            void position(rect window);
            /*Adds a flag to the window to make the window centralized depending on the width and height.*/
            void centralize();
            /* Changes the Window's icon.*/
            int changeIcon(const char* filename);
            /* Changes the Window's name.*/
            int changeName(const char* name);
            /* */
            int createScrollbar(int min, int max);
            /* */
            menu createMenuBar(const char* text);
            /* */
            int createCheckbox(point r);
    
            /*Checks if a key is being pressed If so return true, else return false.*/
            bool isPressed(int key, int port=0);
            /*Checks if a key is being clicked __AT THIS EXACT FRAME__. If so return true, else return false.*/
            bool isClicked(int key, int port=0);
            /*Checks if a key was released this frame. If so return true, else return false.*/
            bool isReleased(int key, int port=0);
        private: 
            int ProcMSG();
    };
    extern drawable win;

    // ============ DRAWING FUNCTIONS ============
    /*Draws a rectangle. Parameter `solid` is always set to true.*/
    int drawRect(rect r, color c, bool solid=true, bool dotted=false, int border_size=3, RSGL::color c2={1991}, bool uptodown=false); 
    /*Draws a triangle. Parameter `solid` is always set to true.*/
    int drawTriangle(triangle t, color c, bool solid=true);
    /*Draws a circle. Parameter `solid` is always set to true.*/
    int drawCircle(circle c, color col, bool solid=true, int border_size=3);
    /* Loads the image into memory.*/
    image loadImage(image r, std::vector<RSGL::color> pixels={}); image loadImage(const char* file, rect r, std::vector<RSGL::color> pixels={}); 
    /* Draw pixels.*/
    int drawPixels(rect r, std::vector<color> pixels); int drawPixels(rect r, std::vector<unsigned int> pixels);
    /* Draws the text.*/
    int drawText(std::string text, RSGL::rect r, const char* font, RSGL::color col);
    /*Draws a line (rectangle). Parameter `solid` is always set to true.*/
    #define drawLine drawRect

    // ============ Collide Functions ============
    /*Circle coliding with point.*/
    int CircleCollidePoint(RSGL::circle c, RSGL::point p);
    /*Circle coliding with rect.*/
    int CircleCollideRect(RSGL::circle c, RSGL::rect r);
    /*Circle coliding with another circle.*/
    int CircleCollideCircle(RSGL::circle cir1,RSGL::circle cir2);
    /*Rect coliding with point.*/
    int RectCollidePoint(RSGL::rect r, RSGL::point p);
    /*Rect coliding with another rect.*/
    int RectCollideRect(RSGL::rect r, RSGL::rect r2);
    /*Point coliding with another point.*/
    int PointCollidePoint(RSGL::point p, RSGL::point p2);
    /*Image coliding with rect.*/
    int ImageCollideRect(RSGL::image img, RSGL::rect r);
    /*Image coliding with circle.*/
    int ImageCollideCircle(RSGL::image img, RSGL::circle c);
    /*Image coliding with point.*/
    int ImageCollidePoint(RSGL::image img, RSGL::point p);
    /*Image coliding with another image.*/
    int ImageCollideImage(RSGL::image img, RSGL::image img2);

    /* ========== .INI Functions ========== */
    // Reads a key from an settings.ini
    std::string readIniKey(const char* path, std::string Key); 
    // Writes a key from settings.ini
    int writeIniKey(const char* path, std::string Key, std::string value); 
    // Deletes a key from settings.ini
    int deleteIniKey(const char* path, std::string Key);
    // Deletes a section from settings.ini
    int deleteIniSection(const char* path); 
    // Checks if a key exists in settings.ini
    bool iniKeyExists(const char* path, std::string Key);

    // ========== Rich Discord Presence Functions ==========
    /* Inits the Discord Presence by getting the application ID. Parameter `timer` is set to false by default.*/
    int discordCreateApplication(discord::ClientId clientID, bool timer=false);
    /* Updates the Discord Presence. All paremeters are set to nothing by default.*/
    int discordUpdatePresence(const char* details="", const char* state="", const char* largeImage="", const char* largeText="", const char* smallImage="", const char* smallText="");

    // ========== Device Information ==========
    /* Gets the important information of the PC. Note that when using this function, it takes at least 2 seconds to load up everything. Load time will vary depending on hardware, so load this only once.*/
    device_info getFullDeviceInfo();
    /* Gets only the RAM information.*/
    device_info getRAMDeviceInfo(bool total=true, bool available=true, bool used=true, bool percentage=true);
    /* Gets only the HDD information.*/
    device_info getHDDDeviceInfo(bool total=true, bool free=true, bool used=true);
    /* Gets only the OS information.*/
    device_info getOSDeviceInfo(bool os=true, bool model=true, bool pc_name=true, bool bit=true, bool serial_number=true);
    /* Gets only the GPU information.*/
    device_info getGPUDeviceInfo(bool gpu=true, bool driver_version=true, bool vram=true);
    /* Gets only the CPU information. This is the most time-consuming info function.*/
    device_info getCPUDeviceInfo(bool cpu=true, bool cores=true, bool percentage=true, bool architecture=true);
    /* Returns the current desktop screen resolution.*/
    RSGL::resolution getScreenResolution();
    /* Returns a vector list of resolutions (and the frequency) that the monitor/computer supports. */
    std::vector<RSGL::resolution> getAvailableResolutions();

    // ============ Misc Functions ============
    /* Gets the username of the user and returns it as a string.*/
    std::string getUsername();
    /* Gets the language code of the user.*/
    int getLanguage();
    /* Gets the local time of the user and outputs the result in a `date` structure.*/
    date getCurrentTime();
    /* Gets the user's clipboard text and returns it as a string.*/
    std::string getClipboardText();
    /* Sets the user's clipboard text to parameter `txt`.*/
    int setClipboardText(std::string txt);
    /* Opens a file/folder dialog.
    When using the `multiple` parameter, the function will output "<directory> <file 1> <file 2> <file 3> (etc.)".
    Example for title - "All\0*.*\0Text\0*.TXT\0".*/
    std::string fileDialog(const char* title, bool multiple=false, bool save=false, bool directory=false);
    /*Opens a message box and returns a value, indicating which button the user clicked.
    `option` parameter list:
    0  - Error
    1  - Warning
    2  - Question
    `flags` parameter list:
    OK
    HELP
    OKCANCEL
    RETRYCANCEL
    YESNOCANCEL
    YESNO
    CANCELTRYCONTINUE*/
    int messageBox(std::string title, std::string message, int option=-1, UINT flags=0);
    /* Gets the current FPS of the program.*/
    int getFPS();
    /* Gets the numbers after the decimal point (eg. function will output 9 if `num` is something like 1.9)*/
    int findNumberAfterDecimalPoint(float num, int howManyKeep=-1);
    /* Turns a color to grayscale.*/
    color setGrayscale(color c);
    /* Inverts the color.*/
    color setInverted(color c);
    /* Clears the console*/
    int clearConsole();
    /* Returns the distance between two objects.*/
    point checkDistance(RSGL::circle c, RSGL::point p);
    /* Returns the pixel distance.*/
    point checkDistance(RSGL::circle c, RSGL::rect r); point checkDistance(RSGL::circle cir1,RSGL::circle cir2); point checkDistance(RSGL::rect r, RSGL::point p); point checkDistance(RSGL::rect r, RSGL::rect r2); point checkDistance(RSGL::point p, RSGL::point p2); point checkDistance(RSGL::image img, RSGL::rect r); point checkDistance(RSGL::image img, RSGL::circle c); point checkDistance(RSGL::image img, RSGL::point p); point checkDistance(RSGL::image img, RSGL::image img2);
    
    // ============ Timer Functions ============
    /* Starts a timer.*/
    timer startTimer();
    /* Updates the timer.*/
    int updateTimer(timer& t, int howManyKeep=2);

    // ============ Xinput Functions  ============
    /* Check if a controller is connected. Parameter `port` is 0 by default, meaning it'll check if the 1st controller is connected.*/
    bool xinputConnected(int port=0);
    /* Vibrates the controller. Parameter `port` is 0 by default, parameters `leftVal` and `rightVal` are 65535 by default.*/
    int xinputVibrate(int leftVal=65535, int rightVal=65535, int port=0);
    /* Returns the LT and RT values in a `trigger` struct (x and y) from 0-255. Parameter `port` is 0 by default.*/
    point xinputGetTriggerValues(int port=0);
    /* Returns the battery values in a `battery` struct (x and y). Parameter `port` is 0 by default.
    X returns:
    0 - The controller isn't connected.
    1 - The controller is wired.
    2 - The controller uses an alkaline battery.
    3 - The controller uses a nickel metal hydride battery. 
    4 - The controller uses an unknown battery type.
    Y returns:
    0 - Battery level is empty.
    1 - Battery level is low.
    2 - Battery level is medium.
    3 - Battery level is full.
    */
    point xinputGetBattery(int port=0);
}