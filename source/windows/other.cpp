#ifndef RSGL
#include "../../include/include/windows/rsgl.hpp"
#endif

DiscordState discord_state;
int64_t duration;

int RSGL::discordCreateApplication(discord::ClientId clientID, bool timer/*=false*/) {
    discord::Core* core{}; 
    discord::Core::Create(clientID, DiscordCreateFlags_Default, &core); // Get the application from the `clientID`
    discord_state.core.reset(core);
    core->UserManager().OnCurrentUserUpdate.Connect([]() {
        discord_state.core->UserManager().GetCurrentUser(&discord_state.currentUser); // Get the discord user
    });
    if (timer) {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
        duration = value.count();
    }
    return 0;
}

int RSGL::discordUpdatePresence(const char* details/*="*/, const char* state/*="*/, const char* largeImage/*="*/, const char* largeText/*="*/, const char* smallImage/*="*/, const char* smallText/*=""*/) {
    discord::Activity activity{};
    if (strcmp(details, "")) activity.SetDetails(details); // We check if the parameters are empty or nah
    if (strcmp(state, "")) activity.SetState(state);
    if (strcmp(largeImage, "")) activity.GetAssets().SetLargeImage(largeImage);
    if (strcmp(largeText, "")) activity.GetAssets().SetLargeText(largeText);
    if (strcmp(smallImage, "")) activity.GetAssets().SetSmallImage(smallImage);
    if (strcmp(smallText, "")) activity.GetAssets().SetSmallText(smallText);
    if (duration != -1) activity.GetTimestamps().SetStart(duration);
    activity.SetType(discord::ActivityType::Playing); // We set the type to g a m i n g

    discord_state.core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {}); // Update
    discord_state.core->RunCallbacks(); // Run it back
    return 0;
}

std::string RSGL::readIniKey(const char* path, std::string Key) {
    char szValue[MAX_PATH];
    char fullFilename[MAX_PATH];
    GetFullPathName(path, MAX_PATH, fullFilename, nullptr);
    
    GetPrivateProfileString(NULL, Key.c_str(), TEXT("ERROR"), szValue, MAX_PATH, fullFilename);
    if ( strcmp(szValue,"ERROR") != 0) { return szValue; }
    return "";
}
int RSGL::writeIniKey(const char* path, std::string Key, std::string value) {
    char fullFilename[MAX_PATH];
    GetFullPathName(path, MAX_PATH, fullFilename, nullptr);
    WritePrivateProfileString (NULL, Key.c_str(), value.c_str(), fullFilename);
    return 0;
}

int RSGL::deleteIniKey(const char* path, std::string Key) {
    char fullFilename[MAX_PATH];
    GetFullPathName(path, MAX_PATH, fullFilename, nullptr);
    WritePrivateProfileString (NULL, Key.c_str(), NULL, fullFilename);
    return 0;
}

int RSGL::deleteIniSection(const char* path) {
    char fullFilename[MAX_PATH];
    GetFullPathName(path, MAX_PATH, fullFilename, nullptr);
    WritePrivateProfileString (NULL, NULL, NULL, fullFilename);
    return 0;
}

bool RSGL::iniKeyExists(const char* path, std::string Key) {
    return readIniKey(path, Key).size() > 0;
}

int RSGL::findNumberAfterDecimalPoint(float num, int howManyKeep/*=-1*/) {
    std::string s;
    std::stringstream out;
    out << num; s = out.str();
    s = s.substr(s.find(".")+1, -1);
    if (howManyKeep != -1) s = s.substr(0, howManyKeep);
    return std::stoi(s);
}

RSGL::timer RSGL::startTimer() {
    auto start = std::chrono::system_clock::now();
    return {0,0,0,0,start};
}

