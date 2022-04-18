#ifndef RSGL
#include "../RSGL.hpp"
#endif
bool running=true;

RSGL::rect rr = {0,0,0,0}; // Debug element, used for positioning buttons.

/* Resolutions.*/
std::vector<RSGL::resolution> resolutions = RSGL::getAvailableResolutions(); /* Get all available resolutions. Used for creating the window and 'Games settings->Fullscreen resolution'.*/
RSGL::resolution current_res = RSGL::getScreenResolution(); /* Gets the current screen resolution. Used for 'Games settings->Fullscreen resolution'*/

/* The window itself*/
RSGL::window win("Hearts of Iron IV (Hydra Launcher)",{0,0,int(current_res.x/2.24),int(current_res.y/2.1)},{0, 0, 0}, WINDOW_CENTRALIZED|DISABLE_RESIZING|DISABLE_MAXIMIZING|REMOVE_WINDOW_MARGINS|ENABLE_DPI_SCALING);

/* Functions*/
void checkButtons();
void drawImages();
void drawRects();
void debugmode(int event);

/* Important paths.*/
std::string user = RSGL::getUsername(); /* Gets the Windows PC's username.*/
std::string steam_path = "C:/Program Files (x86)/Steam/steamapps/common/Hearts of Iron IV"; /* Game path. Used for getting the launcher assets, DLCS and launching `hoi4.exe`.*/
std::string document_path = "C:/Users/"+user+"/Documents/Paradox Interactive/Hearts of Iron IV"; /* User directory path. Used for settings and local mods.*/
std::string workshop_path = "C:/Program Files (x86)/Steam/steamapps/workshop/content/394360"; /* Steam workshop path. Used for workshop mods.*/

/* Audio*/
RSGL::audio bg_music("res/music/allies_peace_morningofdday.mp3"); /* The main bg theme.*/
RSGL::audio click("res/music/click_mouse_over_03.wav"); /* Click sound.*/

/* Settings/checks. */
int tab=0; /* Which tab is selected. 0 - news, 1 - dlc, 2 - mods.*/
bool music=true; /* If music is playing*/
int language=0; /* Which language is selected. 0 - English, 1 - Portuguese, 2 - German, 3 - French, 4 - Spanish, 5 - Polish, 6 - Russian*/
bool tick=false; 
int alpha=0; /* The alpha channel value when hovering the buttons. Max value is 23.*/

/* Interface buttons paths*/
std::string icon_path = steam_path+"/hoi4.exe"; // The actual hoi4 launcher path.
std::string flag_path = "res/gfx/flags/";
std::string tab_path = "res/gfx/tab.png";
std::string selected_path = "res/gfx/selected.png";
std::string playing_path = "res/gfx/playing.png";
std::string n_playing_path = "res/gfx/notplaying.png";

/* Interface buttons*/
RSGL::image  bg("res/gfx/launcher_bg.png", {0,0,win.r.width,win.r.length}); // The BG image, not a button but shut up!
RSGL::button close_button(RSGL::IMAGE, "res/gfx/close.png", {win.r.width-40,8,32,33});
RSGL::button play_button(RSGL::IMAGE, "res/gfx/play_button.png", {569,506,411,79});
RSGL::button clear_user_button(RSGL::IMAGE, "res/gfx/button_221x34.png", {237,551,221,36});
RSGL::button settings_button(RSGL::IMAGE, "res/gfx/launcher_settings_button.png", {169,486,64,64});
RSGL::button tab_1_button(RSGL::IMAGE, selected_path.c_str(), {439,27,141,42}); // 'News' tab
RSGL::button tab_2_button(RSGL::IMAGE, tab_path.c_str(), {579,27,138,42}); // 'DLC' tab
RSGL::button tab_3_button(RSGL::IMAGE, tab_path.c_str(), {719,27,138,42}); // 'Mods' tab
RSGL::button playing_button(RSGL::IMAGE, playing_path.c_str(), {102,486,64,64}); // Enable/disable music and sounds button
#define defs