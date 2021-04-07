#include "Enemy.h"

Enemy::Enemy(sf::Vector2f dimensions) : sf::RectangleShape(dimensions) {
	setFillColor(sf::Color::White); // they're always one color!
	if (t.loadFromFile("alien.png")) {}
	t.setSmooth(true);
	setTexture(&t);
}