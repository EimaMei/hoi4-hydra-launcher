#ifndef SWGL
#include "../../SWGL.hpp"
#endif
int num=0;

int RSAL::audio::checkError(int error_code) {
    if(error_code) { 
        std::cout << "Error " << error_code; 
        std::string msg;
        switch (error_code) {
            case 257: msg = ": Invalid device ID (RSAL-"+std::to_string(num)+")"; break;
            case 263: msg = ": Invalid device name (RSAL-"+std::to_string(num)+")"; break;
            case 275: msg = ": File not found ("+File+")"; break;
            case 296: msg = ": File invalid ("+File+")"; break;
            case 300: msg = ": ID RSAL-"+std::to_string(num)+" ("+File+") has stopped."; break;
            default:  break;
        }
        std::cout << msg << std::endl;
    }
    return error_code;
}

void RSAL::audio::load(std::string file, int start/*=0*/, int end/*=0*/, int loops/*=0*/) {
    char path[MAX_PATH];
    GetShortPathName(file.c_str(),path,sizeof(path));
    File=file;Start=start;End=end; Loops=loops;

    char szCmd[MAX_PATH + 64];
    sprintf( szCmd, "Open \"%s\" Type MPEGVideo alias RSAL-%d", path, num);
    id="RSAL-"+std::to_string(num);
    num++;
    checkError(mciSendString( szCmd, NULL, 0, NULL ));
}

RSAL::audio::audio(std::string file, int start/*=0*/, int end/*=0*/, int loops/*=0*/) {
    load(file, start, end, loops);
}

void RSAL::audio::play() {
    std::string szCmd="play "+id;//+" Wait";
    if (Start!=0) {szCmd+=" from "+std::to_string(Start*1000); }
    if (End!=0) { szCmd+=" to "+std::to_string(End*1000); }
    szCmd+=" notify";
    
    checkError(mciSendString(szCmd.c_str(), NULL, 0, 0 ));
}

void RSAL::audio::pause() {
    std::string szCmd = "pause "+id;
    checkError(mciSendString(szCmd.c_str(), NULL, 0, 0 ));
    done=true;
}

void RSAL::audio::resume() {
    std::string szCmd = "resume "+id;
    checkError(mciSendString(szCmd.c_str(), NULL, 0, 0 ));
    done=false;
}

void RSAL::audio::stop() {
    std::string szCmd = "Close "+id;
    checkError(mciSendString(szCmd.c_str(), NULL, 0, 0 ));
    done=true;
}

bool RSAL::audio::isPlaying() {
    char szCmd[MAX_PATH + 64];
    sprintf( szCmd, "status %s mode", id.c_str());
    checkError(mciSendString(szCmd, szCmd, MAX_PATH + 64, 0 ));
    std::string res = szCmd;

    done=false;
    if (res == "playing") {done=true;}
    return done;
}

std::string SWGL::audio::getFile(){return File;}
void RSAL::quit() { mciSendString(TEXT("Close All"), NULL, 0, 0 ); }