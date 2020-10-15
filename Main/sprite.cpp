#include "sprite.h"
#include <allegro.h>

sprite::sprite() {
	image = NULL;
	animcolumns = 1;
	x = 0.0f;
	y = 0.0f;
	width = 0;
	height = 0;
	velx = 0.0;
	vely = 0.0;
	curframe = 0;
	totalframes = 1;
	framecount = 0;
	framedelay = 10;
	animdir = 1;
	type = "";
}

void sprite::load(const char* filename) {
	image = load_bitmap(filename, NULL);
}

void sprite::drawframe(BITMAP* dest)
{
	int framex = (curframe % animcolumns) * width;
	int framey = (curframe / animcolumns) * height;
	masked_blit(image, dest, framex, framey, (int)x, (int)y, width, height);
}

void sprite::updatePosition()
{
	x += velx;
	y += vely;
}

void sprite::updateAnimation()
{
	if (++framecount > framedelay)
	{
		framecount = 0;
		curframe += animdir;
		if (curframe < 0) {
			curframe = totalframes - 1;
		}
		if (curframe > totalframes - 1) {
			curframe = 0;
		}
	}
}

int sprite::inside(int x, int y, int left, int top, int right, int bottom)
{
	if (x > left&& x < right && y > top&& y < bottom)
		return 1;
	else
		return 0;
}

int sprite::collided(sprite* other, int shrink)
{
	int wa = (int)x + width;
	int ha = (int)y + height;
	int wb = (int)other->x + other->width;
	int hb = (int)other->y + other->height;

	if (inside((int)x, (int)y, (int)other->x + shrink, (int)other->y + shrink, wb - shrink, hb - shrink)) return 1;
	else if (inside((int)x, ha, (int)other->x + shrink, (int)other->y + shrink, wb - shrink, hb - shrink)) return 1;
	else if (inside(wa, (int)y, (int)other->x + shrink, (int)other->y + shrink, wb - shrink, hb - shrink)) return 1;
	else if (inside(wa, ha, (int)other->x + shrink, (int)other->y + shrink, wb - shrink, hb - shrink)) return 1;
	else return 0;
}