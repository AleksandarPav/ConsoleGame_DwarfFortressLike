#include "Enemy.h"

// create objects for enemy's items
Enemy::Enemy(int row, int col, int health, int damage, int armor) : location(row, col), health(health)
{
	items.push_back(new Weapon(damage));
	items.push_back(new Armor(armor));

	// initialize it with being alive
	lifeStatus = Status_ALIVE;
}

// clear all pointers
Enemy::~Enemy()
{
	for (size_t i(0); i < items.size(); ++i)
	{
		delete items[i];
		items[i] = nullptr;
	}
}

Location Enemy::GetLocation()
{
	return location;
}

LifeStatus Enemy::GetLifeStatus()
{
	return lifeStatus;
}

void Enemy::SetLifeStatus(LifeStatus status)
{
	if (CheckLifeStatus(status))
		lifeStatus = status;
}

// move some random number of times
void Enemy::Move(vector<vector<char>>& map)
{
	moveCnt = -1;
	//srand(time(NULL));
	int numberOfMoves = rand() % MAX_ENEMY_MOVEMENT;
	// move while movement counter doesn't become big enough
	while (moveCnt < (numberOfMoves))
	{
		// move if any tried location is valid
		if ((MovementDecision(1, 0, map)) or (MovementDecision(-1, 0, map)) or (MovementDecision(0, 1, map)) or (MovementDecision(0, -1, map)))
			++moveCnt;
	}
	
}

// set new location, if valid
bool Enemy::MovementDecision(int addRow, int addCol, vector<vector<char>>& map)
{
	int newLocationRow = location.row + addRow;
	int newLocationCol = location.col + addCol;

	if (CheckNewLocation(newLocationRow, newLocationCol, map))
	{
		location.row = newLocationRow;
		location.col = newLocationCol;
		return true;
	}
	return false;
}

// return enemy's weapon damage
int Enemy::GetDamage()
{
	return items[Row_WEAPON]->GetValue();
}

// return enemy's armor points
int Enemy::GetArmorPoints()
{
	return items[Row_ARMOR]->GetValue();
}

// take hit
int Enemy::ReceiveDamage(int damage)
{
	// no need for heatlh to go below zero
	if ((health - damage) <= 0)
		health = 0;
	// if damage < 0, no need to increase health
	else if ((health - damage) < health)
		health -= damage;

	return health;
}

int Enemy::GetHealth()
{
	return health;
}