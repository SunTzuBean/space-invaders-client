#pragma once
#include <SFML/Graphics.hpp>
#include <mutex>

#include "Bullet.h"
class EventHandler;


class Player : public sf::ConvexShape {
private:
	double xVelocity;
	bool invincible;
	bool visible;
public:
	int guid;
	/**
	*Constructs a Player object, inherited from ConvexShape.
	*/
	Player();

	/**
	* Sets the x velocity for this player.
	* @param xVelocity the x velocity to give this player.
	*/
	void setXVelocity(double xVelocity);

	/**
	* Gets the x velocity for this player.
	* @return the x velocity of this player.
	*/
	double getXVelocity();

	bool isInvincible();

	void setInvincible(bool yes);

	bool isVisible();

	void setVisible(bool yes);
};