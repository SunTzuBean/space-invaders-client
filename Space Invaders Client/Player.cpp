#include <SFML/Graphics.hpp>
#include "Player.h"
#include <mutex>


Player::Player() {
	setPointCount(7);
	setPoint(0, sf::Vector2f(0, 0));
	setPoint(1, sf::Vector2f(15, 0));
	setPoint(2, sf::Vector2f(30, -15));
	setPoint(3, sf::Vector2f(45, 0));
	setPoint(4, sf::Vector2f(60, 0));
	setPoint(5, sf::Vector2f(60, 15));
	setPoint(6, sf::Vector2f(0, 15));
	setFillColor(sf::Color(57, 199, 20));
	guid = 0;
	xVelocity = 0;
	invincible = false;
	visible = true;
}



void Player::setXVelocity(double xVelocity) {
	std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	this->xVelocity = xVelocity;
}

double Player::getXVelocity() {
	std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	return this->xVelocity;
}

bool Player::isInvincible()
{
	return invincible;
}

void Player::setInvincible(bool status)
{
	invincible = status;
}

bool Player::isVisible() {
	return visible;
}

void Player::setVisible(bool status) {
	visible = status;
}