#include "Player.h"

Player::Player(int row, int col, int health) : location(row, col), health(health)
{
	// in the beginning , we want player to feel and be alive
	lifeStatus = Status_ALIVE;
}

Player::~Player()
{
	// remove all the items
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

// number of rows (should be 4 if player has anything, 0 if empty)
size_t Player::GetInventorySize()
{
	return inventory.size();
}

// set inventory from empty to have some size
void Player::ResizeInventory()
{
	inventory.resize(NUM_OF_ITEMS);
}

// take items from chest or from defeated enemy
ChestStatus Player::FillInventory(int damage, int armor, int health, int boost)
{
	// nothing to be taken (empty chest)
	if (damage == 0 and armor == 0 and health == 0 and boost == 0)
	{
		cout << endl << "The chest is empty!" << endl;
		return Chest_WAS_EMPTY;
	}
	else
	{
		// rows are enumerated for easier understanding
		// add items
		inventory[Row_WEAPON].push_back(new Weapon(damage));
		inventory[Row_ARMOR].push_back(new Armor(armor));
		inventory[Row_HEALTH_POTION].push_back(new HealthPotion(health));
		inventory[Row_MAGIC_MUSHROOM].push_back(new MagicMushroom(boost));
		return Chest_WAS_FULL;
	}
}

// number of packages available
size_t Player::GetInventoryColumnSize()
{
	if (GetInventorySize() > 0)
		return inventory[0].size();
	else
		return 0;
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
	// if empty inventory
	if (IsInventoryEmpty())
		cout << endl << "You have nothing!" << endl;
	else
	{
		// show each package of items available
		cout << endl << "-------Showing all inventory-------" << endl;
		for (size_t col = 0; col < inventory[0].size(); ++col)
		{
			cout << endl << "Package of items no. " << to_string(col + 1) << ":" << endl;
			for (size_t row = Row_WEAPON; row < inventory.size(); ++row)
			{
				inventory[row][col]->displayItem(row);
				// operator<< is overloaded for Item*
				cout << inventory[row][col];
			}
		}
	}
}

// equip player with package of choice
void Player::Equip()
{
	// if has nothing
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

		// decremented because of human indexing from 1
		activePackageIdx = packageNumber - 1;
	}
}

bool Player::IsInventoryEmpty()
{
	// if it doesn't have rows for items, it is empty
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
	// if valid activePackage
	if (activePackage < GetInventoryColumnSize())
	{
		ViewActiveInventory();
		// increase health
		health += inventory[Row_HEALTH_POTION][activePackage]->GetValue();
		if (health > 100)
		{
			// don't let player drink if there's no need
			cout << endl << "No need for health potion!" << endl;
			health -= inventory[Row_HEALTH_POTION][activePackage]->GetValue();
		}
		else
		{
			// no more health potion, set it to 0
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
	// if valid activePackage
	if (activePackage < GetInventoryColumnSize())
	{
		ViewActiveInventory();
		// increase weapon damage
		int newDamageValue = inventory[Row_WEAPON][activePackage]->GetValue() + inventory[Row_MAGIC_MUSHROOM][activePackage]->GetValue();
		inventory[Row_WEAPON][activePackage]->SetValue(newDamageValue);
		// no more magic mushroom
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
	// if valid activePackage
	if (activePackage < GetInventoryColumnSize())
	{
		cout << endl << "Package currently used:" << endl;
		// show all items in that package
		for (size_t row = Row_WEAPON; row < inventory.size(); ++row)
		{
			inventory[row][activePackage]->displayItem(row);
			cout << inventory[row][activePackage];
		}
	}
}

// damage of the weapon currently used
int Player::GetDamage(size_t packageIdx)
{
	// infinity serves as an indicator that no argument is passed
	if (packageIdx == INFINITY)
		packageIdx = activePackageIdx;

	return inventory[Row_WEAPON][packageIdx]->GetValue();
}

// armor points of the armor currently used
int Player::GetArmorPoints(size_t packageIdx)
{
	// infinity serves as an indicator that no argument is passed
	if (packageIdx == INFINITY)
		packageIdx = activePackageIdx;

	return inventory[Row_ARMOR][packageIdx]->GetValue();
}

// bonus health in the health potion
int Player::GetHealthPotion(size_t packageIdx)
{
	// infinity serves as an indicator that no argument is passed
	if (packageIdx == INFINITY)
		packageIdx = activePackageIdx;

	return inventory[Row_HEALTH_POTION][packageIdx]->GetValue();
}

// bonus damage in the magic mushroom
int Player::GetMagicMushroom(size_t packageIdx)
{
	// infinity serves as an indicator that no argument is passed
	if (packageIdx == INFINITY)
		packageIdx = activePackageIdx;

	return inventory[Row_MAGIC_MUSHROOM][packageIdx]->GetValue();
}

int Player::ReceiveDamage(int damage)
{
	// if player's armor points are greater than enemy's damage, don't increase health
	if ((health - damage) < health)
		health -= damage;
	// no need for health to be below zero
	if (health < 0)
		health = 0;

	return health;
}

int Player::GetHealth()
{
	return health;
}