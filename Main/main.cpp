//Assignment 2
//COMP 369
//Ball Breaker Game
//By Marcello Kuenzler
//3140074
//December 19, 2019

#include <string>
#include <vector>
#include "sprite.h"
#include "datafile.h"
#include <allegro.h>

//Define basic stats
#define WHITE makecol(255, 255, 255)
#define BLACK makecol(0, 0, 0)
#define RED makecol(255, 0, 0)
#define SCREENW 800
#define SCREENH 600
#define BRICK_W 32
#define BRICK_H 12
#define ROW_W 23
#define NUMLIVES 3
#define PSPEED 2
#define PUPSPEED 0.3
#define STARTBSPEED 0.8
#define PLUSBSPEED 0.1
#define BGVOLUME 100
#define SEVOLUME 255
#define R_TIMER 10000

//Declare basic stats, bitmaps and sounds
int score;
int level;
int lives;
int numballs;
int numbricks;
int rpowerup;
int rpoweruptimer;
double bspeed;
volatile int musiccounter = 0;
volatile int counter = 0;
std::vector<sprite*> sprites;
DATAFILE* data;
sprite* paddle;
BITMAP* startimage;
BITMAP* buffer;
BITMAP* bg;
BITMAP* help;
BITMAP* again;
MIDI* bmusic;
SAMPLE* sample;
int panning = 128;
int pitch = 1000;
int musicplaying = 1;

//Function to generate a ball
//It takes a location (x,y) and a speed(x,y) as two ints and two doubles
void ball_generator(int x, int y, double vx, double vy) {
	//Only generate a ball if there isn't already three in play
	if (numballs < 3) numballs++;
	else return;
	sprite *ball = new sprite();
	ball->image = (BITMAP*)data[RollingBall].dat;
	ball->x = x;
	ball->y = y;
	ball->width = 16;
	ball->height = 16;
	ball->animcolumns = 5;
	ball->velx = vx;
	ball->vely = vy;
	ball->totalframes = 28;
	ball->type = "ball";
	sprites.push_back(ball);
}

//Function to generate a powerup
//It takes a location (x,y) as two ints
void powerup_generator(int x, int y) {
	sprite *powerup = new sprite();
	//Randomly assign the powerup one of three benefits
	int r = (rand() % 3) + 1;
	if (rpowerup == 1) r = 1;
	if (r == 1) {
		powerup->image = (BITMAP*)data[BluePowerUp].dat;
		powerup->type = "bpowerup";
	}
	else if (r == 2) {
		powerup->image = (BITMAP*)data[RedPowerUp].dat;
		powerup->type = "rpowerup";
	}
	else if (r == 3) {
		powerup->image = (BITMAP*)data[YellowPowerUp].dat;
		powerup->type = "ypowerup";
	}
	powerup->x = x;
	powerup->y = y;
	powerup->width = 32;
	powerup->height = 32;
	powerup->vely = PUPSPEED;
	powerup->animcolumns = 2;
	powerup->totalframes = 4;
	sprites.push_back(powerup);
}

