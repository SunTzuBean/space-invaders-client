#pragma once
#include <SFML/Graphics.hpp>
#include <mutex>
#include "Player.h"
#include "EventHandler.h"
#include "Enemy.h"
#include "Bullet.h"

#define ENEMY_BULLET "enemyBullet"
#define PLAYER_BULLET "playerBullet"
#define ENEMY "enemy"
#define PLAYER "player"

#define ENEMY_WIDTH 40
#define ENEMY_HEIGHT 30
class EventHandler;
class Player;

/**
* Game objects can be enemies, players, or bullets.
*/
class GameObject {
protected:
	std::string type;
	Bullet* bullet;
	Enemy* enemy;
	Player* player;
	
	/** The pointer to the object that handles events for this game object*/
	EventHandler* handler;
	
	/** This is so garbage collection can be spoofed: allows for iteration over each game
		object and will delete it if it's dead, and it may cause GUIDS to change. */
	bool isAlive;
	
public:
	int guid;

	GameObject();

	/**
	* Constructs a game object bullet.
	*/
	GameObject(std::string type);

	/**
	* Destroys a game object, whose contents are allocated on the heap.
	*/
	~GameObject();

	GameObject(const GameObject& g2);

	GameObject& operator=(const GameObject& other);
	
	void setGUID(int guid);
	int getGUID();

	std::string getType();

	/**
	* Gets this GameObject's event handler.
	* @return a pointer to the game object's event handler.
	*/
	EventHandler* getHandler();

	void draw(sf::RenderWindow* window);

	void move(float x, float y);

	/**
	* Only bullets can intersect enemies, bullets can intersect bullets,
	* and only enemies can intersect
	* players. Enemies should never intersect other enemies, players should
	* never intersect other players.
	*/
	bool collidesWith(GameObject other);

	void setAlive(bool isAlive);

	void kill();

	bool cleanUp();

	void setPosition(sf::Vector2f);

	void setTexture(std::string texture);

	Bullet* getBullet();

	Enemy* getEnemy();

	Player* getPlayer();

};