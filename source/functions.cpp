#ifndef SWGL
#include "../SWGL.hpp"
#endif
#ifndef defs
#include "../include/defines.hpp"
#endif 
#include <fstream>

int startup() {
    SWGL::discordCreateApplication(924064708628205598, true); // Set the discord presence and add a timer
    win.changeIcon(icon_path.c_str());

    saveSettings();
    if (music) {bg_music.play();} // Play the music if it's enabled
    else {playing_button.load(n_playing_path.c_str(), {102,486,64,64});}

    for (int i=0; i<mods.size(); i++) { SWGL::button faker(SWGL::IMAGE, "res/gfx/mod_0.png", {475,118+(i*35),403,35});    mod_list.insert(mod_list.end(), faker);   }
    for (int i=0; i<dlcs.size(); i++) { SWGL::button faker(SWGL::IMAGE, "res/gfx/mod_0.png", {475,118+(i*35),403,35}); dlc_list.insert(dlc_list.end(), faker);   }
    for (int i=0; i<3; i++) { int num=0; if (i==2) {num++;} SWGL::button faker(SWGL::IMAGE, "res/gfx/dropdown_0.png", {629,195+(i*50),36,32-num}); settings_dropdown_list.insert(settings_dropdown_list.end(), faker); }
    for (int i=0; i<3; i++) {SWGL::button faker(SWGL::IMAGE, "res/gfx/language_button.png", {319,229+(i*26),335,29}); display_mode_list.insert(display_mode_list.end(), faker); }

    for (int i=resolutions.size()-1; i>-1; i--) { if (resolutions[i].x < 1280 && resolutions[i].y < 720) resolutions.pop_back(); else break; } // Remove useless resolutions that aren't supported in hoi4

    mod_scrollbar.setPointer(SWGL::IMAGE, "res/gfx/scroll/dragger.png", {901,127,16,16});
    mod_scrollbar.limit=319+109-32;
    mod_scrollbar.speed=3;

    return 0;
}

int quit() {
    std::fstream file(music_path); const char* val="no\n";
    if (music) val="yes";
    file << "launcher_sound=" << val;
    file.close();
    win.close();
    return 0;
}

void saveSettings() {
    std::string cmd = "mkdir \""+document_path+"/pdx_launcher\" 2> nul";
    pclose(popen(cmd.data(), "r"));
    std::fstream file(music_path);
    if (!file) file << "launcher_sound=yes"<< "\n";

    std::getline(file, cmd);
    if (cmd.find("yes") != std::string::npos) music=true;
    else if (cmd.find("no") != std::string::npos) music=false;
    file.close();
}

std::vector<dlc> returnDLCList() {
    char buff[1234]; std::string cmd = "dir \""+steam_path+"/dlc\" /s /b *.dlc 2> nul | findstr /I /E \\.dlc";
    FILE*  fp = popen (cmd.data(), "r"); if (fp== NULL){ std::cout << strerror(errno) << std::endl; return {};}
    fread(buff,sizeof(buff),1,fp);  std::string cur_string(buff); pclose(fp); 
    std::vector<dlc> output; std::string pipe;

    for (int i=68; i < cur_string.size(); i++){
        if (cur_string.at(i) == '.'){ 
            dlc m; m.path="/dlc/"+pipe+".dlc"; m.long_path=steam_path+"/dlc/"+pipe+".dlc";
            std::ifstream file(m.long_path); std::string name; bool third_party=false;
            while (file.good() && std::getline(file, name)) {
                if (name.find("name") != std::string::npos) { name=name.substr(name.find("\"")+1); m.name=name.substr(0, name.find("\""));} //m.name=name.substr(0, name.find("\"")); }
                else if (name.find("third_party_content") != std::string::npos) {third_party=true; if (name.find("yes") != std::string::npos) m.third_party=true; }
                else if (name.find("category") != std::string::npos) { 
                    if (name.find("content_pack") != std::string::npos) {m.category=content_pack;} 
                    else if (name.find("unit_pack") != std::string::npos) {m.category=unit_pack;}
                    else if (name.find("music") != std::string::npos) {m.category=music_pack;}
                    else if (name.find("expansion") != std::string::npos) {m.category=expansion;}
                } 

                if (m.name != "" && third_party && m.category == EMPTY) { output.insert(output.end(),m); break; }
            }
            pipe=""; i+=72;
        }
        else if (cur_string.at(i) == '\n') { continue;}
        else pipe+=cur_string.at(i);
    }
    return output;
}


