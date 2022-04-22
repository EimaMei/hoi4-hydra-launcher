#ifndef SWGL
#include "../../SWGL.hpp"
#endif

HMENU hMenubar={0};
int l_num=1;

std::string SWGL::getIPv4() {
    char buff[512];
    FILE*  fp = popen ("curl http://ip.jsontest.com/ 2> nul", "r");
    fread(buff,sizeof(buff),1,fp); pclose(fp); 
    std::string m(buff); 

    return m.substr(m.find("{\"ip\": \""));
}

SWGL::menu SWGL::window::createMenuBar(const char* text) { /* Should later be converted to SWGL::window::createMenuBar*/
    if (hMenubar == 0) hMenubar=CreateMenu();
    menu mm;
    mm.sMenubar=CreateMenu();
    mm.window = SWGL::window::hwnd;

    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)mm.sMenubar, text);
    SetMenu(mm.window, hMenubar);

    return mm;
}

SWGL::menu SWGL::menu::addSection(const char *text) {
    list.insert(list.end(), {text, l_num});
    id_list.insert(id_list.end(), count);
    AppendMenu(sMenubar, MF_STRING, l_num, text);
    
    SetMenu(window, hMenubar);
    l_num++; count++;
    
    return { id_list, sMenubar, list, count, window};
}

/*int menu::removeMenuSection(const char* text) {
    for (int i=list.size()-1; i>=0; i--) {
        if (list[i].first == text) {
            RemoveMenu(hMenubar, list[i].second, MF_BYCOMMAND);
            SetMenu(menu::window, hMenubar);
            list.erase(list.begin()+(i-1), list.end()-(list.size()-i));
            break;
        }
    }

    return 0;
}

int menu::removeMenuSection(int id) {
    for (int i=0; i<list.size(); i++) {
        if (list[i].second == id) {
            RemoveMenu(hMenubar, list[i].second, MF_BYCOMMAND);
            SetMenu(menu::window, hMenubar);
            list.erase(list.begin()+(i-1), list.end()-(list.size()-i));
            break;
        }
    }

    return 0;
}*/


bool SWGL::menu::clickedSection(int id) {
    if (SWGL::win.old_menu != 0) { 
        for (int i=0; i<list.size(); i++) {
            if (id_list[i] == id && list[i].second == SWGL::win.old_menu) { SWGL::win.old_menu=0; return true;}
        }
    }
    return false;
}

struct colorPicker {
    /* Pick the color.*/
    SWGL::color pickColor();
    /* Change the current colorPicker color to something new.*/
    int color(SWGL::color c);
    /* Change the current colorPicker color to something new.*/
    int position(SWGL::rect r); int position(SWGL::circle cir);

    private: SWGL::rect rr; SWGL::circle cirr; SWGL::color c; SWGL::shape ID;
    public:
    colorPicker(SWGL::shape id, SWGL::rect r, SWGL::color c){rr=r; c=colorPicker::c;ID=id;}; 
    colorPicker(SWGL::shape id, SWGL::circle cir, SWGL::color c){cirr=cir; c=colorPicker::c;ID=id;}; 

    void draw();
};

SWGL::color colorPicker::pickColor() {
    CHOOSECOLOR ccolor;                 // common dialog box structure 
    static COLORREF acrCustClr[16]; // array of custom colors 
    int col;
    
    // Initialize CHOOSECOLOR 
    ZeroMemory(&ccolor, sizeof(ccolor));
    ccolor.lStructSize = sizeof(ccolor);
    ccolor.lpCustColors = (LPDWORD) acrCustClr;
    ccolor.rgbResult = RSGLRGBTOHEX(c.b, c.g, c.r);
    ccolor.Flags = CC_FULLOPEN | CC_RGBINIT;
     
    if (ChooseColor(&ccolor)==TRUE) { col = ccolor.rgbResult; } 
    c = { (col & 0xFF), ((col >> 8) & 0xFF), ((col >> 16) & 0xFF)};
    return c;
}

int colorPicker::color(SWGL::color c) {colorPicker::c=c; return 0;};
int colorPicker::position(SWGL::rect r) {rr=r; return 0;} int colorPicker::position(SWGL::circle cir) {cirr=cir; return 0;}

void colorPicker::draw() {
    switch (ID) {
        case SWGL::RECTANGLE: SWGL::drawRect(rr, colorPicker::c); break;
        case SWGL::TRIANGLE: {SWGL::triangle f ={rr.x, rr.y, rr.width, rr.length}; SWGL::drawTriangle(f, colorPicker::c); break;}
        case SWGL::CIRCLE: SWGL::drawCircle(cirr, colorPicker::c); break;
        default: break;
    }
}

