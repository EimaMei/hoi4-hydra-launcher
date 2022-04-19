#ifndef RSGL
#include "../RSGL.hpp"
#endif
#ifndef defs
#include "defines.hpp"
#endif 
#include <fstream>

std::vector<mod> returnModList() {
    char buff[1234];
    std::string cmd = "dir \""+document_path+"/mod\" /b *.mod | findstr /I /E \.mod";
    FILE*  fp = popen (cmd.data(), "r");
    if (fp== NULL){ std::cout << strerror(errno) << std::endl; return {};}
    
    fread(buff,sizeof(buff),1,fp);  std::string cur_string(buff);
    pclose(fp); 
    std::vector<mod> output;
    std::string pipe;
    for (int i=0; i < cur_string.size(); i++){
        if (cur_string.at(i) == '.'){ 
            mod m;
            m.path=document_path+"/mod/"+pipe+".mod";
            m.id = std::stoll(pipe.substr(4));
            m.workshop=true;

            std::ifstream file(m.path);
            std::string name;
            bool no_tags=true;
            while (file.good() && std::getline(file, name)) {
                if (no_tags) {
                    if (name.find("name=") != std::string::npos) { name=name.substr(name.find("name=")+6); m.name=name.substr(0, name.find("\"")); }
                    else if (name.find("version=") != std::string::npos) { name=name.substr(name.find("version=")+9); m.version=name.substr(0, name.find("\"")); }
                    else if (name.find("tags") != std::string::npos) no_tags=false;
                }
                else {
                    if (name.find("}") != std::string::npos) {no_tags=true;m.tag_list.edited=true;}
                    
                    else if (name.find("Graphics") != std::string::npos) m.tag_list.graphics=true;
                    else if (name.find("Alternative History") != std::string::npos) m.tag_list.alternative_history=true;
                    else if (name.find("Events") != std::string::npos) m.tag_list.events=true;
                    else if (name.find("Fixes") != std::string::npos) m.tag_list.fixes=true;
                    else if (name.find("Gameplay") != std::string::npos) m.tag_list.gameplay=true;
                    else if (name.find("National focuses") != std::string::npos) m.tag_list.national_focuses=true;
                    else if (name.find("Sound") != std::string::npos) m.tag_list.sound=true;
                    else if (name.find("Technolohies") != std::string::npos) m.tag_list.technologies=true;
                    else if (name.find("Translation") != std::string::npos) m.tag_list.translation=true;
                    else if (name.find("Historical") != std::string::npos) m.tag_list.historical=true;
                    else if (name.find("Utilities") != std::string::npos) m.tag_list.utilities=true;
                    else if (name.find("Ideologies") != std::string::npos) m.tag_list.ideologies=true;
                    else if (name.find("Map") != std::string::npos) m.tag_list.map=true;
                }
                if (m.name != "" && m.version != "" && m.tag_list.edited) {
                    output.insert(output.end(),m); 
                    break;
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
}

void drawImages() {
    bg.draw();
    if (tab==2) { 
        mods_bg.draw();
        grey_button.draw();
        for (int i=0; i<mod_list.size(); i++) {
            mod_list[i].draw();
            if (mod_list[i].clicked()) std::cout << "You clicked the mod: " << mods[i].name << std::endl;
        }
    }

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
    if (close_button.hovered() && !close_button.pressed()) {RSGL::drawRect({win.r.width-40,8,32,33}, white); if (white.a<28) white.a++;}
    else if (play_button.hovered() && !play_button.pressed()) {RSGL::drawRect({577,511,395,68}, white); if (white.a<28) white.a++;}
    else if (clear_user_button.hovered() && !clear_user_button.pressed()) { RSGL::drawRect({243,556,207,28}, white); if (white.a<28) white.a++;}
    else if (playing_button.hovered() && !playing_button.pressed()) { /*RSGL::drawCircle(rr, white);*/ if (white.a<28) white.a++;}
    else if (tab==2) {
        if (grey_button.hovered() && !grey_button.pressed()) { RSGL::drawRect({834,74,136,30}, white); if (white.a<28) white.a++;}
        else white.a=0;
    }
    else white.a=0;


    if (close_button.pressed()) {RSGL::drawRect({win.r.width-40,8,32,33}, black); if (black.a<84) black.a+=3;}
    else if (play_button.pressed()) {RSGL::drawRect({577,511,395,68}, black); if (black.a<84) black.a+=3;}
    else if (clear_user_button.pressed()) { RSGL::drawRect({243,556,207,28}, black); if (black.a<84) black.a+=3;}
    else if (playing_button.pressed()) { /*RSGL::drawCircle(rr , black); white);*/ if (black.a<84) black.a+=3;}
    else if (tab==2) {
        if (grey_button.pressed()) { RSGL::drawRect({834,74,136,30}, black); if (black.a<84) black.a+=3;}
        else black.a=0;
    }
    else black.a=0;
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
    
        case RK_Q: {white.a--;black.a--; break;}
        case RK_E: {white.a++;black.a++; break;}
        default: break;
    }
    std::cout << rr.x << "x" << rr.y << " | " << rr.width << "x" << rr.length << " | " << white.a << " | " << mm.x << "x" << mm.y << " | " << mm.radius << " | " << RSGL::getFPS() << " FPS" << std::endl; 
}