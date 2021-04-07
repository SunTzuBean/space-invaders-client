#pragma once
#include <map>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "GameObject.h"


#define PLAYER_BULLETS_MAX 1
#define ENEMY_BULLETS_MAX 2
#define ENEMY_MAX 50
#define PLAYER_MAX 1
#define NUM_ROWS 5
#define NUM_COLS 10

#define ENEMY_LANE_IDX 0
#define ENEMY_RAND_IDX 1

#define PLAYER_BULLET_GUID -696969
#define ENEMY_BULLET_LANE_GUID -214215
#define ENEMY_BULLET_RAND_GUID -12512856

#define BULLET_VELOCITY 500

// part of the function A * B^numEnemies
#define A_POW 71.81273538
// part of the function A * B^numEnemies
#define B_POW 0.9747574665

#define A_ALT 838.574062
#define B_ALT 0.596250257


class GameStateManager {
private:
	/**The list that contains player bullets. */
	GameObject* playerBullets[PLAYER_BULLETS_MAX] = {};
	int playerBulletsSize;

	/**The list that contains enemy bullets. */
	GameObject* enemyBullets[ENEMY_BULLETS_MAX] = {};
	int enemyBulletsSize;

	/**The array that contains enemies. */
	GameObject* enemies[ENEMY_MAX] = {};
	int numEnemies;
	bool isMovingRight;

	/**The list that contains players */
	GameObject* players[PLAYER_MAX] = {};
	int numPlayers;

	static GameStateManager* instance;

	sf::RectangleShape leftBound;
	sf::RectangleShape rightBound;
	sf::RectangleShape upBound;
	sf::RectangleShape lowBound;

	int lives;
	

public:
	GameStateManager();
	static GameStateManager* getInstance();
	bool addPlayerBullet(GameObject* pb);
	void removePlayerBullet();

	void addEnemyBullet(GameObject* eb);
	bool addEnemyBullet(int index, GameObject* eb);
	void removeEnemyBullet(int index);

	void addEnemy(GameObject* e);
	void addEnemy(int index, GameObject* e);
	void removeEnemy(int index);

	void addPlayer(GameObject* p);
	void removePlayer();


	/**
	* Checks collisions between all objects that can collide.
	* Takes in current time to help organize and order events that are 
	* raised becasue of colliisions.
	* 
	* @param currentTime the current time for the game loop that this is called.
	*/
	void checkCollisions(double currentTime);

	/**
	* Does AI movement of enemies, and also moves bullets.
	* 
	* @param deltaT the deltaT between this frame and the last.
	*/
	void doMovement(double deltaT);
	
	/**
	* Makes an enemy shoot a bullet in the same lane as the player's current pos
	* as long as there's no bullet that's already been fired. Also chooses a
	* random time to shoot another bullet and a random enemy to fire it.
	* 
	* @param currentTime the current time of the game loop that this is called.
	*/
	void makeEnemiesShoot(double currentTime);

	/**
	* During runtime, some bullets are created that need to have their GUIDs
	* registered by the event manager.
	*/
	void registerPlayerBullets(int guid);

	void registerEnemyBullets(int guid);

	/**
	* After events have been handled, the game board needs to clean its references
	* to game objects. This deletes the game objects that are not alive. The 
	* game board is updated accordingly.
	*/
	void cleanUpObjects();

	/**
	* Checks win conditions. If the player wins!
	*/
	void checkWinConditions(double currentTime);

	/**
	* Once the dead objects have been cleared from the board, the remaining ones
	* can be drawn on the game window.
	* 
	* @param window a pointer to a render window.
	*/
	void drawGameBoard(sf::RenderWindow* window);

	/**
	* The client may want to know if the player can shoot so it can avoid creating
	* extra events that slow the runtime of the game down.
	*/
	bool canPlayerShoot();
	
	/**
	* Sets the player to visible so that the client can draw them.
	* A little less extensible, but there should only be one player anyway.
	*/
	void makePlayerVisible();
};