/*el_num types { RGB, RGBA, BGR, BGRA, ARGB, ABGR, HEX};
std::vector<unsigned int> dumpImage(const char* filename, bool print=true, types version=HEX, types type=BGRA, const char* output="") {
    std::vector<unsigned int> img;
    int w,h;
    png::image< png::rgba_pixel > image(filename);
    w=image.get_width(); h=image.get_height();
    if (print || output != "") {
        std::string s = filename;

        std::replace(s.begin(), s.end(), ' ', '_');
        std::replace(s.begin(), s.end(), '.', '_');

        std::string format="unsigned int";
        if (version == RGB) format="SWGL::color";

        if (print) {
            std::cout << 
            "int "+s+"_width = "+std::to_string(w)+";\n" <<
            "int "+s+"_length = "+std::to_string(h)+";\n" <<
            "std::vector<"<<format<<"> " << s << " = {";
        }
        if (output != "") {
            std::ofstream header(output);
            std::replace(s.begin(), s.end(), ' ', '_');
            std::replace(s.begin(), s.end(), '.', '_');
            header << "#include <vector>\n\nint " << s << "_width = " << w << ";\nint " << s << "_length = " << std::to_string(h) << ";\nstd::vector<"<<format<<"> " << s << " = {\n    ";
            header.close();
        }
    }
    std::string msg;

    for (png::uint_16 y=0; y < h; y++){  
        for (png::uint_16 x=0; x < w; x++) {
            unsigned int res;
            switch (type) {
                case RGB: res=RSGLRGBTOHEX(image[y][x].red, image[y][x].green, image[y][x].blue); break;
                case RGBA: res=RSGLRGBTOHEX(image[y][x].red, image[y][x].green, image[y][x].blue)*0x1000000+image[y][x].alpha; break;
                case BGR: res=RSGLRGBTOHEX(image[y][x].blue, image[y][x].green, image[y][x].red); break;
                case BGRA: res=RSGLRGBTOHEX(image[y][x].blue, image[y][x].green, image[y][x].red)*0x1000000+image[y][x].alpha; break;
                case ARGB: res=0x1000000*image[y][x].alpha+RSGLRGBTOHEX(image[y][x].red, image[y][x].green, image[y][x].blue); break;
                case ABGR: res=0x1000000*image[y][x].alpha+RSGLRGBTOHEX(image[y][x].blue, image[y][x].green, image[y][x].red); break;
            }
            img.insert(img.end(), res);
            if (print||output!="") {
                if (version == HEX) {
                    std::stringstream stream;
                    stream << "0x"  << std::setfill ('0') << std::setw(sizeof(int)*2) << std::hex << img.back();
                    if (y == h-1 && x == w-1) msg += stream.str()+"\n};";
                    else msg+= stream.str()+", ";
                }
                else if (version == RGB) {
                        SWGL::color c = {image[y][x].red, image[y][x].green, image[y][x].blue, 255};
                        switch (type) {
                            case RGB: c = {image[y][x].red, image[y][x].green, image[y][x].blue, 255}; break;
                            case RGBA: c = {image[y][x].red, image[y][x].green, image[y][x].blue, image[y][x].alpha}; break;
                            case BGR: c = {image[y][x].blue, image[y][x].green, image[y][x].red}; break;
                            case BGRA: c = {image[y][x].blue, image[y][x].green, image[y][x].red, image[y][x].alpha}; break;
                            case ARGB: c = {image[y][x].alpha, image[y][x].red, image[y][x].green, image[y][x].blue}; break;
                            case ABGR: c = {image[y][x].alpha, image[y][x].blue, image[y][x].green, image[y][x].red}; break;
                        }
                        if (y == h-1 && x == w-1) msg += "{"+std::to_string(c.r) +","+std::to_string(c.g)+","+std::to_string(c.b)+","+std::to_string(c.a)+"} };"; 
                        else msg += "{"+std::to_string(c.r) +","+std::to_string(c.g)+","+std::to_string(c.b)+","+std::to_string(c.a)+"},"; 
                    }
                }
            }
        }
    if (print) std::cout << msg << std::endl;
    if (output!="") {
        std::ofstream header(output, std::fstream::out | std::fstream::app);
        header << msg;
        header.close();
    }
    return img;
}*/


/*int SWGL::window::createScrollbar(int min, int max) {
    RECT rc = { 0 };
    GetClientRect(win.hwnd, &rc);
    SCROLLINFO si = { 0 };
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    si.nMin = min;
    si.nMax =  max;
    si.nPage = (rc.bottom - rc.top);
    si.nPos = 0;
    si.nTrackPos = 0;
    SetScrollInfo(win.hwnd, SB_VERT, &si, true);
    SWGL::win.owr.width = min;
    SWGL::win.owr.length = max;
    MoveWindow(SWGL::window::hwnd, SWGL::window::r.x, SWGL::window::r.y, SWGL::window::r.width+1, SWGL::window::r.length, true);
    MoveWindow(SWGL::window::hwnd, SWGL::window::r.x, SWGL::window::r.y, SWGL::window::r.width-1, SWGL::window::r.length, true);

    return 0;
}

int SWGL::window::createCheckbox(SWGL::point r) {
    CreateWindowW(L"Button",NULL,WS_VISIBLE|WS_CHILD |BS_CHECKBOX,r.x, r.y,19,19,SWGL::window::hwnd, (HMENU) 1,GetModuleHandle(nullptr),NULL);
    CheckDlgButton(hwnd, 1, BST_CHECKED);
        
    l_num++;
    return 0;
}*/

SWGL::color SWGL::image::getPixelColor(SWGL::point pos, bool og_size/*=false*/) {
    if ( pos.x+pos.y == 0 || pos.x > r.width || pos.y > r.length ) return {-1,-1,-1};

    if (og_size || r.width+r.length == original_size.x+original_size.y) {
        std::vector<unsigned char> image = std::vector<unsigned char>(data, data + original_size.x * original_size.y * 4);
        size_t index = 4 * (pos.y * original_size.x + pos.x);

        return { static_cast<int>(image[index + 0]),  static_cast<int>(image[index + 1]),  static_cast<int>(image[index + 2]),  static_cast<int>(image[index + 3])};
    } 
    
    return {-1,-1,-1};
}

std::string getCurrentPath() {
    char filename[1024];
    FILE* f = popen("cd", "r");
    fgets(filename, 1024, f);
    fclose(f);

    return (std::string)filename;
}