#include "../RSGL.hpp" // Main library to draw everything
//#include "windows/extension.cpp" // Extension to the main library
#include "defines.hpp" // Launcher variables/settings
#include "functions.cpp"

int main(){
    RSGL::discordCreateApplication(924064708628205598, true); // Set the discord presence and add a timer
    win.changeIcon(icon_path.c_str());

    if (music) {bg_music.play();} // Play the music if it's enabled
    else {playing_button.load(n_playing_path.c_str(), {102,486,64,64});}
    //((BOOL(WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(1);

    for (int i=0; i<mods.size(); i++) {
        RSGL::button faker(RSGL::IMAGE, "res/gfx/mod_0.png", {0,35*i,404,35});   
        mod_list.insert(mod_list.end(), faker);  
    }

    while (running){
        win.checkEvents();
        if (win.event.type == RSGL::quit || win.isPressed(RK_ESC) ) running = false; // Check if the user wants to quit
        checkButtons(); // Check the buttons

        if (!bg_music.isPlaying() && music) { bg_music.load(bg_music.getFile()); } // Check if the music ended and the music setting is enabled. If so, load the song again and loop it back.
        if (tick) { tick=false; if (music) { click.play(); click.load(click.getFile()); } } // Check if the user clicked and play a sound if music is enabled

        //debugmode(win.event.key);
        drawImages(); //Draws the bg and the buttons
        drawRects(); //Draws the transition animations
        win.clear(); // Clear everything for the next frame
        RSGL::discordUpdatePresence("Playing Hearts of Iron IV", "", "hoi4"); // Update the discord presence
    } 
    win.close();
    return 0;
} 