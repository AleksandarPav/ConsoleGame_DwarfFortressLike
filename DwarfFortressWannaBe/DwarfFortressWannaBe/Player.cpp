#include "Player.h"

Player::Player(int row, int col, int health) : location(row, col), health(health)
{
	// in the beginning , we want player to feel and be alive
	lifeStatus = Status_ALIVE;
}

Player::~Player()
{
	for (size_t i(0); i < inventory.size(); ++i)
		for (size_t j(0); j < inventory[i].size(); ++j)
		{
			delete inventory[i][j];
			inventory[i][j] = nullptr;
		}
}

Location Player::GetLocation()
{
	return location;
}

void Player::SetLocation(int row, int col)
{
	location.row = row;
	location.col = col;
}

size_t Player::GetInventorySize()
{
	return inventory.size();
}

// each row represents one type of item
void Player::ResizeInventory()
{
	inventory.resize(NUM_OF_ITEMS);
}

ChestStatus Player::FillInventory(int damage, int armor, int health, int boost)
{
	// nothing left in the chest
	if (damage == 0 and armor == 0 and health == 0 and boost == 0)
	{
		cout << endl << "The chest is empty!" << endl;
		return Chest_WAS_EMPTY;
	}
	else
	{
		// rows are enumerated for easier understanding
		inventory[Row_WEAPON].push_back(new Weapon(damage));
		inventory[Row_ARMOR].push_back(new Armor(armor));
		inventory[Row_HEALTH_POTION].push_back(new HealthPotion(health));
		inventory[Row_MAGIC_MUSHROOM].push_back(new MagicMushroom(boost));
		return Chest_WAS_FULL;
	}
}

size_t Player::GetInventoryColumnSize()
{
	// number of packages of items
	if (GetInventorySize() > 0)
		return inventory[0].size();
	else
		return 0;
}

// drinks health potion
void Player::BoostHealth(int h)
{
	health += h;
	if (health > 100)
		health = 100;
}

// eats magic mushroom
void Player::BoostDamage(int d, int idx)
{
	inventory[0][idx] += d;
}

void Player::SetLifeStatus(LifeStatus status)
{
	if (CheckLifeStatus(status))
		lifeStatus = status;
}

LifeStatus Player::GetLifeStatus()
{
	return lifeStatus;
}

void Player::ViewCurrentInventory()
{
	if (IsInventoryEmpty())
		cout << endl << "You have nothing!" << endl;
	else
	{
		cout << endl << "-------Showing all inventory-------" << endl;
		for (size_t col = 0; col < inventory[0].size(); ++col)
		{
			cout << endl << "Package of items no. " << to_string(col + 1) << ":" << endl;
			for (size_t row = Row_WEAPON; row < inventory.size(); ++row)
			{
				inventory[row][col]->displayItem(row);
				cout << inventory[row][col];
			}
		}
	}
}

void Player::Equip()
{
	if (IsInventoryEmpty())
		cout << endl << "You have nothing!" << endl;
	else
	{
		// prompt user to choose package of items
		int packageNumber = -1;

		cout << endl << "List of available packages:" << endl;
		ViewCurrentInventory();
		do
		{
			cout << endl << "Choose a package number!" << endl;
			cin >> packageNumber;
		} while ((packageNumber <= 0) or (packageNumber > GetInventoryColumnSize()));
		cout << endl << "Well done, you can continue the game!" << endl;

		activePackageIdx = packageNumber - 1;
	}
}

bool Player::IsInventoryEmpty()
{
	if (inventory.size() != NUM_OF_ITEMS)
		return true;
	return false;
}

size_t Player::GetActivePackageIdx()
{
	if (!IsInventoryEmpty())
		return activePackageIdx;
	else
		// valid idx has to have smaller value
		return GetInventoryColumnSize(); // has nothing, valid idx can't have this value
}

void Player::DrinkHealthPotion()
{
	int activePackage = GetActivePackageIdx();
	if (activePackage < GetInventoryColumnSize())
	{
		ViewActiveInventory();
		health += inventory[Row_HEALTH_POTION][activePackage]->GetValue();
		if (health > 100)
		{
			// don't let player dring if there's no need
			cout << endl << "No need for health potion!" << endl;
			health -= inventory[Row_HEALTH_POTION][activePackage]->GetValue();
		}
		else
		{
			inventory[Row_HEALTH_POTION][activePackage]->SetValue(0);
			cout << endl << "Your health is now " << health << endl;
		}
	}
	else
		cout << endl << "You have nothing!" << endl;
}

// increase weapon damage
void Player::EatMagicMushroom()
{
	int activePackage = GetActivePackageIdx();
	if (activePackage < GetInventoryColumnSize())
	{
		ViewActiveInventory();
		int newDamageValue = inventory[Row_WEAPON][activePackage]->GetValue() + inventory[Row_MAGIC_MUSHROOM][activePackage]->GetValue();
		inventory[Row_WEAPON][activePackage]->SetValue(newDamageValue);
		inventory[Row_MAGIC_MUSHROOM][activePackage]->SetValue(0);
		cout << endl << "Your new weapon damage is " << newDamageValue << endl;
	}
	else
		cout << endl << "You have nothing!" << endl;
}

// which package of items is chosen
void Player::ViewActiveInventory()
{
	int activePackage = GetActivePackageIdx();
	if (activePackage < GetInventoryColumnSize())
	{
		cout << endl << "Package currently used:" << endl;
		for (size_t row = Row_WEAPON; row < inventory.size(); ++row)
		{
			inventory[row][activePackage]->displayItem(row);
			cout << inventory[row][activePackage];
		}
	}
}

// damage of the weapon currently used
int Player::GetDamage()
{
	return inventory[Row_WEAPON][activePackageIdx]->GetValue();
}

// armor points of the armor currently used
int Player::GetArmorPoints()
{
	return inventory[Row_ARMOR][activePackageIdx]->GetValue();
}

int Player::ReceiveDamage(int damage)
{
	health -= damage;
	if (health < 0)
		health = 0;

	return health;
}