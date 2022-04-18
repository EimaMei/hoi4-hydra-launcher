#ifndef RSGL
#include "../../include/include/windows/rsgl.hpp"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h"

int RSGL::drawRect(RSGL::rect r, RSGL::color c, bool solid/*=true*/, bool dotted/*=false*/, int border_size/*=3*/, RSGL::color c2/*={1991}*/, bool uptodown/*=true*/) {
    float i = RSGL::win.r.width/2*1.0f; //Convert RSGL::win.r int coordinates to OpenGL float coordinates
    float  x = (r.x/i)-1.0f;
    float  x2 =((r.x+r.width)/i)-1.0f;
    i = RSGL::win.r.length/2*1.0f;
    float  y = (-(r.y)/i)+1.0f;
    float  y2 =(-(r.y+r.length)/i)+1.0f;
    
    if (solid) { glBegin(GL_POLYGON); }  else { glBegin(GL_LINE_LOOP); } //For unfilled rectangles
        if (c2.r != 1991 && !uptodown) {glColor4f(c2.r/255.0, c2.g/255.0, c2.b/255.0, c2.a/255.0);}
        else {glColor4f(c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0);}
        glVertex2f(x,  y);
        glVertex2f(x2, y);
        if (c2.r != 1991 && uptodown) {glColor4f(c2.r/255.0, c2.g/255.0, c2.b/255.0, c2.a/255.0);}
        else {glColor4f(c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0);}
        glVertex2f(x2, y2);
        glVertex2f(x, y2);
    glEnd();

    return 0;
}


int RSGL::drawTriangle(RSGL::triangle t, RSGL::color c, bool solid/*=true*/) {
    float i = RSGL::win.r.width/2*1.0f;
    float  x  = (t.x/i)-1.0f;
    float  x2 = ((t.x+t.width)/i)-1.0f;
    i = RSGL::win.r.length/2*1.0f;
    float  y = (-(t.y)/i)+1.0f;
    float  y2 =(-(t.y+t.length)/i)+1.0f;
    
    if (solid) { glBegin(GL_POLYGON); }
    else { glBegin(GL_LINE_LOOP);}
        glColor4f(c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0);
        glVertex2f((x+x2)/2.0f,  y);
        glVertex2f(x, y2);
        glVertex2f(x2, y2);
    glEnd();

    return 1;
}

int RSGL::drawCircle(RSGL::circle c, RSGL::color col, bool solid/*=true*/, int border_size/*=3*/) {
    float i = RSGL::win.r.width/2*1.0f;
    float  x2 = (c.x/i)-1.0f;
    i = RSGL::win.r.length/2*1.0f;
    float  y2 = (-(c.y)/i)+1.0f;

    i = RSGL::win.r.width/2*1.0f;
    float  r = ((-(c.radius)/i)-1.0f);
    if (!solid){
        glBegin(GL_LINE_LOOP);
            glColor4f(col.r/255.0, col.g/255.0, col.b/255.0,col.a/255.0);
            for(int ii = 0; ii < 60; ii++){
                float theta = 2.0f * 3.1415926f * float(ii) / float(60);
    
                float x = r * cosf(theta);
                float y = r * sinf(theta);
    
                glVertex2f(x + x2, y + y2);
            }
        glEnd();
    }
    if (solid){
        glBegin(GL_TRIANGLE_FAN);
            glColor4f(col.r/255.0,col.g/255.0,col.b/255.0,col.a/255.0);
            glVertex2f((x2+r)/2,(y2+r)/2);
    
            for (float angle=1.0f;angle<361.0f;angle+=0.2){
                float x3 = x2+sin(angle)*r;
                float y3 = y2+cos(angle)*r;
                glVertex2f(x3,y3);
            }   
        glEnd();
    }

    return 1;
}

int RSGL::drawText(std::string text, RSGL::rect r, const char* font, RSGL::color col){ return 0; }

RSGL::color RSGL::setGrayscale(RSGL::color c) {int num=(c.r+c.g+c.b)/3;return {num,num,num, c.a};}
RSGL::color RSGL::setInverted(RSGL::color c) {return {255-c.r, 255-c.g, 255-c.b, c.a};}

RSGL::image::image(const char * Filename, rect R, bool load/*=true*/) {
    if (load) {
        data = stbi_load(Filename, &original_size.x, &original_size.y, 0, 4); 

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,original_size.x, original_size.y,0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        //free(data);
    }
    filename=Filename; r=R; loaded=load;
}

void RSGL::image::load() {
    data = stbi_load(filename, &original_size.x, &original_size.y, 0, 4); 

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,original_size.x, original_size.y,0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    loaded=true;
}

void RSGL::button::load(const char* filename, RSGL::rect r, bool load_regardless/*=false*/) {
    if (i.filename != filename || load_regardless) {
        i.filename=filename;
        i.r=r;
        i.load();
    }
}

int RSGL::image::draw() {
    float i = win.r.width/2*1.0f; //Convert RSGL::win.r int coordinates to OpenGL float coordinates
    float  x = (r.x/i)-1.0f;
    float  x2 =((r.x+r.width)/i)-1.0f;
    i = win.r.length/2*1.0f;
    float  y = (-(r.y)/i)+1.0f;
    float  y2 =(-(r.y+r.length)/i)+1.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2f(x,  y);
        glTexCoord2i(1, 0); glVertex2f(x2, y);
        glTexCoord2i(1, 1);  glVertex2f(x2, y2);
        glTexCoord2i(0, 1); glVertex2f(x, y2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return 0;
}
/*
int RSGL::drawPixels(RSGL::rect r, std::vector<RSGL::color> pixels) {
    int xx=-1;

    for (int i=0; i<p.size(); i++) { if (pixels.size() == p[i].first.size()) { xx=i; break; }  }
    // Quick and dirty way to check if the pixels are the same. Isn't 100% accurate but it's better than
    // iterating through every element in the vector.
    //if (xx==-1) { RSGL::loadImage({"9607", r}, pixels); xx=p.size()-1; }
    r.y += RSGL::win.owr.x;

    float i = win.r.width/2*1.0f; //Convert RSGL::win.r int coordinates to OpenGL float coordinates
    float  x = (r.x/i)-1.0f;
    float  x2 =((r.x+r.width)/i)-1.0f;
    i = win.r.length/2*1.0f;
    float  y = (-(r.y)/i)+1.0f;
    float  y2 =(-(r.y+r.length)/i)+1.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, p[xx].second.tex);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2f(x,  y);
        glTexCoord2i(1, 0); glVertex2f(x2, y);
        glTexCoord2i(1, 1);  glVertex2f(x2, y2);
        glTexCoord2i(0, 1); glVertex2f(x, y2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return 0;
}*/

void RSGL::button::draw(RSGL::rect r/*={}*/) {
    if (r.x+r.width+r.length+r.y != 0) rr=r;
    switch (ID) {
        case RECTANGLE: RSGL::drawRect(rr, cc); break;
        case TRIANGLE: {RSGL::triangle f ={rr.x, rr.y, rr.width, rr.length}; RSGL::drawTriangle(f, cc); break;}
        case CIRCLE: RSGL::drawCircle(cirr, cc); break;
        case IMAGE: {i.r=rr; i.draw(); break;}
        default: break;
    }
}