std::vector<mod> returnModList() {
    char buff[1234]; std::string cmd = "dir \""+document_path+"/mod\" /b *.mod 2> nul | findstr /I /E \\.mod";
    FILE*  fp = popen (cmd.data(), "r");
    if (fp== NULL){ std::cout << strerror(errno) << std::endl; return {};}
    
    fread(buff,sizeof(buff),1,fp); pclose(fp);  std::string cur_string(buff);
    std::vector<mod> output; std::string pipe;
    for (int i=0; i < cur_string.size(); i++){
        if (cur_string.at(i) == '.'){
            mod m; m.long_path=document_path+"/mod/"+pipe+".mod"; m.path="/mod/"+pipe+".mod";

            std::ifstream file(m.path); std::string name;
            bool no_tags=true;
            while (file.good() && std::getline(file, name)) {
                if (no_tags) {
                    if (name.find("name=") != std::string::npos) { name=name.substr(name.find("name=")+6); m.name=name.substr(0, name.find("\"")); }
                    else if (name.find("version=") != std::string::npos) { name=name.substr(name.find("version=")+9); m.version=name.substr(0, name.find("\"")); }
                    else if (name.find("tags") != std::string::npos) no_tags=false;

                    if (m.name != "" && m.version != "") { output.insert(output.end(),m); break; }
                }
                else {
                    if (name.find("}") != std::string::npos) {no_tags=true;}
                    for (int i=0; i<m.tag_list.size(); i++) { if (name.find(m.tag_list[i].first) != std::string::npos) { m.tag_list[i].second=true; } }
                }
            }
            pipe="";
            file.close();
        }
        else if (cur_string.at(i) == '\n') pipe="";
        else pipe+=cur_string.at(i);
    }
    return output;
}

void checkButtons() {
    if (!settings) {
        if (close_button.released()) { running = false; }
        if (play_button.released()) { click.load("res/music/click_ok.wav"); click.play(); std::string cmd="\""+steam_path+"/hoi4.exe\""; popen(cmd.c_str(),"r"); tick=true; click.load("res/music/click_mouse_over_03.wav"); };
        if (playing_button.released()) {
            if (music) { bg_music.pause(); playing_button.load(n_playing_path.c_str(), {102,486,64,64});}
            else { bg_music.play(); playing_button.load(playing_path.c_str(), {102,486,64,64}); }
            music=!music;
        }
    
        if (tab_1_button.released() && tab!=0) { tab=0; tab_1_button.load(selected_path.c_str(), {439,27,141,42}); tab_2_button.load(tab_path.c_str(), {439+140,27,141,42}); tab_3_button.load(tab_path.c_str(), {439+140*2,27,141,42}); tick=true; }
        if (tab_2_button.released() && tab!=1) { tab=1; tab_1_button.load(tab_path.c_str(), {440,27,137,42}); tab_2_button.load(selected_path.c_str(), {439+140,27,141,42}); tab_3_button.load(tab_path.c_str(), {439+140*2,27,141,42}); tick=true; }
        if (tab_3_button.released() && tab!=2) { tab=2; tab_1_button.load(tab_path.c_str(), {440,27,137,42}); tab_2_button.load(tab_path.c_str(), {439+140,27,141,42}); tab_3_button.load(selected_path.c_str(), {439+140*2,27,141,42}); tick=true; }
    
        if (settings_button.released()) { settings = true; tick=true; }
    } 
    else {
        if (close_button.released()) { settings = false; }
        for (int i=0; i<settings_dropdown_list.size(); i++) {
            if (settings_dropdown_list[i].held()) settings_dropdown_list[i].load(dropdown_selected_path.c_str(), settings_dropdown_list[i].r);
            else if (settings_dropdown_list[i].released()) {
                if (draw_list_settings == i ) draw_list_settings=-1;
                else draw_list_settings=i;

                settings_dropdown_list[i].load(dropdown_path.c_str(), settings_dropdown_list[i].r); tick=true;
            }
        }
    }
}

void drawImages() {
    bg.draw();
    if (tab==1) { for (int i=0; i<dlc_list.size(); i++) { dlc_list[i].draw(); } }
    else if (tab==2) { 
        mod_scrollbar.checkEvents();
        mods_bg.draw();
        grey_button.draw();
        for (int i=0; i<mod_list.size(); i++) { 
            if (mod_list[i].r.y-mod_scrollbar.pos > 118+(8*35)) {break;}
            if (mod_list[i].r.y-mod_scrollbar.pos >  80) {
                SWGL::rect new_r = mod_list[i].r;
                new_r.y -= mod_scrollbar.pos;
                mod_list[i].draw(new_r); 
            }
        }
        mods_bg_num2.draw();
        mod_scrollbar.draw();
        scroll_up_button.draw();
        scroll_down_button.draw();
    }
    play_button.draw();
    clear_user_button.draw();
    
    tab_1_button.draw();
    tab_2_button.draw();
    tab_3_button.draw();

    settings_button.draw();
    playing_button.draw();
    localisation_button.draw();

    if (settings) {
        SWGL::rect old_r=close_button.r; close_button.draw(old_r);
        SWGL::drawRect({0,0,win.r.width, win.r.length}, {0,0,0,70});

        settings_bg.draw();
        for (int i=0; i<settings_dropdown_list.size(); i++) settings_dropdown_list[i].draw();
        switch (draw_list_settings) {
            case 0: for (int i=0; i<3;i++) {display_mode_list[i].draw();} break;
            case 1: break;
            case 2: break;
            default: break;
        }
        close_button.draw({677,124,32,33});
    }
    else { close_button.draw({win.r.width-40,8,32,33}); }
}

