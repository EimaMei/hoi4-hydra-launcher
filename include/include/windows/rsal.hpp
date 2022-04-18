#include <iostream>
#include <windows.h>

namespace RSAL{
    struct audio{
        void play(std::string file, int start=0, int end=0);
        void play();
        std::string getFile();
        void stop();
        void load(std::string file, int start=0, int end=0, int loops=0);

        void pause();
        void resume();

        bool isPlaying();
        bool done=false;
        int checkError(int error_code);

        audio(){}

        private:
            std::string File;
            std::string id;
            int Start,End, Loops;
        public: audio(std::string file, int start=0, int end=0, int loops=0);//{File=file;Start=start;End=end;}
    };
    void quit();
};