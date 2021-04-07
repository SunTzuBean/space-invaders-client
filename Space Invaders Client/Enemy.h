#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include <ostream>

#define ENEMY_COLOR 215, 242, 249

class Enemy : public sf::RectangleShape {
	sf::Texture t;
public:
	Enemy(sf::Vector2f dimensions);
};