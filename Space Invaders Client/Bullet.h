#pragma once
#include <SFML/Graphics.hpp>

class Bullet : public sf::RectangleShape {
public:
	Bullet(sf::Vector2f dimensions);
	/**
	* Bullets are fired from the player and enemy, but there is an important
	* distinction between them. They will both have velocities in opposite
	* directions, but the program will need their velocities from time to time.
	* Doing this will allow bullets to be stored in a single container, but their
	* distinction will remain.
	*/
};

class PlayerBullet : public Bullet {
private:
	sf::Texture t;
public:
	PlayerBullet(sf::Vector2f dimensions);

};

class EnemyBullet : public Bullet {
private:
	sf::Texture t;
public:
	EnemyBullet(sf::Vector2f dimensions);

};