//Function to bounce a ball against an object
//It takes two sprites, the ball and the object it is bouncing against
void ball_bounce(sprite* s1, sprite* s2) {
	//If the other object is a brick
	if (s2->type == "brick") {
		//Determine which side or corner the ball is coming from and bounce accordingly
		if (s1->inside(s1->x, s1->y, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
			if (s1->inside(s1->x + 1, s1->y, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->vely *= -1;
				s1->y += 1;
				s1->animdir *= -1;
			}
			else if (s1->inside(s1->x, s1->y + 1, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->velx *= -1;
				s1->x += 1;
				s1->animdir *= -1;
			}
			else {
				s1->vely *= -1;
				s1->y += 1;
				s1->velx *= -1;
				s1->x += 1;
				s1->animdir *= -1;
			}
		}
		else if (s1->inside(s1->x + s1->width, s1->y + s1->height, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
			if (s1->inside(s1->x + s1->width - 1, s1->y + s1->height, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->vely *= -1;
				s1->y -= 1;
				s1->animdir *= -1;
			}
			else if (s1->inside(s1->x + s1->width, s1->y + s1->height - 1, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->velx *= -1;
				s1->x -= 1;
				s1->animdir *= -1;
			}
			else {
				s1->vely *= -1;
				s1->y -= 1;
				s1->velx *= -1;
				s1->x -= 1;
				s1->animdir *= -1;
			}
		}
		else if (s1->inside(s1->x, s1->y + s1->height, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
			if (s1->inside(s1->x - 1, s1->y + s1->height, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->vely *= -1;
				s1->y -= 1;
				s1->animdir *= -1;
			}
			else if (s1->inside(s1->x, s1->y + s1->height - 1, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->velx *= -1;
				s1->x += 1;
				s1->animdir *= -1;
			}
			else {
				s1->vely *= -1;
				s1->y -= 1;
				s1->velx *= -1;
				s1->x += 1;
				s1->animdir *= -1;
			}
		}
		else if (s1->inside(s1->x + s1->width, s1->y, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
			if (s1->inside(s1->x + s1->width - 1, s1->y, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->vely *= -1;
				s1->y += 1;
				s1->animdir *= -1;
			}
			else if (s1->inside(s1->x + s1->width, s1->y - 1, s2->x, s2->y, s2->x + s2->width, s2->y + s2->height)) {
				s1->velx *= -1;
				s1->x -= 1;
				s1->animdir *= -1;
			}
			else {
				s1->vely *= -1;
				s1->y += 1;
				s1->velx *= -1;
				s1->x -= 1;
				s1->animdir *= -1;
			}
		}
	}
	//If the other object is a paddle
	else if (s2->type == "paddle") {
		//It will always be hitting from the top, so bounce back up, accounting for if the paddle is moving
		s1->vely *= -1;
		s1->y -= 1;
		double spd = 0;
		spd = (rand() % int(bspeed * 10)) + 1;
		spd /= 10;
		if (spd < 0.2) spd = 0.2;
		if (s2->velx > 0) s1->velx = spd;
		else if (s2->velx < 0) s1->velx = spd * -1;
		s1->animdir *= -1; 
	}
}

//Function to collide two objects and determine the outcome
//It takes two objects
void collide(sprite* s1, sprite* s2) {
	//If the first object is a powerup
	if (s1->type == "bpowerup" || s1->type == "rpowerup" || s1->type == "ypowerup") {
		//And the second is as well, nothing happens
		if (s2->type == "bpowerup" || s2->type == "rpowerup" || s2->type == "ypowerup") return;
		//Same if the second is a ball
		else if (s2->type == "ball") return;
		//And if its a brick
		else if (s2->type == "brick") return;
		//If its a paddle, then the powerup is collected
		else if (s2->type == "paddle") {
			//If it's a blue powerup, a life is gained
			if (s1->type == "bpowerup") {
				lives++;
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)));
			}
			//If it's a red powerup, the double paddle width is activated
			else if (s1->type == "rpowerup" && rpowerup == 0) {
				rpowerup = 1;
				rpoweruptimer = R_TIMER;
				paddle = new sprite();
				paddle->image = (BITMAP*)data[DoublePaddle].dat;
				paddle->x = s2->x - 16;
				paddle->y = s2->y;
				paddle->width = 128;
				paddle->height = 8;
				paddle->type = "paddle";
				sprites.push_back(paddle);
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)));
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)));
			}
			//If it's a yellow powerup, a ball is generated above the paddle
			else if (s1->type == "ypowerup") {
				ball_generator(sprites.at(std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)))->x + 16, sprites.at(std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)))->y - 300, 0, bspeed);
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)));
			}
		}
	}
	//If the first object is a ball
	if (s1->type == "ball") {
		//And the second is a powerup, nothing happens
		if (s2->type == "bpowerup" || s2->type == "rpowerup" || s2->type == "ypowerup") return;
		//Same if it's another ball
		else if (s2->type == "ball") return;
		//If it's a paddle, bounce the ball
		else if (s2->type == "paddle") ball_bounce(s1, s2);
		//And if it's a brick, bounce the ball and break the brick
		else if (s2->type == "brick") {
			sample = (SAMPLE*)data[Shatter].dat;
			play_sample(sample, SEVOLUME, panning, pitch, FALSE);
			s2->animdir = 1;
			ball_bounce(s1, s2);
		}
	}
	//If the first object is a paddle
	if (s1->type == "paddle") {
		//And the second object is a ball, bounce the ball
		if (s2->type == "ball") ball_bounce(s2, s1);
		//Or if it's a powerup, collect it
		else if (s2->type == "bpowerup" || s2->type == "rpowerup" || s2->type == "ypowerup") {
			if (s2->type == "bpowerup") {
				lives++;
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)));
			}
			else if (s2->type == "rpowerup") {
				rpowerup = 1;
				rpoweruptimer = R_TIMER;
				paddle = new sprite();
				paddle->image = (BITMAP*)data[DoublePaddle].dat;
				paddle->x = s1->x - 16;
				paddle->y = s1->y;
				paddle->width = 128;
				paddle->height = 8;
				paddle->type = "paddle";
				sprites.push_back(paddle);
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)));
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)));
			}
			else if (s2->type == "ypowerup") {
				ball_generator(sprites.at(std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)))->x + 16, sprites.at(std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s1)))->y - 300, 0, bspeed);
				sprites.erase(sprites.begin() + std::distance(sprites.begin(), std::find(sprites.begin(), sprites.end(), s2)));
			}
		}
	}
	//If the first object is a brick
	if (s1->type == "brick") {
		//Ignore powerups
		if (s2->type == "bpowerup" || s2->type == "rpowerup" || s2->type == "ypowerup") return;
		//And bounce balls and then break
		else if (s2->type == "ball") {
			sample = (SAMPLE*)data[Shatter].dat;
			play_sample(sample, SEVOLUME, panning, pitch, FALSE);
			s1->animdir = 1;
			ball_bounce(s2, s1);
		}
	}
}

