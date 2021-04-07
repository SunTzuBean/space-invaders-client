#include "GameObject.h"

GameObject::GameObject() {
	this->bullet = NULL;
	this->enemy = NULL;
	this->player = NULL;
	this->handler = NULL;
}

GameObject::GameObject(std::string type) {
	if (type == ENEMY_BULLET) {
		this->bullet = new EnemyBullet(sf::Vector2f(10, 30));
		this->enemy = NULL;
		this->player = NULL;
		this->handler = new BulletHandler(this);
	}
	else if (type == PLAYER_BULLET) {
		this->bullet = new PlayerBullet(sf::Vector2f(10, 30));
		this->enemy = NULL;
		this->player = NULL;
		this->handler = new BulletHandler(this);
	}
	else if (type == ENEMY) {
		this->bullet = NULL;
		this->enemy = new Enemy(sf::Vector2f(ENEMY_WIDTH, ENEMY_HEIGHT));
		this->player = NULL;
		this->handler = new EnemyHandler(this);
	}
	else if (type == PLAYER) {
		this->bullet = NULL;
		this->enemy = NULL;
		this->player = new Player();
		this->handler = new PlayerHandler(this);
	}
	else {
		// We should NEVER EVER get here, if we got here we initialized an object incorrectly.
		this->bullet = NULL;
		this->enemy = NULL;
		this->player = NULL;
		this->handler = NULL;
	}
	this->type = type;
	isAlive = true;
	guid = 0;

	
}

GameObject::~GameObject() {
	if (type == ENEMY_BULLET || type == PLAYER_BULLET) {
		delete bullet;
	}
	else if (type == ENEMY) {
		delete enemy;
	}
	else if (type == PLAYER) {
		delete player;
	}
	delete handler;
}

GameObject::GameObject(const GameObject& g2) {
	this->type = g2.type;
	this->bullet = g2.bullet;
	this->enemy = g2.enemy;
	this->player = g2.player;
	this->handler = g2.handler;
	this->isAlive = g2.isAlive;
	this->guid = g2.guid;
}

GameObject& GameObject::operator=(const GameObject& g2)
{
	this->type = g2.type;
	this->bullet = g2.bullet;
	this->enemy = g2.enemy;
	this->player = g2.player;
	this->handler = g2.handler;
	this->isAlive = g2.isAlive;
	this->guid = g2.guid;
	return *this;
}

void GameObject::setGUID(int guid) {
	this->guid = guid;
}

int GameObject::getGUID() {
	return this->guid;
}

std::string GameObject::getType() {
	return this->type;
}

EventHandler* GameObject::getHandler() {
	return handler;
}

void GameObject::draw(sf::RenderWindow* window) {
	if (type == PLAYER_BULLET || type == ENEMY_BULLET) {
		window->draw(*bullet);
	}
	else if (type == ENEMY) {
		window->draw(*enemy);
	}
	else if (type == PLAYER) {
		window->draw(*player);
	}
}

void GameObject::move(float x, float y) {
	if (type == PLAYER_BULLET || type == ENEMY_BULLET) {
		bullet->move(x, y);
	}
	else if (type == ENEMY) {
		enemy->move(x, y);
	}
	else if (type == PLAYER) {
		player->move(x, y);
	}
}

bool GameObject::collidesWith(GameObject other) {
	std::string oType = other.getType();
	if (type == PLAYER_BULLET) {
		if (oType == PLAYER_BULLET) {
			return this->bullet->getGlobalBounds().intersects(other.getBullet()->getGlobalBounds());
		}
		else if (oType == ENEMY) {
			return this->bullet->getGlobalBounds().intersects(other.getEnemy()->getGlobalBounds());
		}
		else if (oType == PLAYER) {
			return this->bullet->getGlobalBounds().intersects(other.getPlayer()->getGlobalBounds());
		}
	}
	else if (type == ENEMY) {
		if (oType == PLAYER_BULLET) {
			return this->enemy->getGlobalBounds().intersects(other.getBullet()->getGlobalBounds());
		} // remember, enemies can't intersect each other.
		else if (oType == PLAYER) {
			return this->enemy->getGlobalBounds().intersects(other.getPlayer()->getGlobalBounds());
		}
	}
	else if (type == PLAYER) {
		if (oType == PLAYER_BULLET) {
			return this->player->getGlobalBounds().intersects(other.getBullet()->getGlobalBounds());
		} // remember, no more than 1 player so no intersection between players.
		else if (oType == ENEMY) {
			return this->player->getGlobalBounds().intersects(other.getEnemy()->getGlobalBounds());
		}
	}
	return false; // You should not EVER get here.
}

void GameObject::setAlive(bool isAlive) {
	this->isAlive = isAlive;
}

void GameObject::kill() {
	this->isAlive = false;
}

bool GameObject::cleanUp() {
	if (!isAlive) { // if it's dead,
		if (type == PLAYER_BULLET || type == ENEMY_BULLET) { // clean the memory allocated for the object.
			delete this->bullet; 
		}
		else if (type == ENEMY) {
			delete this->enemy;
		}
		else if (type == PLAYER) {
			delete this->player;
		}
		delete this->handler; // free the handler for the object.
		return true;
	}
	return false;
	// nothing happens if it's called and it's still alive.
	// I assume you would call delete from outside to free this object, but then I'm worried you can't
	// remove it from a vector?
}

void GameObject::setPosition(sf::Vector2f pos) {
	if (type == PLAYER_BULLET || type == ENEMY_BULLET) {
		bullet->setPosition(pos);
	}
	else if (type == ENEMY) {
		enemy->setPosition(pos);
	}
	else if (type == PLAYER) {
		player->setPosition(pos);
	}
}

void GameObject::setTexture(std::string texture) {
	sf::Texture t;
	if (!t.loadFromFile(texture)) {}
	t.setRepeated(true);
	if (type == PLAYER_BULLET || type == ENEMY_BULLET) {
		bullet->setTexture(&t);
	}
	else if (type == ENEMY) {
		enemy->setTexture(&t);
	}
	else if (type == PLAYER) {
		player->setTexture(&t);
	}
}

Bullet* GameObject::getBullet() {
	return this->bullet;
}

Enemy* GameObject::getEnemy() {
	return this->enemy;
}

Player* GameObject::getPlayer() {
	return this->player;
}