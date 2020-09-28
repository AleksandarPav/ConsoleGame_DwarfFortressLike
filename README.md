# ConsoleGame_DwarfFortressLike

Unpack jsoncpp.zip to the same level where solution folder and ReadMe are.
Jsoncpp library built with CMake 3.18.2, source code cloned from https://github.com/open-source-parsers/jsoncpp.
Run program from main.cpp.
All controls shown when the game starts.
Weapon, Armor, Health potion and Magic mushroom (boosts weapon damage) are all types of class Item.
Player's inventory is stored as a 4x(num_of_packages_collected) matrix.
Each row in that matrix corresponds to one type of item.
Enemy is not capable of inventory and can only have one weapon and one armor.
Suggestion: Load cfg3.json (input 3 when prompted), because that map is the only one that looks nice. Also, it is of size 50x50
so it can whole be seen (when V is pressed).
Suggestion 2: Put command window in Fullscreen mode so that the whole map can be seen.
Suggestion 3: Press 'v' after every move so you can see the current look of the map and know where you are.

Walkthrough:
When the game starts, welcome massage is printed and chosen configuration is loaded.
When player stands next to a chest, he can, but doesn't have to, loot it by the press of SPACE button.
If the chest wasn't empty, content will be put into player's inventory and chest will be emptied.
After the chest is looted, it's the enemy's turn.
All the enemies move some random number of times, but never to already taken areas.
When player finds himself next to the enemy, combat is automatically triggered.
When combat starts, player is asked to choose set of items in his inventory to fight.
If the player doesn't have any items, dies automatically.
Enemy is first to hit and the battle lasts until someone dies.
If the player dies, the game is over and user gets to choose to play another game.
If the enemy dies, it's player's turn again.
All of the enem's possessions transfer to the player's inventory and he's free to move to loot chests or go to the enemies.
If all enemies die, the game is over and user gets to choose to play another game.