//Function to check collisions within the list of objects
//It takes no parameters
void checkcollisions() {
	//Loop through the list of objects
	for (int n1 = 0; n1 < static_cast<int>(sprites.size()); n1++) {
		//Loop through the list of objects for each object
		for (int n2 = 0; n2 < static_cast<int>(sprites.size()); n2++) {
			//If the objects are not the same object, and they are in collision, run the collision between them
			//If either are destroyed, lower the current count within that loop to account for them not being in it anymore
			if (n1 != n2 && sprites.at(n1)->collided(sprites.at(n2), -1)) {
				int m1 = 0;
				int m2 = 0;
				if (sprites.at(n1)->type == "bpowerup" || sprites.at(n1)->type == "rpowerup" || sprites.at(n1)->type == "ypowerup") {
					if (sprites.at(n2)->type == "paddle") m1 = 1;
				}
				if (sprites.at(n1)->type == "paddle") {
					if (sprites.at(n2)->type == "bpowerup" || sprites.at(n2)->type == "rpowerup" || sprites.at(n2)->type == "ypowerup") m2 = 1;
				}
				collide(sprites.at(n1), sprites.at(n2));
				if (m1 == 1) n1--;
				if (m2 == 1) n2--;
			}
		}
	}
	//Then check for collisions between objects and walls
	//If an object hits either wall or the ceiling, bounce it
	//If an object hits the floor, destroy it
	for (int n = 0; n < static_cast<int>(sprites.size()); n++) {
		if (sprites.at(n)->x < 0)
		{
			sprites.at(n)->x = 0;
			sprites.at(n)->velx *= -1;
			sprites.at(n)->animdir *= -1;
		}

		else if (sprites.at(n)->x > SCREENW - sprites.at(n)->width)
		{
			sprites.at(n)->x = SCREENW - sprites.at(n)->width;
			sprites.at(n)->velx *= -1;
			sprites.at(n)->animdir *= -1;
		}

		if (sprites.at(n)->y < 0)
		{
			sprites.at(n)->y = 0;
			sprites.at(n)->vely *= -1;
			sprites.at(n)->animdir *= -1;
		}

		else if (sprites.at(n)->y > SCREENH - sprites.at(n)->height)
		{
			if (sprites.at(n)->type == "ball") {
				sprites.erase(sprites.begin() + n);
				n--;
				numballs--;
			}
			else if (sprites.at(n)->type == "bpowerup") {
				sprites.erase(sprites.begin() + n);
				n--;
			}
			else if (sprites.at(n)->type == "rpowerup") {
				sprites.erase(sprites.begin() + n);
				n--;
			}
			else if (sprites.at(n)->type == "ypowerup") {
				sprites.erase(sprites.begin() + n);
				n--;
			}
			else {
				sprites.at(n)->y = SCREENH - sprites.at(n)->height;
				sprites.at(n)->vely *= -1;
				sprites.at(n)->animdir *= -1;
			}
		}
	}
}

