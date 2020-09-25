// weapon, armor, health potion, magic mushroom
#ifndef NUM_OF_ITEMS
#define NUM_OF_ITEMS 4
#endif

#pragma once
#include "Item.h"
#include <vector>
#include <string>
using namespace std;

enum KeyPressed
{
	Key_ESC = 27,				// quit game
	Key_ARROW_UP = 256 + 72,	// move up
	Key_ARROW_DOWN = 256 + 80,	// move down
	Key_ARROW_LEFT = 256 + 75,	// move left
	Key_ARROW_RIGHT = 256 + 77,	// move right
	Key_W = 87,					// move up
	Key_w = 119,				// move up
	Key_S = 83,					// move down
	Key_s = 115,				// move down
	Key_A = 65,					// move left
	Key_a = 97,					// move left
	Key_D = 68,					// move right
	Key_d = 100,				// move right
	Key_V = 86,					// view map
	Key_v = 118,				// view map
	Key_SPACE = 32,				// take items from chest
	// TODO	Key_J = 74,					// save configuration
	// TODO	Key_j = 106,				// save configuration
	Key_H = 72,					// drink health potion if available
	Key_h = 104,				// drink health potion if available
	Key_M = 77,					// eat magic mushroom if available
	Key_m = 109,				// eat magic mushroom if available
	Key_C = 67,					// choose package of items
	Key_c = 99,					// choose package of items
	Key_Q = 81,					// suicide
	Key_q = 113					// suicide
};

enum Turn
{
	Turn_PLAYER,	// player's turn
	Turn_ENEMY,		// enemy's turn
	Turn_COMBAT		// nobody's turn, combat is happening
};

enum LifeStatus
{
	Status_ALIVE,	// subject is alive
	Status_DEAD		// subject is dead
};

enum ItemRow
{
	Row_WEAPON = 0,			// in package of items, weapon comes first
	Row_ARMOR = 1,			// in package of items, armor comes second
	Row_HEALTH_POTION = 2,	// in package of items, health potion comes third
	Row_MAGIC_MUSHROOM = 3	// in package of items, magic mushroom comes fourth
};

enum ChestStatus
{
	Chest_WAS_EMPTY,	// chest was empty in time of looting
	Chest_WAS_FULL		// chest was full in time of looting
};

enum CombatResult
{
	Result_WON,		// all enemies died
	Result_LOST,	// player died
	Result_CONTINUE	// no one died
};

bool CheckLifeStatus(LifeStatus status);

struct Location
{
	Location(int row, int col);
	int row;
	int col;
};

struct Chest
{
	// location and item characteristics
	Chest(int row, int col, int damage, int armor, int health, int boost);
	~Chest();
	Location location;
	vector<Item*> items;
	// remove all items
	void CleanItems();
};

// check if the desired location is valid
bool CheckNewLocation(int newLocationRow, int newLocationCol, vector<vector<char>>& map);