int RSGL::updateTimer(RSGL::timer& t, int howManyKeep/*=2*/) {
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-t.id;
    int h=0,
        m=0,
        s=0,
        ms=0;
    double time =  elapsed_seconds.count(); 
    ms=RSGL::findNumberAfterDecimalPoint(time, howManyKeep);
    s=time;
    if (m >= 60) { h=m/60;}
    if (time >= 60.0) { m=abs(s)/60; }

    t = {h, m, s-(m*60), ms,t.id};

    return 0;
}

RSGL::date RSGL::getCurrentTime() {
    time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm utc_tm = *localtime(&tt);
    return {utc_tm.tm_year + 1900, utc_tm.tm_mon + 1,utc_tm.tm_mday, utc_tm.tm_hour,utc_tm.tm_min, utc_tm.tm_sec};
}

RSGL::resolution RSGL::getScreenResolution() {
    HMONITOR monitor = MonitorFromWindow(RSGL::win.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    DEVMODE dev;
    dev.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, 0, &dev);
       
    return {info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top, (int)dev.dmDisplayFrequency};
}

std::vector<RSGL::resolution> RSGL::getAvailableResolutions() {
    DEVMODE dev;
    dev.dmSize = sizeof(DEVMODE);
    int i=0;
    std::vector<RSGL::resolution> list= { {0,0,0} };
    while (EnumDisplaySettings(NULL, i, &dev)) {
        if (list.begin()->x == (int)dev.dmPelsWidth && list.begin()->y == (int)dev.dmPelsHeight)  { }
        else list.insert(list.begin(), {(int)dev.dmPelsWidth, (int)dev.dmPelsHeight, (int)dev.dmDisplayFrequency}); 
        i++;
    }
    list.pop_back();
    return list;
}

bool RSGL::button::clicked() {
    if (RSGL::win.type == RSGL::MouseButtonPressed && RSGL::RectCollidePoint({rr.x, rr.y, rr.width, rr.length }, {win.mouse.x, win.mouse.y})) { mouse=win.button; m=true; return true;}
    mouse=0;
    return false;
}

bool RSGL::button::hovered() {
    if (RSGL::RectCollidePoint(rr, {win.mouse.x, win.mouse.y})) return true;
    return false;
}

bool RSGL::button::released() {
    if (RSGL::win.type == RSGL::MouseButtonReleased && (RSGL::RectCollidePoint(rr, {win.mouse.x, win.mouse.y}) || m==false) ) { mouse=0; return true;}
    mouse=0;
    return false;
}

bool RSGL::button::pressed() { return held(); } 
bool RSGL::button::held() {
    clicked();
    if (m==true && RSGL::win.type != RSGL::MouseButtonReleased) { return true;}
    if (RSGL::win.type == RSGL::MouseButtonReleased) m=false;
    return false;
}

RSGL::point checkDistance(RSGL::circle c, RSGL::rect r) {return {};}
RSGL::point checkDistance(RSGL::circle cir1,RSGL::circle cir2) {return {};}
RSGL::point checkDistance(RSGL::rect r, RSGL::point p) {return {p.x-r.x+r.width, p.y-r.y+r.length};}
RSGL::point checkDistance(RSGL::rect r, RSGL::rect r2) {return {r2.x+r2.width-r.x+r.width, r2.y+r2.length-r.y+r.length};}
RSGL::point checkDistance(RSGL::point p, RSGL::point p2) {return {p.x-p2.x, p.y-p2.y};}
RSGL::point checkDistance(RSGL::image img, RSGL::rect r) {return {img.r.x+img.r.width-r.x+r.width, img.r.y+img.r.length-r.x+r.length};}
RSGL::point checkDistance(RSGL::image img, RSGL::circle cir) {return {};}
RSGL::point checkDistance(RSGL::image img, RSGL::point p) {return {img.r.x+img.r.width-p.x, img.r.y+img.r.length-p.y};}
RSGL::point checkDistance(RSGL::image img, RSGL::image img2) {return {img.r.x+img.r.width-img2.r.x+img2.r.width, img.r.y+img.r.length-img2.r.x+img2.r.length};}