//Function to build a row of bricks
//It takes a row number
void build_row(int row) {
	sprite* br;
	//For each brick that makes up the row, determined by the row width basic stat
	//Create a brick at the appropriate spot and give it a random color
	for (int i = 0; i < ROW_W; i++) {
		br = new sprite();
		int r = (rand() % 3) + 1;
		if (r == 1) br->image = (BITMAP*)data[BlueBrick].dat;
		else if (r == 2) br->image = (BITMAP*)data[GreenBrick].dat;
		else if (r == 3) br->image = (BITMAP*)data[RedBrick].dat;
		else if (r == 4) br->image = (BITMAP*)data[YellowBrick].dat;
		br->x = i * (BRICK_W + 2) + 8;
		br->y = (row + 1) * BRICK_H * 3;
		br->width = BRICK_W;
		br->height = BRICK_H;
		br->animcolumns = 2;
		br->animdir = 0;
		br->totalframes = 13;
		br->type = "brick";
		sprites.push_back(br);
		numbricks++;
	}
}

//Function to build level 1
//It takes no parameters
void level_1() {
	//Create one row
	for (int i = 0; i < 1; i++) {
		build_row(i);
	}
}

//Function to build level 2
//It takes no parameters
void level_2() {
	//Create two rows
	for (int i = 0; i < 2; i++) {
		build_row(i);
	}
}

//Function to build level 3
//It takes no parameters
void level_3() {
	//Create three rows
	for (int i = 0; i < 3; i++) {
		build_row(i);
	}
}

//Function to build level 4
//It takes no parameters
void level_4() {
	//Create four rows
	for (int i = 0; i < 4; i++) {
		build_row(i);
	}
}

//Function to build level 5
//It takes no parameters
void level_5() {
	//Create a random number of rows, between one and eight
	//Row one will always be created, otherwise it is random if a row is created
	for (int i = 0; i < 8; i++) {
		if (i == 0) build_row(i);
		else if (rand() % 2) build_row(i);
	}
}

