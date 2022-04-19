#ifndef RSGL
#include "../../include/include/windows/rsgl.hpp"
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION 
#include "deps/stb_truetype.h" 
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

char ttf_buffer[1<<25];
char ttf_buffer2[1<<25];

void drawTextRAW(std::string text, RSGL::circle r, const char* Font, RSGL::color col, RSGL::drawable d){
    int high=0;
    FILE* f = fopen(Font, "rb");
    glBegin(GL_POINTS); 
    glColor4f(col.r/255.0, col.g/255.0, col.b/255.0,col.a/255.0);
    for (int dr=0; dr<2; dr++){
        int L2=0; 
        for (int L=0; L < text.size(); L++){
            stbtt_fontinfo font;
            unsigned char *bitmap;
            int w,h,i,j,c = text.at(L), s = r.radius; L2++;

            if (ttf_buffer != NULL && f != NULL) fread(ttf_buffer, 1, 1<<25, f);

            stbtt_InitFont(&font, (unsigned char*)ttf_buffer, stbtt_GetFontOffsetForIndex((unsigned char*)ttf_buffer,0));
            bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, 0,0);
            if (h > high && !dr) high=h; 
            else if (dr){
                int b=0;
                if (h < high && text.size() > 2) b=high-h;
                if (text.at(L) == ' ') r.x+=10;
                for (j=0; j < h; ++j) {
                    for (i=0; i < w; ++i)
                        if (" .:ioVM@"[bitmap[j*w+i]>>5] != ' '){ 
                            RSGL::point p1 = {r.x+i,r.y+j+b};
                            float i = d.r.width/2*1.0f;
                            float  x = (p1.x/i)-1.0f;
                            i = d.r.length/2*1.0f;
                            float  y = (-(p1.y)/i)+1.0f;
                            glVertex2f(x,y);
                        }
                }  //r.x+=w + bias.at(text.at(L));
                b=0; int w2=w;
                if (L+1 < text.size()) bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), text.at(L+1), &w2, &h, 0,0);
                if (w >= 20 || w2 >= 20 ) b++; 
                if (w >= 32 || w2 >= 32) b++;  
                r.x+=w + b; 
            } 
        }
   } glEnd(); *ttf_buffer=*ttf_buffer2;
}


struct Letter{
    std::string letter;
    int size; int num;
    std::string text;
    const char* font;
};

std::vector<Letter> ls;
std::vector<RSGL::point> points;

void RSGL::drawText(std::string text, RSGL::circle r, const char* Font, RSGL::color col){
    int x; int x2=0; int y;
    std::string textStr = text;
    
    long size;
    unsigned char* fontBuffer;

    FILE* fontFile = fopen(Font, "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile); /* how long is the file ? */
    fseek(fontFile, 0, SEEK_SET); /* reset */
    fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);

    for (int I3=0; I3 < textStr.size(); I3++){
        text = textStr.at(I3);
        bool done=false; int L3=0;
        for (L3; L3 < ls.size(); L3++){
            if (text == ls.at(L3).letter && r.radius == ls.at(L3).size && Font == ls.at(L3).font && ls.at(L3).num == I3 && ls.at(L3).text == textStr){ done=true; break;} 
        }
        if (!done){
            if (textStr.size() > I3+1) text += textStr.at(I3+1);
             /* prepare font */
            stbtt_fontinfo info;
            if (!stbtt_InitFont(&info, fontBuffer, 0)) printf("failed\n");


            int b_w = r.radius*text.size(); /* bitmap width */
            int b_h = r.radius; /* bitmap height */
            int l_h = r.radius; /* line height */

            /* create a bitmap for the phrase */
            unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h , sizeof(unsigned char));
            /* calculate font scaling */
            float scale = stbtt_ScaleForPixelHeight(&info, l_h);

            char* word = (char*)text.data();
            
            x = 0;

            int ascent, descent, lineGap;

            stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

            ascent = roundf(ascent * scale);

            for (int i = 0; i < 1; i++) {
                /* how wide is this character */
                int ax;
                int lsb;
                stbtt_GetCodepointHMetrics(&info, word[i], &ax, &lsb);
                /* (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character word[i].) */

                /* get bounding box for character (may be offset to account for chars that dip above or below the line */
                int c_x1, c_y1, c_x2, c_y2;
                stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
                /* compute y (different characters have different heights */
                y = ascent + c_y1;

                /* advance x */
                x += roundf(ax * scale);

                /* add kerning */
                int kern;
                kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
                x += roundf(kern * scale);
            } 
            std::string ftext; ftext += text.at(0);
            drawTextRAW(ftext,{r.x + x2,r.y+y,r.radius},Font,col,win);
            ls.insert(ls.end(),{ftext,r.radius,I3,textStr,Font});
            points.insert(points.end(),{x2,y});
            x2+=x;
        }
        else{
            std::string ftext; ftext += text.at(0);
            drawTextRAW(ftext,{r.x + points.at(L3).x ,r.y+ points.at(L3).y ,r.radius},Font,col,win);
        }
    }
}


RSGL::Text::Text(std::string txt, RSGL::circle r, const char* font, RSGL::color col, bool draw){rect=r; text=txt; c=col; f=font; if (draw) RSGL::drawText(txt,r,font,col);}

void RSGL::Text::draw(){ RSGL::drawText(text,rect,f,c); }