void drawRects() {
    if (!settings) {
        if (close_button.hovered() && !close_button.pressed()) {SWGL::drawRect({win.r.width-40,8,32,33}, white); if (white.a<28) white.a++;}
        else if (play_button.hovered() && !play_button.pressed()) {SWGL::drawRect({577,511,395,68}, white); if (white.a<28) white.a++;}
        else if (clear_user_button.hovered() && !clear_user_button.pressed()) { SWGL::drawRect({243,556,207,28}, white); if (white.a<28) white.a++;}
        else if (playing_button.hovered() && !playing_button.pressed()) { /*SWGL::drawCircle(rr, white);*/ if (white.a<28) white.a++;}
        else if (tab==2) {
            int n=0;
            if (grey_button.hovered() && !grey_button.pressed()) { SWGL::drawRect({834,74,136,30}, white); if (white.a<28) white.a++; n++; }
            /*for (int i=0; i<mod_list.size(); i++) {
                if (mod_list[i].r.y > 398) break;
                if (mod_list[i].hovered() && !mod_list[i].pressed()) { SWGL::drawRect(mod_list[i].r, white); if (white.a<28) white.a++; n++; break;}
            }*/
            if (n==0) white.a;
        }
        else white.a=0;


        if (close_button.pressed()) {SWGL::drawRect({win.r.width-40,8,32,33}, black); if (black.a<84) black.a+=3;}
        else if (play_button.pressed()) {SWGL::drawRect({577,511,395,68}, black); if (black.a<84) black.a+=3;}
        else if (clear_user_button.pressed()) { SWGL::drawRect({243,556,207,28}, black); if (black.a<84) black.a+=3;}
        else if (playing_button.pressed()) { /*SWGL::drawCircle(rr , black); white);*/ if (black.a<84) black.a+=3;}
        else if (tab==2) {
            if (grey_button.pressed()) { SWGL::drawRect({834,74,136,30}, black); if (black.a<84) black.a+=3;}
            else black.a=0;
        }
        else black.a=0;
    }
    else {
        if (close_button.hovered() && !close_button.pressed()) {SWGL::drawRect({677,124,32,33}, white); if (white.a<28) white.a++;}
        else white.a=0;
    }
}    

void debugmode(int event) {
    switch (event) {
        case RK_Left: {rr.x--; mm.x--; break;}
        case RK_Right: {rr.x++; mm.x++; break;}
        case RK_Down: {rr.y++; mm.y++; break;}
        case RK_Up: {rr.y--; mm.y--; break;}
    
        case RK_A: { rr.width--;  mm.radius--; break; }
        case RK_D: { rr.width++;  mm.radius++; break; }
        case RK_S: { rr.length++; mm.radius++; break; }
        case RK_W: { rr.length--; mm.radius--; break; }
    
        case RK_Q: {white.a--;black.a--;cc.a--; break;}
        case RK_E: {white.a++;black.a++;cc.a++; break;}
        default: break;
    }
    std::cout << rr.x << "x" << rr.y << " | " << rr.width << "x" << rr.length << " | W:" << white.a << " B:" << black.a << " CC:" << cc.a << " | " << mm.x << "x" << mm.y << " | " << mm.radius << " | " << SWGL::getFPS() << " FPS" << std::endl; 
}

SWGL::scrollbar SWGL::window::createScrollbar(SWGL::shape id, const char* filename, SWGL::rect r) { 
    scrollbar list;
    list.r=r; list.ID=id; 
    list.s_window=SWGL::window::hwnd;
    list.load(filename, r);
    
    return list;
}; 

void SWGL::scrollbar::load(const char* filename, SWGL::rect r) {
    i.filename=filename;  i.r=r; 
    i.load();
}

void SWGL::scrollbar::setPointer(SWGL::shape id, const char* filename, SWGL::rect r) {
    r_pointer = r; original_r=r;
    ID_pointer=id;
    i_pointer.filename=filename; i_pointer.r=r_pointer;
    i_pointer.load();
}

int SWGL::scrollbar::checkEvents() {
    if ((r_pointer.y >= original_r.y || r_pointer.y < limit) && SWGL::win.hwnd==s_window) {
        if (SWGL::win.type == SWGL::MouseWheelScrolling) {
            switch (SWGL::win.button) {
                case 4 /* up*/: {r_pointer.y-=speed; pos-=speed; return 0;}
                case 5 /* down*/: {r_pointer.y+=speed; pos+=speed; return 0;}
                default: break;
            }
        }
        if (SWGL::win.type == SWGL::MouseButtonPressed && SWGL::win.button==1) {
            if (SWGL::RectCollidePoint(r_pointer, {SWGL::win.mouse.x, SWGL::win.mouse.y})) {old_mouse=SWGL::win.mouse; r_pointer.y+=speed;}
        }
        m=false;
    }
    return 0;
}

void SWGL::scrollbar::draw() {
    i.r=r; 
    i_pointer.r = r_pointer;

    i.draw();
    i_pointer.draw();
}