//Function to draw the screen
//It takes no parameters
void draw_screen() {
	//Erase the buffer by drawing the background
	blit(bg, buffer, 0, 0, 0, 0, SCREENW, SCREENH);
	//If the double width powerup is active, decrease the timer
	//If the time is done, turn the powerup off
	if (rpoweruptimer > 0) {
		rpoweruptimer--;
		if (rpoweruptimer == 0) {
			rpowerup = 0;
			paddle = new sprite();
			for (int n = 0; n < static_cast<int>(sprites.size()); n++) {
				if (sprites.at(n)->type == "paddle") {
					paddle->image = (BITMAP*)data[Paddle].dat;
					paddle->x = sprites.at(n)->x + 16;
					paddle->y = sprites.at(n)->y;
					paddle->width = 64;
					paddle->height = 8;
					paddle->type = "paddle";
					sprites.erase(sprites.begin() + n);
				}
			}
			sprites.push_back(paddle);
		}
	}
	//For each object, update its position and animation
	//If the object is a brick at the end of its break animation, destroy it
	//And increase score, lower number of bricks and randomly drop a powerup
	for (int n = 0; n < static_cast<int>(sprites.size()); n++) {
		sprites.at(n)->updatePosition();
		sprites.at(n)->updateAnimation();
		if (sprites.at(n)->type == "brick" && sprites.at(n)->curframe == sprites.at(n)->totalframes - 1) {
			int pup = 0;
			pup = (rand() % (9 * level));
			if (pup == 0) powerup_generator(sprites.at(n)->x, sprites.at(n)->y + 16);
			sprites.erase(sprites.begin() + n);
			numbricks--;
			score++;
			n--;
		}
	}
	//Check for collisions at new positions
	checkcollisions();
	//For each object, draw them to the buffer
	for (int n = 0; n < static_cast<int>(sprites.size()); n++) {
		sprites.at(n)->drawframe(buffer);
	}
	//Draw the level information to the bottom of the buffer
	textprintf_ex(buffer, font, 10, 590, BLACK, -1, "Level: %d", level);
	textprintf_ex(buffer, font, 110, 590, BLACK, -1, "Score: %d", score);
	textprintf_ex(buffer, font, 210, 590, BLACK, -1, "Lives: %d", lives);
	textprintf_ex(buffer, font, 310, 590, BLACK, -1, "Bricks Left: %d", numbricks);
	if (rpowerup == 1) textout_ex(buffer, font, "DOUBLE PADDLE ACTIVE", 600, 590, RED, -1);
	//Draw the buffer to the screen
	acquire_screen();
	masked_blit(buffer, screen, 0, 0, 0, 0, SCREENW, SCREENH);
	release_screen();
}

//Function to display the help screen
//It takes no parameters
	void gethelp() {
		//Draw the static help screen
		acquire_screen();
		masked_blit(help, screen, 0, 0, 0, 0, SCREENW, SCREENH);
		release_screen();
		//Wait the player to proceed or quit
		while (key[KEY_SPACE]);
		while (!key[KEY_SPACE]) if (key[KEY_ESC]) allegro_exit();
}

//Function to create a level
//It takes no parameters
void create_level() {
	//Clear any previous objects
	sprites.clear();
	//Create the bricks based on the current level
	if (level == 1) level_1();
	else if (level == 2) level_2();
	else if (level == 3) level_3();
	else if (level == 4) level_4();
	else if (level < 4) level_5();
	//While there are still lives or bricks, play the game
	while (!(lives < 1) && !(numbricks < 1)) {
		//Create the paddle and ball
		paddle = new sprite();
		paddle->image = (BITMAP*)data[Paddle].dat;
		paddle->x = 368;
		paddle->y = 550;
		paddle->width = 64;
		paddle->height = 8;
		paddle->type = "paddle";
		sprites.push_back(paddle);
		numballs = 0;
		ball_generator(400, 400, 0, bspeed);
		//While the player has not quit the game, check for keyboard controls
		while (!key[KEY_ESC]) {
			//Control the paddle using left and right
			if (key[KEY_LEFT] && !key[KEY_RIGHT]) paddle->velx = PSPEED * -1;
			else if (!key[KEY_LEFT] && key[KEY_RIGHT]) paddle->velx = PSPEED;
			else if (!key[KEY_LEFT] && !key[KEY_RIGHT]) paddle->velx = 0;
			else if (key[KEY_LEFT] && key[KEY_RIGHT]) paddle->velx = 0;
			//Pause or play the music using ctrl+M
			if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) if (key[KEY_M]) {
				if (musicplaying == 0 && musiccounter > 1000) {
					midi_resume();
					musicplaying = 1;
					musiccounter = 0;
				}
				else if (musicplaying == 1 && musiccounter > 1000) {
					midi_pause();
					musicplaying = 0;
					musiccounter = 0;
				}
			}
			//Activate the help screen using ctrl+H
			if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) if (key[KEY_H]) gethelp();
			//Draw the screen
			draw_screen();
			//If all balls or bricks are gone, loop is over
			if (numballs == 0) break;
			if (numbricks == 0) break;
		}
		//After exiting loop, determine game state
		//If there are still balls and bricks, player chose to quit
		if (numballs != 0 && numbricks != 0) allegro_exit();
		//If there are no balls, player lost
		if (numballs == 0) {
			//Destroy old paddle and any falling powerups, reset powerup if active and lose a life
			for (int n = 0; n < static_cast<int>(sprites.size()); n++) {
				if (sprites.at(n)->type == "paddle") {
					sprites.erase(sprites.begin() + n);
					n--;
				}
				else if (sprites.at(n)->type == "bpowerup" || sprites.at(n)->type == "rpowerup" || sprites.at(n)->type == "ypowerup") {
					sprites.erase(sprites.begin() + n);
					n--;
				}
			}
			lives--;
			rpowerup = 0;
			rpoweruptimer = 0;
		}
	}
	//Draw the background to the screen
	blit(bg, buffer, 0, 0, 0, 0, SCREENW, SCREENH);
	acquire_screen();
	masked_blit(buffer, screen, 0, 0, 0, 0, SCREENW, SCREENH);
	release_screen();
}

