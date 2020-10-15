#ifndef _SPRITE_H
#define _SPRITE_H 1

#include <string>
#include <allegro.h>

class sprite {
public:
	double x, y;
	int width, height;
	double velx, vely;
	int curframe, totalframes, animdir;
	int framecount, framedelay;
	int animcolumns;
	BITMAP* image;
	std::string type;

public:
	sprite();
	void load(const char* filename);
	void drawframe(BITMAP* dest);
	void updatePosition();
	void updateAnimation();
	int inside(int x, int y, int left, int top, int right, int bottom);
	int collided(sprite* other = NULL, int shrink = 5);
};

#endif