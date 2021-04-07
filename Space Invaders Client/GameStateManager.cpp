#include "GameStateManager.h"

GameStateManager* GameStateManager::instance;

GameStateManager::GameStateManager() {
	playerBulletsSize = 0;
	enemyBulletsSize = 0;
	numEnemies = 0;
	numPlayers = 0;
	isMovingRight = true;
	leftBound = sf::RectangleShape(sf::Vector2f(1, 900));
	leftBound.setPosition(sf::Vector2f(10, 0));

	rightBound = sf::RectangleShape(sf::Vector2f(1, 900));
	rightBound.setPosition(sf::Vector2f(790, 0));

	upBound = sf::RectangleShape(sf::Vector2f(800, 1));
	upBound.setPosition(sf::Vector2f(0, 0));
	
	lowBound = sf::RectangleShape(sf::Vector2f(800, 1));
	lowBound.setPosition(sf::Vector2f(0, 900));

}

GameStateManager* GameStateManager::getInstance() {
	if (!instance) {
		instance = new GameStateManager();
	}
	return instance;
}

bool GameStateManager::addPlayerBullet(GameObject* pb) {
	if (playerBullets[0] == NULL) {
		playerBullets[0] = pb;
		return true;
	}
	return false;
}

void GameStateManager::removePlayerBullet() {
	if (playerBulletsSize == 1) {
		playerBullets[0]->kill();
		playerBulletsSize--;
	}
}

void GameStateManager::addEnemyBullet(GameObject* eb) {
	if (enemyBulletsSize < ENEMY_BULLETS_MAX) {
		enemyBullets[enemyBulletsSize] = eb;
		enemyBulletsSize++;
	}
}

bool GameStateManager::addEnemyBullet(int index, GameObject* eb) {
	if (index < ENEMY_BULLETS_MAX) {
		if (enemyBullets[index] == NULL) {
			enemyBullets[index] = eb;
			enemyBulletsSize++;
			return true;
		} // if the space is occupied already, don't do anything
	}
	return false;
}

void GameStateManager::removeEnemyBullet(int index) {
	if (index < ENEMY_BULLETS_MAX) {
		delete enemyBullets[index];
		enemyBullets[index] = NULL;
		enemyBulletsSize--;
	}
}

void GameStateManager::addEnemy(GameObject* e) {
	if (numEnemies < ENEMY_MAX) {
		enemies[numEnemies] = e;
		numEnemies++;
	}
}

void GameStateManager::addEnemy(int index, GameObject* e) {
	if (index < ENEMY_MAX) {
		enemies[index] = e;
		numEnemies++;
	}
}

void GameStateManager::removeEnemy(int index) {
	if (index < ENEMY_MAX) {
		delete enemies[index];
		enemies[index] = NULL;
		numEnemies--;
	}
}

void GameStateManager::addPlayer(GameObject* p) {
	if (numPlayers < PLAYER_MAX) {
		players[0] = p;
		numPlayers++;
	}
}

void GameStateManager::removePlayer() {
	if (numPlayers == PLAYER_MAX) {
		delete players[0];
		players[0] = NULL;
		numPlayers--;
	}
}

