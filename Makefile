GXX = g++
ARGS = -Wall -fPIC -g -O2
SOURCEDIR = source/linux
SOURCE = $(SOURCEDIR)/rsgl.cpp
LIBDIR = $(SOURCEDIR)/deps
LIBS = $(LIBDIR)/libX11.so.6.4.0 $(LIBDIR)/libpng16.so.16.37.0  $(LIBDIR)/libpthread-2.33.so

WARGS = -O2 -Wno-unknown-pragmas -mwindows
WSOURCEDIR = source/windows
WSOURCE = $(WSOURCEDIR)/rsgl.cpp $(WSOURCEDIR)/rsal.cpp $(WSOURCEDIR)/collide.cpp $(WSOURCEDIR)/device.cpp $(WSOURCEDIR)/draw.cpp $(WSOURCEDIR)/xinput.cpp $(WSOURCEDIR)/other.cpp $(WSOURCEDIR)/extension.cpp
WLIBS = -L"$(CURDIR)/source/windows/deps" -lcomdlg32 -lgdi32 -ldiscord -lopengl32 -lxinput -lwinmm -lpsapi

all:
	make windowsBuild
	make windowsCompile
windowsCompile:
	@$(GXX) $(WARGS) source/main.cpp libRSGL.lib -lcomdlg32 -lopengl32 -lmingw32 -static-libgcc -static-libstdc++ -o HOI4.exe
	./HOI4.exe
windowsBuild: 
	@$(GXX) -C $(WARGS) $(WSOURCE) -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	@rm rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o
windowsBuildRSGL: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/rsgl.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildXinput: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/xinput.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildDraw: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/draw.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildOther: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/other.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildDevice: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/device.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildRSAL: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/rsal.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
windowsBuildExtension: 
	@$(GXX) -C $(WARGS) $(WSOURCEDIR)/extension.cpp -c --no-warnings
	@$(GXX) $(WARGS) -shared rsgl.o draw.o collide.o other.o device.o xinput.o rsal.o extension.o $(WLIBS) -static -o libRSGL.lib
	make windowsCompile
	
help:
	@echo "make help : runs this help tab"
	@echo "make : runs both build and install"
	@echo "make build : builds libRSGL.so in the local dir"
	@echo "make install : installs the build into /usr/lib, then removes in, runs buld if it's not already build"
	@echo "make update : update preexisting RSGL"
	@echo "make uninstall : uninstall RSGL"