//Function to reset the stats when a new game is started
//It takes no parameters
void reset_stats() {
	//Resets all basic stats to default settings
	score = 0;
	level = 1;
	lives = NUMLIVES;
	bspeed = STARTBSPEED;
	numballs = 0;
	numbricks = 0;
	rpowerup = 0;
	rpoweruptimer = 0;
}

//Function to start a game
//It takes no parameters
void start_game() {
	//Keep going until the player quits
	int play = 1;
	while (play == 1) {
		//Reset the stats at the start of each game
		reset_stats();
		while (lives > 0) {
			create_level();
			//If there are no bricks left the level is complete
			//Proceed to the next and increase difficulty
			if (numbricks == 0) {
				level++;
				bspeed = bspeed + PLUSBSPEED;
			}
		}
		//When all lives are lost, display game over screen and wait for player to play again or quit
		acquire_screen();
		masked_blit(again, screen, 0, 0, 0, 0, SCREENW, SCREENH);
		release_screen();
		while (!key[KEY_SPACE]) if (key[KEY_ESC]) allegro_exit();
	}
}

//Function to update the counters
//It takes no parameters
void update_counter() {
	//Update all counters
	counter++;
	musiccounter++;
}

int main(void) {
	//Initialize all needed allegro functions
	allegro_init();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREENW, SCREENH, 0, 0);
	install_keyboard();
	install_mouse();
	install_timer();
	data = load_datafile("datafile.dat");
	LOCK_VARIABLE(counter);
	LOCK_VARIABLE(musiccounter);
	LOCK_FUNCTION(update_counter);
	install_keyboard();
	install_int(update_counter, 1);
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "");
	show_mouse(screen);
	srand(time(NULL));
	//Setup the bitmaps and music
	buffer = create_bitmap(SCREENW, SCREENH);
	set_volume(SEVOLUME, BGVOLUME);
	bmusic = (MIDI*)data[Music].dat;
	play_midi(bmusic, 1);
	startimage = (BITMAP*)data[Startup].dat;
	bg = (BITMAP*)data[Background].dat;
	help = (BITMAP*)data[Help].dat;
	again = (BITMAP*)data[PlayAgain].dat;
	clear_bitmap(screen);
	//Draw the start screen with title to the screen
	blit(startimage, screen, 0, 0, 0, 0, SCREENW, SCREENH);
	//Wait for player to proceed or quit, then display help screen
	while (!key[KEY_SPACE]) if (key[KEY_ESC]) allegro_exit();
	clear_bitmap(screen);
	gethelp();
	start_game();
	allegro_exit();
	return 0;
}

END_OF_MAIN()