void GameStateManager::checkCollisions(double currentTime) {
	// this raises collision events as they occur.
	for (int i = 0; i < PLAYER_BULLETS_MAX; i++) {
		// comapre player bullets against enemies
		for (int j = 0; j < ENEMY_MAX; j++) {
			if (enemies[j] != NULL && playerBullets[i] != NULL) { // meaning there is an enemy there,
				if (enemies[j]->getEnemy()->getGlobalBounds().intersects(playerBullets[i]->getBullet()->getGlobalBounds())) {
					// raise a collision event.
					EventManager* manager = manager->getInstance();
					Event e(COLLISION_ENEMY, playerBullets[i]->getGUID(), currentTime); // handled by bullet first.
					Variant g;
					g.type = Variant::Type::INTEGER;
					g.intData = enemies[j]->getGUID();

					e.addArg(INTENDED_GUID, g); // send the target that it hit so it knows to die.
					manager->raise(e);
				}
			}
		}
		if (playerBullets[i] != NULL) {
			if (playerBullets[i]->getBullet()->getGlobalBounds().intersects(upBound.getGlobalBounds())) { // if the player bullet is touching the upper bound
				EventManager* manager = manager->getInstance();
				Event e(COLLISION_BOUND, playerBullets[i]->getGUID(), currentTime);

				manager->raise(e);
			}
		}
		
	}

	for (int i = 0; i < ENEMY_BULLETS_MAX; i++) {
		// compare enemy bullets against players.
		for (int j = 0; j < PLAYER_MAX; j++) {
			if (players[j] != NULL && enemyBullets[i] != NULL) { // meaning there is a player there,
				if (players[j]->getPlayer()->getGlobalBounds().intersects(enemyBullets[i]->getBullet()->getGlobalBounds())) {
					// raise a collision event.
					EventManager* manager = manager->getInstance();
					Event e(COLLISION_PLAYER, enemyBullets[i]->getGUID(), currentTime); // handled by bullet first.

					manager->raise(e);
				}
			}
		}
		if (enemyBullets[i] != NULL) {
			if (enemyBullets[i]->getBullet()->getGlobalBounds().intersects(lowBound.getGlobalBounds())) { // if the player bullet is touching the lower bound
				EventManager* manager = manager->getInstance();
				Event e(COLLISION_BOUND, enemyBullets[i]->getGUID(), currentTime);

				manager->raise(e);
			}
		}
	}

	for (int i = 0; i < PLAYER_MAX; i++) {
		// compare players against enemies.
		for (int j = 0; j < ENEMY_MAX; j++) {
			if (enemies[j] != NULL && players[i] != NULL) {
				if (enemies[j]->getEnemy()->getGlobalBounds().intersects(players[i]->getPlayer()->getGlobalBounds())) {
					// raise a collision event.
					EventManager* manager = manager->getInstance();
					Event e(COLLISION_PLAYER, enemies[j]->getGUID(), currentTime);
					
					manager->raise(e);
				}
			}
		}
	}
}

void GameStateManager::doMovement(double deltaT) {
	// player movement is handled by main, where input is read.
	// The widest row dictates the width of the alien patrol. They remain in the same
	// relative ordering throughout the level.
	if (isMovingRight) {
		// we need to find the largest row
		int largestRow = -1;
		int widestIndex = -1;
		for (int i = 0; i < NUM_ROWS; i++) {
			for (int j = 0; j < NUM_COLS; j++) {
				if (enemies[(i * NUM_COLS) + j] != NULL && j >= widestIndex) { // if the spot we're looking at has an enemy
					largestRow = i;
					widestIndex = j;
				}
			}
		}
		if (largestRow != -1 && widestIndex != -1) {
			// now we have the farthest right index at enemies[(largestRow * NUM_COLS) + widestIndex].
			// if that one is not touching the rightmost boundary, then move as normal.
			// raise an event for every active enemy.
			if (enemies[(largestRow * NUM_COLS) + widestIndex]->getEnemy()->getGlobalBounds().intersects(rightBound.getGlobalBounds())) {
				// the enemy is touching the right most bound, time to shift directions.
				isMovingRight = false;
				for (int i = 0; i < ENEMY_MAX; i++) {
					if (enemies[i] != NULL) {
						// move each enemy down by raising an event
						EventManager* manager = manager->getInstance();
						Event e(MOVE_DOWN, enemies[i]->getGUID(), 0);

						manager->raise(e);
					}
				}
			}
			else {
				EventManager* manager = manager->getInstance();
				// it's not intersecting, give each active enemy an amount to move by.
				for (int i = 0; i < ENEMY_MAX; i++) {
					if (enemies[i] != NULL) {
						// raise an event to move the enemy.

						Event e(MOVE_LAT, enemies[i]->getGUID(), 0);

						Variant v;
						v.type = Variant::Type::DOUBLE;
						double velocity;
						if (numEnemies <= 5) {
							velocity = A_ALT * std::pow(B_ALT, numEnemies);
						}
						else {
							velocity = A_POW * std::pow(B_POW, numEnemies);
						}
						v.doubleData = deltaT * velocity;
						e.addArg(X_DIST, v);
						manager->raise(e);
					}
				}
			}
		}
	}
	else {
		// we need to find the largest row to the left.
		int largestRow = NUM_ROWS;
		int widestIndex = NUM_COLS;
		for (int i = NUM_ROWS - 1; i >= 0; i--) {
			for (int j = NUM_COLS - 1; j >= 0; j--) {
				if (enemies[(i * NUM_COLS) + j] != NULL && j < widestIndex) { // if the spot we're looking at has an enemy and j is less than found widest index
					largestRow = i;
					widestIndex = j;
				}
			}
		}
		if (largestRow != NUM_ROWS && widestIndex != NUM_COLS) {
			// now we have the farthest left index at enemies[(largestRow * NUM_COLS) + widestIndex].
			// if that one is not touching the leftmost boundary, then move as normal.
			// raise an event for every active enemy.
			if (enemies[(largestRow * NUM_COLS) + widestIndex]->getEnemy()->getGlobalBounds().intersects(leftBound.getGlobalBounds())) {
				isMovingRight = true;
				for (int i = 0; i < ENEMY_MAX; i++) {
					if (enemies[i] != NULL) {
						// move each enemy down by raising an event
						EventManager* manager = manager->getInstance();
						Event e(MOVE_DOWN, enemies[i]->getGUID(), 0);

						manager->raise(e);
					}
				}
			}
			else {
				// it's not intersecting or colliding, raise a movement event so that they move left.
				for (int i = 0; i < ENEMY_MAX; i++) {
					if (enemies[i] != NULL) {
						// raise an event to move the enemy.
						EventManager* manager = manager->getInstance();
						Event e(MOVE_LAT, enemies[i]->getGUID(), 0);

						Variant v;
						v.type = Variant::Type::DOUBLE;
						double velocity;
						if (numEnemies <= 5) {
							velocity = -A_ALT * std::pow(B_ALT, numEnemies);
						}
						else {
							velocity = -A_POW * std::pow(B_POW, numEnemies);
						}
						v.doubleData = deltaT * velocity;
						e.addArg(X_DIST, v);
						manager->raise(e);
					}
				}
			}
		}
		
	}
	// now we need to move all of the bullets that are active.
	for (int i = 0; i < PLAYER_BULLETS_MAX; i++) {
		if (playerBullets[i] != NULL) {
			EventManager* manager = manager->getInstance();
			Event e(MOVE, playerBullets[i]->getGUID(), 0);
			Variant v;
			Variant d;

			v.type = Variant::Type::DOUBLE;
			v.doubleData = -BULLET_VELOCITY;

			d.type = Variant::Type::DOUBLE;
			d.doubleData = deltaT;

			e.addArg(Y_VEL, v);
			e.addArg(DELTAT, d);
			manager->raise(e);
		}
	}

	for (int i = 0; i < ENEMY_BULLETS_MAX; i++) {
		if (enemyBullets[i] != NULL) {
			EventManager* manager = manager->getInstance();
			Event e(MOVE, enemyBullets[i]->getGUID(), 0);
			Variant v;
			Variant d;

			v.type = Variant::Type::DOUBLE;
			v.doubleData = BULLET_VELOCITY;

			d.type = Variant::Type::DOUBLE;
			d.doubleData = deltaT;

			e.addArg(Y_VEL, v);
			e.addArg(DELTAT, d);
			manager->raise(e);
		}
	}

}

