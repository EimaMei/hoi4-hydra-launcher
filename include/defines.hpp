#ifndef SWGL
#include "../SWGL.hpp"
#endif
bool running=true;
#define defs 1 // Make sure that this header was read.

struct mod {
    std::string name; /* Name of the mod.*/
    std::string long_path; /* The full directory of the mod.*/
    std::string path; /* The short directory path of the mod.*/
    std::string version; /* Version of the mod.*/
    std::string supported_version; /* Supported version.*/
    std::vector<std::pair<std::string, bool>> tag_list = {{"Graphics", false}, {"Alternative history", false}, {"Events", false}, {"Fixes", false}, {"Gameplay", false}, {"National focuses", false}, {"Sound", false}, {"Technologies", false}, {"Translation", false}, {"Historical",false}, {"Utilities", false}, {"Ideologies", false}, {"Map", false} }; /* The enabled tags.*/
    bool selected;
}; /* The mod structure.*/

enum categories {EMPTY,content_pack,music_pack,unit_pack,expansion};
struct dlc {
    std::string name; /* Name of the dlc.*/
    std::string long_path; /* The full directory of the dlc.*/
    std::string path; /* Directory of the dlc.*/
    bool third_party=false; /* Check if the DLC has any third party content*/
    categories category=EMPTY;
    bool selected;
}; /* The DLC structure.*/

/* Functions*/
int startup();
int quit();
void checkButtons();
void drawImages();
void drawRects();
void debugmode(int event);
void saveSettings();
std::vector<mod> returnModList();
std::vector<dlc> returnDLCList();

SWGL::rect rr = {475,118+(0*35),403,35}; // Debug element, used for positioning buttons.
SWGL::circle mm = {693,521,50}; // Debug element, used for positioning circular objects.
SWGL::color cc; // Debug element, used for alpha channel testing

/* Resolutions.*/
std::vector<SWGL::resolution> resolutions = SWGL::getAvailableResolutions(); /* Get all available resolutions. Used for creating the window and 'Games settings->Fullscreen resolution'.*/
SWGL::resolution current_res = SWGL::getScreenResolution(); /* Gets the current screen resolution. Used for 'Games settings->Fullscreen resolution'*/

/* The window itself*/
SWGL::window win("Hearts of Iron IV (Hydra Launcher)",{0,0,int(current_res.x/2.24),int(current_res.y/2.1)},{0, 0, 0}, WINDOW_CENTRALIZED|DISABLE_RESIZING|DISABLE_MAXIMIZING|REMOVE_WINDOW_MARGINS|ENABLE_DPI_SCALING);

/* Important paths.*/
std::string user = SWGL::getUsername(); /* Gets the Windows PC's username.*/
std::string steam_path = "C:/Program Files (x86)/Steam/steamapps/common/Hearts of Iron IV"; /* Game path. Used for getting the launcher assets, DLCS and launching `hoi4.exe`.*/
std::string document_path = "C:/Users\\"+user+"/Documents/Paradox Interactive/Hearts of Iron IV"; /* User directory path. Used for settings and local mods.*/
std::string music_path = document_path+"/pdx_launcher/user_settings.txt";

/* Audio*/
SWGL::audio bg_music("res/music/allies_peace_morningofdday.mp3"); /* The main bg theme.*/
SWGL::audio click("res/music/click_mouse_over_03.wav"); /* Click sound.*/

/* Settings/checks. */
int tab=0; /* Which tab is selected. 0 - news, 1 - dlc, 2 - mods.*/
bool music=false; /* If music is playing*/
int language=0; /* Which language is selected. 0 - English, 1 - Portuguese, 2 - German, 3 - French, 4 - Spanish, 5 - Polish, 6 - Russian*/
bool tick=false; 
bool settings=false;
int draw_list_settings=-1;
SWGL::color white={255,255,255,0}; /* The white animation rectangle. Max c.a value is 29.*/
SWGL::color black={0,0,0,0}; /* The white animation rectangle. Max c.a value is 90.*/

/* Mods & DLC*/
std::vector<mod> mods = returnModList();
std::vector<SWGL::button> mod_list; // Mod button list
std::vector<dlc> dlcs = returnDLCList();
std::vector<SWGL::button> dlc_list; // DLC button list

/* Interface buttons paths*/
std::string icon_path = steam_path+"/hoi4.exe"; // The actual hoi4 launcher path.
std::string flag_path = "res/gfx/flags/";
std::string tab_path = "res/gfx/tab.png";
std::string selected_path = "res/gfx/selected.png";
std::string playing_path = "res/gfx/playing.png";
std::string n_playing_path = "res/gfx/notplaying.png";
std::string dropdown_path = "res/gfx/dropdown_0.png";
std::string dropdown_selected_path = "res/gfx/dropdown_1.png";

/* Interface buttons/images*/
SWGL::image  bg("res/gfx/launcher_bg.png", {0,0,win.r.width,win.r.length});
SWGL::button close_button(SWGL::IMAGE, "res/gfx/close.png", {win.r.width-40,8,32,33});
SWGL::button play_button(SWGL::IMAGE, "res/gfx/play_button.png", {569,506,411,79});
SWGL::button clear_user_button(SWGL::IMAGE, "res/gfx/button_221x34.png", {237,551,221,36});
SWGL::button settings_button(SWGL::IMAGE, "res/gfx/launcher_settings_button.png", {169,486,64,64});
SWGL::button tab_1_button(SWGL::IMAGE, selected_path.c_str(), {439,27,141,42}); // 'News' tab
SWGL::button tab_2_button(SWGL::IMAGE, tab_path.c_str(), {579,27,138,42}); // 'DLC' tab
SWGL::button tab_3_button(SWGL::IMAGE, tab_path.c_str(), {719,27,138,42}); // 'Mods' tab
SWGL::button playing_button(SWGL::IMAGE, playing_path.c_str(), {102,486,64,64}); // Enable/disable music and sounds button
SWGL::image  settings_bg("res/gfx/settings_bg.png",{275, 100,447,365});
SWGL::button localisation_button(SWGL::IMAGE, "res/gfx/flags/English_flag.png", {21,494,66,48});

/* Settings buttons*/
std::vector<SWGL::button> settings_dropdown_list;
std::vector<SWGL::button> display_mode_list;

/* Mods tab images/buttons/scrollbar*/
SWGL::scrollbar mod_scrollbar = win.createScrollbar(SWGL::IMAGE, "res/gfx/scroll/scrollbar_vertical.png", {900,109,19,319});
SWGL::image  mods_bg("res/gfx/mods_bg.png", {436,68,541,373});
SWGL::image  mods_bg_num2("res/gfx/mods_bg_mfbuttons.png", {436,68,541,373});
SWGL::button grey_button(SWGL::IMAGE, "res/gfx/button_148x34.png", {830,71,148,36}); /* The 'Mod tools' button*/
SWGL::button scroll_up_button(SWGL::IMAGE, "res/gfx/scroll/scroll_up.png", {902,113,16,16});
SWGL::button scroll_down_button(SWGL::IMAGE, "res/gfx/scroll/scroll_down.png", {901,408,16,16});