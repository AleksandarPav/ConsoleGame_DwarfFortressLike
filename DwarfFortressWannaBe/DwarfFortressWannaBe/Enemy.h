// needed for random movement
#ifndef MAX_ENEMY_MOVEMENT
#define MAX_ENEMY_MOVEMENT 10
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
	Location GetLocation();
	LifeStatus GetLifeStatus();
	void SetLifeStatus(LifeStatus status);
	void Move(vector<vector<char>>& map);
	int GetDamage();
	int GetArmorPoints();
	int ReceiveDamage(int damage);
	
private:
	Location location;
	vector<Item*> items;
	int health;
	LifeStatus lifeStatus;
	bool MovementDecision(int addRow, int addCol, vector<vector<char>>& map);
	int moveCnt;
};