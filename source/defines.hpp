#ifndef RSGL
#include "../RSGL.hpp"
#endif
bool running=true;
#define defs 1 // Make sure that this header was read.
struct tags {
    bool alternative_history=false;
    bool map=false;
    bool events=false;
    bool national_focuses=false;
    bool fixes=false;
    bool sound=false;
    bool gameplay=false;
    bool technologies=false;
    bool graphics=false;
    bool translation=false;
    bool historical=false;
    bool utilities=false;
    bool ideologies=false;

    bool edited=false;
}; /* Tags strcture. Could've made it shorter but then it would be practically unreadable and way harder to manage.*/

struct mod {
    std::string name; /* Name of the mod.*/
    long long id=0; /*Workshop mod ID. Can't use int as some of the IDs are 'long long' and not 'int'. */
    bool workshop=false; /* If the mod is a workshop mod*/
    std::string path; /* Directory of the mod.*/
    std::string version; /* Version of the mod.*/
    std::string supported_version; /* Supported version.*/
    tags tag_list; /* The enabled tags.*/
}; /* The mod structure.*/

/* Functions*/
void checkButtons();
void drawImages();
void drawRects();
void debugmode(int event);
std::vector<mod> returnModList();

RSGL::rect rr = {830,71,148,36}; // Debug element, used for positioning buttons.
RSGL::circle mm = {693,521,50}; // Debug element, used for positioning circular objects.

/* Resolutions.*/
std::vector<RSGL::resolution> resolutions = RSGL::getAvailableResolutions(); /* Get all available resolutions. Used for creating the window and 'Games settings->Fullscreen resolution'.*/
RSGL::resolution current_res = RSGL::getScreenResolution(); /* Gets the current screen resolution. Used for 'Games settings->Fullscreen resolution'*/

/* The window itself*/
RSGL::window win("Hearts of Iron IV (Hydra Launcher)",{0,0,int(current_res.x/2.24),int(current_res.y/2.1)},{0, 0, 0}, WINDOW_CENTRALIZED|DISABLE_RESIZING|DISABLE_MAXIMIZING|REMOVE_WINDOW_MARGINS|ENABLE_DPI_SCALING);

/* Important paths.*/
std::string user = RSGL::getUsername(); /* Gets the Windows PC's username.*/
std::string steam_path = "C:/Program Files (x86)/Steam/steamapps/common/Hearts of Iron IV"; /* Game path. Used for getting the launcher assets, DLCS and launching `hoi4.exe`.*/
std::string document_path = "C:/Users\\"+user+"/Documents/Paradox Interactive/Hearts of Iron IV"; /* User directory path. Used for settings and local mods.*/
std::string workshop_path = "C:/Program Files (x86)/Steam/steamapps/workshop/content/394360"; /* Steam workshop path. Used for workshop mods.*/

/* Audio*/
RSGL::audio bg_music("res/music/allies_peace_morningofdday.mp3"); /* The main bg theme.*/
RSGL::audio click("res/music/click_mouse_over_03.wav"); /* Click sound.*/

/* Settings/checks. */
int tab=0; /* Which tab is selected. 0 - news, 1 - dlc, 2 - mods.*/
bool music=true; /* If music is playing*/
int language=0; /* Which language is selected. 0 - English, 1 - Portuguese, 2 - German, 3 - French, 4 - Spanish, 5 - Polish, 6 - Russian*/
bool tick=false; 
RSGL::color white={255,255,255,0}; /* The white animation rectangle. Max c.a value is 29.*/
RSGL::color black={0,0,0,0}; /* The white animation rectangle. Max c.a value is 90.*/

/* The mods*/
std::vector<mod> mods = returnModList();
std::vector<RSGL::button> mod_list; // Mod button list

/* Interface buttons paths*/
std::string icon_path = steam_path+"/hoi4.exe"; // The actual hoi4 launcher path.
std::string flag_path = "res/gfx/flags/";
std::string tab_path = "res/gfx/tab.png";
std::string selected_path = "res/gfx/selected.png";
std::string playing_path = "res/gfx/playing.png";
std::string n_playing_path = "res/gfx/notplaying.png";

/* Interface buttons/images*/
RSGL::image  bg("res/gfx/launcher_bg.png", {0,0,win.r.width,win.r.length});
RSGL::image  mods_bg("res/gfx/mods_bg.png", {436,68,541,371});
RSGL::button close_button(RSGL::IMAGE, "res/gfx/close.png", {win.r.width-40,8,32,33});
RSGL::button play_button(RSGL::IMAGE, "res/gfx/play_button.png", {569,506,411,79});
RSGL::button clear_user_button(RSGL::IMAGE, "res/gfx/button_221x34.png", {237,551,221,36});
RSGL::button settings_button(RSGL::IMAGE, "res/gfx/launcher_settings_button.png", {169,486,64,64});
RSGL::button grey_button(RSGL::IMAGE, "res/gfx/button_148x34.png", {830,71,148,36}); /* The 'Mod tools' button*/
RSGL::button tab_1_button(RSGL::IMAGE, selected_path.c_str(), {439,27,141,42}); // 'News' tab
RSGL::button tab_2_button(RSGL::IMAGE, tab_path.c_str(), {579,27,138,42}); // 'DLC' tab
RSGL::button tab_3_button(RSGL::IMAGE, tab_path.c_str(), {719,27,138,42}); // 'Mods' tab
RSGL::button playing_button(RSGL::IMAGE, playing_path.c_str(), {102,486,64,64}); // Enable/disable music and sounds button