void GameStateManager::makeEnemiesShoot(double currentTime) {
	if (enemyBullets[ENEMY_LANE_IDX] == NULL) { // if the enemy can shoot in a lane because there isn't a bullet
		// we have to find a lane with an x coordinate similar to the player
		int closestLane = 0;
		float leastDistance = 900000; // arbitrary value because the distance shouldn't be greater than this any time.
		float xCoordOfPlayer = players[0]->getPlayer()->getPosition().x;
		for (int i = 0; i < ENEMY_MAX; i++) {
			if (enemies[i] != NULL) {
				// take note of its x coordinate
				float xCoordOfEnemy = enemies[i]->getEnemy()->getPosition().x;
				// subtract them
				float diff = xCoordOfPlayer - xCoordOfEnemy;
				diff = diff * diff; // square the difference so that it's always positive
				if (diff < leastDistance) {
					leastDistance = diff; // set least distance
					closestLane = i % NUM_COLS;
				}
			}
		}
		if (leastDistance == 900000) {
			// that means we found nothing to do here.
			return; // no enemies are alive anyway.
		}
		
		// Now that we have the column of the enemy that's closest to the player, we have to
		// get the lowest enemy.
		// To increase average runtime, we can start looking from the bottom.
		int lowestEnemyIndex = 0;
		for (int i = 40 + closestLane; i >= 0; i -= NUM_COLS) { // starts on last row.
			// this should look at where the lowest enemy should be, then go to the next lowest enemy
			// if we couldn't find it this time.
			if (enemies[i] != NULL) {
				lowestEnemyIndex = i;
				break; // we're done, because we found an enemy
			}
		}
		
		// now we have the enemy that can shoot, so raise an event to the manager that tells this enemy to shoot.
		EventManager* manager = manager->getInstance();
		Event e(SHOOT_LANE, enemies[lowestEnemyIndex]->getGUID(), currentTime);

		// requires no arguments.
		manager->raise(e);

	}
	// now we should look to see if we can fire a random bullet. 
	if (enemyBullets[ENEMY_RAND_IDX] == NULL) {
		// we should calculate a random number.
		// there is a 5% chance an enemy will fire a random bullet.
		srand(time(NULL));

		int chance = rand() % 10;
		if (chance < 3) { // should be 30% probability per frame a bullet is shot
			// now choose a random available column to shoot.
			bool foundValidCol = false;
			int validIndex = 0;
			while (!foundValidCol) {
				int col = rand() % 10;
				for (int i = 40 + col; i >= 0; i -= 10) { // start from the bottom
					if (enemies[i] != NULL) {
						foundValidCol = true;
						validIndex = i;
						break;
					}
				}
			}

			// now we should've found a valid index to have the enemy shoot.
			// raise an event now.
			EventManager* manager = manager->getInstance();
			Event e(SHOOT_RANDOM, enemies[validIndex]->getGUID(), currentTime);

			// requires no arguments.
			manager->raise(e);
		}
	}
}

