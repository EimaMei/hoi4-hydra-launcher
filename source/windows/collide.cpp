#ifndef SWGL
#include "../../SWGL.hpp"
#endif

int SWGL::CircleCollideCircle(SWGL::circle cir,SWGL::circle cir2){
	float distanceBetweenCircles = (float)sqrt(
	(cir2.x - cir.x) * (cir2.x - cir.x) + 
    (cir2.y - cir.y) * (cir2.y - cir.y)
  	);
	if (distanceBetweenCircles > cir.radius + cir2.radius){return 0;}else{return 1;}
}

int SWGL::CircleCollideRect(SWGL::circle c, SWGL::rect r){
  float testX = c.x; float testY = c.y;

  if (c.x < r.x) {testX = r.x;}  else if (c.x > r.x+r.width) {testX = r.x+r.width;}
  if (c.y < r.y) {testY = r.y;}  else if (c.y > r.y+r.length) {testY = r.y+r.length;} 
  
  return (sqrt( ( (c.x-testX) * (c.x-testX) ) + ( (c.y-testY) *(c.y-testY) ) )  <= c.radius);
}

int SWGL::CircleCollidePoint(SWGL::circle c, SWGL::point p){
	float testX = c.x; float testY = c.y;

  	if (c.x < p.x) {testX = p.x;}  else if (c.x > p.x+1) {testX = p.x+1;}
  	if (c.y < p.y) {testY = p.y;}  else if (c.y > p.y+1) {testY = p.y+1;} 
  
  	return (sqrt( ( (c.x-testX) * (c.x-testX) ) + ( (c.y-testY) *(c.y-testY) ) )  <= c.radius);
}

int SWGL::RectCollidePoint(SWGL::rect r, SWGL::point p){
    if (p.x >= r.x &&  p.x <= r.x + r.width && p.y >= r.y && p.y <= r.y + r.length) return 1;
    return 0;
}

int SWGL::PointCollidePoint(SWGL::point p, SWGL::point p2){
    if (p.x == p2.x && p.y == p2.y) return 1;
    return 0;
}

int SWGL::RectCollideRect(SWGL::rect r, SWGL::rect r2){
    if(r.x + r.width >= r2.x && r.x <= r2.x + r2.width && r.y + r.length >= r2.y && r.y <= r2.y + r2.length) return 1; 
    return 0;
}

int SWGL::ImageCollideRect(SWGL::image img, SWGL::rect r){
    if(img.r.x + r.width >= r.x && img.r.x <= r.x + r.width && img.r.y + img.r.length >= r.y && img.r.y <= r.y + r.length) return 1; 
    return 0;
}

int SWGL::ImageCollideCircle(SWGL::image img, SWGL::circle c){
    SWGL::rect r = img.r;
    float testX = c.x; float testY = c.y;

    if (c.x < r.x) {testX = r.x;}  else if (c.x > r.x+r.width) {testX = r.x+r.width;}
    if (c.y < r.y) {testY = r.y;}  else if (c.y > r.y+r.length) {testY = r.y+r.length;} 
    
    return (sqrt( ( (c.x-testX) * (c.x-testX) ) + ( (c.y-testY) *(c.y-testY) ) )  <= c.radius);
}

int SWGL::ImageCollidePoint(SWGL::image img, SWGL::point p){
    SWGL::rect r = img.r;
    if (p.x >= r.x &&  p.x <= r.x + r.width && p.y >= r.y && p.y <= r.y + r.length) return 1;
    return 0;     
}

int SWGL::ImageCollideImage(SWGL::image img, SWGL::image img2){
    SWGL::rect r = img.r; SWGL::rect r2 = img2.r;
    if(r.x + r.width >= r2.x && r.x <= r2.x + r2.width && r.y + r.length >= r2.y && r.y <= r2.y + r2.length) return 1; 
    return 0;    
}