#include "../SWGL.hpp" // Main library to draw everything
//#include "windows/extension.cpp" // Extension to the main library
#include "../include/defines.hpp" // Launcher variables/settings
#include "functions.cpp"

int main(){
    startup();
    while (running){
        win.checkEvents();
        if (win.event.type == SWGL::quit || win.isPressed(RK_ESC) ) running = false; // Check if the user wants to quit
        checkButtons(); // Check the buttons

        if (!bg_music.isPlaying() && music) { bg_music.load(bg_music.getFile()); } // Check if the music ended and the music setting is enabled. If so, load the song again and loop it back.
        if (tick) { tick=false; if (music) { click.play(); click.load(click.getFile()); } } // Check if the user clicked and play a sound if music is enabled

        //debugmode(win.event.key);
        drawImages(); //Draws the bg and the buttons
        drawRects(); //Draws the transition animations
        win.clear(); // Clear everything for the next frame
        SWGL::discordUpdatePresence("Playing Hearts of Iron IV", "", "hoi4"); // Update the discord presence
    } 
    quit();
    return 0;
} 