#include "Enemy.h"

// create all the enemies
Enemy::Enemy(int row, int col, int health, int damage, int armor) : location(row, col), health(health)
{
	items.push_back(new Weapon(damage));
	items.push_back(new Armor(armor));

	lifeStatus = Status_ALIVE;
}

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
	srand(time(NULL));
	int numberOfMoves = rand() % MAX_ENEMY_MOVEMENT;
	while (moveCnt < (numberOfMoves))
	{
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

int Enemy::GetDamage()
{
	return items[Row_WEAPON]->GetValue();
}

int Enemy::GetArmorPoints()
{
	return items[Row_ARMOR]->GetValue();
}

int Enemy::ReceiveDamage(int damage)
{
	health -= damage;
	if (health < 0)
		health = 0;

	return health;
}