void GameStateManager::registerPlayerBullets(int guid) {
	// go through all bullets and if they exist make sure they're set up right.
	for (int i = 0; i < PLAYER_BULLETS_MAX; i++) {
		if (playerBullets[i] != NULL) {
			if (playerBullets[i]->getGUID() == 0) { // if the default GUID was used, we must change it
				playerBullets[i]->setGUID(guid);
				EventManager* manager = manager->getInstance();
				
				manager->addToHandlers(playerBullets[i]->getGUID(), playerBullets[i]->getHandler());
			}
		}
	}
}
void GameStateManager::registerEnemyBullets(int guid) {
	for (int i = 0; i < ENEMY_BULLETS_MAX; i++) {
		if (enemyBullets[i] != NULL) {
			if (enemyBullets[i]->getGUID() == 0) {
				enemyBullets[i]->setGUID(guid);
				EventManager* manager = manager->getInstance();
				
				manager->addToHandlers(enemyBullets[i]->getGUID(), enemyBullets[i]->getHandler());
			}
		}
	}
}

void GameStateManager::cleanUpObjects() {
	// go through all game objects and if they're dead make the go away
	for (int i = 0; i < PLAYER_BULLETS_MAX; i++) {
		if (playerBullets[i] != NULL) {
			if (playerBullets[i]->cleanUp()) { // if it cleaned up
				playerBullets[i] = NULL;
				playerBulletsSize--;
			}
		}
	}

	for (int i = 0; i < ENEMY_BULLETS_MAX; i++) {
		if (enemyBullets[i] != NULL) {
			if (enemyBullets[i]->cleanUp()) {
				enemyBullets[i] = NULL;
				enemyBulletsSize--;
			}
		}
	}

	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemies[i] != NULL) {
			if (enemies[i]->cleanUp()) {
				enemies[i] = NULL;
				numEnemies--;
			}
		}
	}

	for (int i = 0; i < PLAYER_MAX; i++) {
		if (players[i] != NULL) {
			if (players[i]->cleanUp()) {
				players[i] = NULL;
				numPlayers--;
			}
		}
	}
}

void GameStateManager::drawGameBoard(sf::RenderWindow* window) {
	// go through all game objects and if they exist draw them
	for (int i = 0; i < PLAYER_BULLETS_MAX; i++) {
		if (playerBullets[i] != NULL) {
			playerBullets[i]->draw(window);
		}
	}

	for (int i = 0; i < ENEMY_BULLETS_MAX; i++) {
		if (enemyBullets[i] != NULL) {
			enemyBullets[i]->draw(window);
		}
	}

	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemies[i] != NULL) {
			enemies[i]->draw(window);
		}
	}

	for (int i = 0; i < PLAYER_MAX; i++) {
		if (players[i] != NULL) {
			if (players[i]->getPlayer()->isVisible()) {
				players[i]->draw(window);
			}
			
		}
	}
}

bool GameStateManager::canPlayerShoot() {
	return playerBullets[0] == NULL;
}

void GameStateManager::makePlayerVisible() {
	if (players[0] != NULL) {
		players[0]->getPlayer()->setVisible(true);
	}
}

void GameStateManager::checkWinConditions(double currentTime) {
	if (numEnemies == 0) {
		EventManager* manager = manager->getInstance();
		
		Event e(WON, -1, currentTime);

		manager->raise(e);
	}
}
