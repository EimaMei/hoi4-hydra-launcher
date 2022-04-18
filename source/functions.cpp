#ifndef RSGL
#include "../RSGL.hpp"
#endif
#ifndef defs
#include "defines.hpp"
#endif 

void checkButtons() {
    if (close_button.clicked()) { running = false; }
    if (play_button.clicked()) { click.load("res/music/click_ok.wav"); click.play(); std::string cmd="\""+steam_path+"/hoi4.exe\""; popen(cmd.c_str(),"r"); tick=true; click.load("res/music/click_mouse_over_03.wav"); };
    if (playing_button.clicked()) {
        if (music) { bg_music.pause(); playing_button.load(n_playing_path.c_str(), {102,486,64,64});}
        else { bg_music.play(); playing_button.load(playing_path.c_str(), {102,486,64,64}); }
        music=!music;
        tick=true;
    }

    if (tab_1_button.clicked() && tab!=0) { tab=0; tab_1_button.load(selected_path.c_str(), {439,27,141,42}); tab_2_button.load(tab_path.c_str(), {439+140,27,141,42}); tab_3_button.load(tab_path.c_str(), {439+140*2,27,141,42}); tick=true; }
    if (tab_2_button.clicked() && tab!=1) { tab=1; tab_1_button.load(tab_path.c_str(), {439,27,141,42}); tab_2_button.load(selected_path.c_str(), {439+140,27,141,42}); tab_3_button.load(tab_path.c_str(), {439+140*2,27,141,42}); tick=true; }
    if (tab_3_button.clicked() && tab!=2) { tab=2; tab_1_button.load(tab_path.c_str(), {439,27,141,42}); tab_2_button.load(tab_path.c_str(), {439+140,27,141,42}); tab_3_button.load(selected_path.c_str(), {439+140*2,27,141,42}); tick=true; }
}

void drawImages() {
    bg.draw();
    close_button.draw();
    play_button.draw();
    clear_user_button.draw();
    
    tab_1_button.draw();
    tab_2_button.draw();
    tab_3_button.draw();

    settings_button.draw();
    playing_button.draw();
}

void drawRects() {
    if (close_button.hovered()) {RSGL::drawRect({win.r.width-40,8,32,33}, {255,255,255, alpha}); if (alpha<27) alpha++;}
    else if (play_button.hovered()) {RSGL::drawRect({577,511,395,68}, {255,255,255, alpha}); if (alpha<27) alpha++;}
    else if (clear_user_button.hovered()) { RSGL::drawRect({243,556,207,28}, {255,255,255, alpha}); if (alpha<27) alpha++;}
    else if (playing_button.hovered()) { /*RSGL::drawCircle(rr, {255,255,255, alpha});*/ if (alpha<27) alpha++;}
    else alpha=0;
}

void debugmode(int event) {
    if (event == RK_Left) rr.x--;
    if (event == RK_Right) rr.x++;
    if (event == RK_Down) rr.y++;
    if (event == RK_Up) rr.y--;
    
    if (event == RK_A) rr.width--;
    if (event == RK_D) rr.width++;
    if (event == RK_S) rr.length++;
    if (event == RK_W) rr.length--;

    /*if (event == RK_A) rr.radius--;
    if (event == RK_D) rr.radius++;
    if (event == RK_S) rr.radius++;
    if (event == RK_W) rr.radius--;
    If `rr` is a circle*/
    
    if (event == RK_Q) alpha--;
    if (event == RK_E) alpha++;
    
    std::cout << rr.x << "x" << rr.y << " | " << rr.width << "x" << rr.length << " | " << alpha << " | " << RSGL::getFPS() << " FPS" << std::endl; 
}