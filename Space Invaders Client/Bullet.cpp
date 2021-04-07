#include "Bullet.h"
#include <iostream>

Bullet::Bullet(sf::Vector2f dimensions) : sf::RectangleShape(dimensions) {}

PlayerBullet::PlayerBullet(sf::Vector2f dimensions) : Bullet(dimensions) {
	// player bullets move up the screen. Their velocity is negative, since moving up means subtracting velocity.
	if (t.loadFromFile("bullet.png")) {}
	t.setSmooth(true);
	setTexture(&t);
}


EnemyBullet::EnemyBullet(sf::Vector2f dimensions) : Bullet(dimensions) {
	if (t.loadFromFile("bullet.png")) {}
	t.setSmooth(true);
	setTexture(&t);
}
