//#include "GameStateManager.h"
//#include "GameObject.h"
//
///** The frame time for 60 FPS*/
//#define THRITY_FPS_FRAME_TIME 0.0666
//
//
//bool running = true;
//
//static int numGameObjects = 0;
//
//std::mutex mutex;
//std::condition_variable movementCV;
//
//Timeline realTime = Timeline(1);
//Timeline gameTime = Timeline(&realTime, 1);
//
//EventHandler* timeHandler;
//
//void movementThreadFunction(GameObject& player, double& deltaT, bool& hasFocus, double& currentTime) {
//    /** unique lock for CV, doesn't actually synchornize anything. */
//    bool paused = false;
//
//
//    while (running) {
//        EventManager* manager = manager->getInstance();
//        std::unique_lock<std::mutex> movementLock(mutex);
//
//        //std::cout << "deltaT passed in is " << deltaT << std::endl;
//
//        // if the user wants to move left
//        movementCV.wait(movementLock);
//        //std::cout << "Moved player, xVel: " << player.getXVelocity() << " deltaT: " << deltaT << std::endl;
//        //player.move(player.getXVelocity() * deltaT, 0);
//
//        // Even if the player did not end up moving, we will still tell the event manager the position
//        // of the player moved.
//
//        if (hasFocus) {
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
//                // raise an event that says to move the character
//                Event e(INPUT, player.guid, 0);
//                Variant v;
//                Variant d;
//
//                v.type = Variant::Type::DOUBLE;
//                v.doubleData = -200;
//
//                d.type = Variant::Type::DOUBLE;
//                d.doubleData = deltaT;
//
//                e.addArg(X_VEL, v);
//                e.addArg(DELTAT, d);
//                manager->raise(e);
//            }
//            // if the user wants to move right
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
//                Event e(INPUT, player.guid, 0);
//                Variant v;
//                Variant d;
//
//                v.type = Variant::Type::DOUBLE;
//                v.doubleData = 200;
//
//                d.type = Variant::Type::DOUBLE;
//                d.doubleData = deltaT;
//
//                e.addArg(X_VEL, v);
//                e.addArg(DELTAT, d);
//                manager->raise(e);
//            }
//
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
//                // shoot laser
//                Event e(SHOOT, player.guid, 0);
//                manager->raise(e);
//            }
//
//            /**
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !paused) {
//                // we want to pause the game.
//                // set client game state
//                paused = true;
//
//                Event e(PAUSED, -1, 0);
//
//                manager->raise(e);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::U) && paused) {
//                // we want to unpause the game.
//                paused = false;
//
//                Event e(UNPAUSED, -1, 0);
//
//                manager->raise(e);
//            }
//            */
//
//            /**
//            double currentTick = gameTime.getTick();
//            double desiredTick = gameTime.getTick();
//            bool changedTick = false;
//
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
//                //gameTime.changeTick((double)1);
//                desiredTick = 1.0;
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma)) {
//                //std::cout << "about to change speed of game" << std::endl;
//                //gameTime.changeTick((double)2);
//                desiredTick = 2.0;
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period)) {
//                //std::cout << "about to change speed of game" << std::endl;
//                //gameTime.changeTick((double).5);
//                desiredTick = 0.5;
//            }
//            if (currentTick != desiredTick) {
//                changedTick = true;
//            }
//
//            if (changedTick) {
//                if (manager->isReplaying) {
//                    Event e(CHANGED_TIMESCALE, -1, 0);
//
//                    Variant scale;
//                    scale.type = Variant::Type::DOUBLE;
//
//                    scale.doubleData = desiredTick;
//                    e.addArg(SCALE, scale);
//                    manager->raise(e);
//                }
//                else {
//                    Event e(CHANGED_TIMESCALE, -1, currentTime);
//
//                    Variant scale;
//                    scale.type = Variant::Type::DOUBLE;
//
//                    scale.doubleData = desiredTick;
//                    e.addArg(SCALE, scale);
//                    manager->raise(e);
//                }
//
//            }
//            */
//
//            /**
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && pressedTBeforeR) { // record a replay
//                recordingStarted = true;
//                pressedTBeforeR = false;
//
//                // a series of events will be raised later in the game loop
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !pressedTBeforeR) { // end recording, and begin playback
//                recordingEnded = true;
//                pressedTBeforeR = true;
//                oldPlayerPosition = player.getPosition();
//                postRecordingOffset = offset;
//            }
//
//            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && player.isAirborne() == false) {
//                //player.setYVelocity(6500);
//                // give the player a bit of headroom to perform a jump, smooth so that the object is
//                // not colliding in the next frame
//                //player.move(0, -4); // this is necessary to get the player to jump/get uncollided with something.
//                //player.setAirborne(true);
//
//                // raise an event instead
//                Event e(JUMPED, player.guid, currentTime);
//                manager->raise(e);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
//                // the player may teleport several times. also, their y velocity will decrease as long as they hold the button,
//                // which is ugly, but it allows me to avoid creating another event.
//                shouldTeleport = true;
//            }
//            */
//        }
//    }
//
//}
//
//int main() {
//    sf::Event event;
//    sf::RenderWindow window(sf::VideoMode(800, 900), "Hi Paola!");
//
//    double deltaT = 0;
//    double currentTime = 0;
//
//    bool hasFocus = true;
//
//    // init data structures to store game objects.
//    // also create game objects to init world.
//    GameStateManager* state = state->getInstance();
//    EventManager* manager = manager->getInstance();
//
//    // init event handler storage.
//    std::vector<EventHandler*> playerHandlers;
//
//    // create a player and init location.
//    GameObject* player = new GameObject(PLAYER);
//    player->setPosition(sf::Vector2f(40, 850));
//    player->guid = numGameObjects++;
//    manager->addToHandlers(player->guid, player->getHandler());
//    playerHandlers.push_back(player->getHandler());
//    state->addPlayer(player);
//    
//    manager->reg(DIED, playerHandlers);
//
//
//    // create enemies and init locations.
//    for (int i = 0; i < NUM_ROWS; i++) {
//        for (int j = 0; j < NUM_COLS; j++) {
//            GameObject* enemy = new GameObject(ENEMY);
//            enemy->setPosition(sf::Vector2f(50 * j + 30, i * 50 + 20)); // no rhyme or reason to these.
//            enemy->guid = numGameObjects++;
//            manager->addToHandlers(enemy->guid, enemy->getHandler());
//
//            state->addEnemy(enemy);
//        }
//    }
//
//
//    std::thread movementThread(movementThreadFunction, std::ref(*player), std::ref(deltaT), std::ref(hasFocus), std::ref(currentTime)); // begin movement thread.
//
//    while (window.isOpen()) {
//        // check to see if the window has focus.
//        hasFocus = window.hasFocus();
//        // clear the window with black color.
//        window.clear(sf::Color(0, 0, 0));
//
//        // get game time.
//        deltaT = gameTime.getTime() - currentTime;
//        if (deltaT > THRITY_FPS_FRAME_TIME || deltaT < 0) {
//            deltaT = 0;
//
//        }
//
//        // At this point, the threads have completed their calcuation and every event
//        // that happened in this game loop iteration has populated the manager's queue.
//        movementCV.notify_all();
//        currentTime = gameTime.getTime();
//
//        state->checkCollisions(currentTime);
//        state->doMovement(deltaT);
//        state->makeEnemiesShoot(currentTime);
//
//        // all events that are necessary have been added.
//        manager->handleEvents();
//
//        // all dead objects are deallocated.
//        state->cleanUpObjects();
//
//        while (window.pollEvent(event)) {
//            // "close requested" event: we close the window
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//        }
//    }
//    running = false;
//
//    movementCV.notify_all(); // let all the threads know to terminate
//
//    movementThread.join();
//}