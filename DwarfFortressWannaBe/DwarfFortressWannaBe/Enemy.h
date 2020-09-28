// needed for random movement
#ifndef MAX_ENEMY_MOVEMENT
#define MAX_ENEMY_MOVEMENT 15
#endif

#pragma once
#include "Commons.h"
#include <time.h>
#include <vector>
using namespace std;


class Enemy
{
public:
	Enemy(int row, int col, int health, int damage, int armor);
	~Enemy();
	
	// return enemy's location
	Location GetLocation();
	// return enemy's life status
	LifeStatus GetLifeStatus();
	// set enemy's life status
	void SetLifeStatus(LifeStatus status);
	// move enemy
	void Move(vector<vector<char>>& map);
	// return enemy's weapon damage
	int GetDamage();
	// return enemy's armor points
	int GetArmorPoints();
	// take hit
	int ReceiveDamage(int damage);
	// return health left
	int GetHealth();
	
private:
	Location location;
	// vector of items - one weapon and one armor
	vector<Item*> items;
	LifeStatus lifeStatus;
	int health;
	// movement counter
	int moveCnt;

	// set enemy to new location if possible
	bool MovementDecision(int addRow, int addCol, vector<vector<char>>& map);
};