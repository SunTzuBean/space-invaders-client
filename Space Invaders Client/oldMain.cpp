//#include <SFML/Graphics.hpp>
//#include "StaticPlatform.h"
//#include "MovingPlatform.h"
//#include "Player.h"
//#include "Timeline.h"
//#include "GameObject.h"
//#include "EventManager.h"
//#include "ScriptManager.h"
//#include "dukglue/dukglue.h"
//class Platform;
//class Spawn;
//class Boundary;
//
//#include <thread>
//#include <mutex> 
//#include <iostream>
//#include <cmath>
//#include <vector>
//#include <zmq.hpp>
//#include <json.hpp>
//
///** offset for moving platforms */
//#define MAX_OFFSET 100
//
///** how much to reduce the jump velocity and find the next position. */
//#define JUMP_VELOCITY_FACTOR -8800
//
///** How long the platform will move in one direction in seconds */
//#define PLATFORM_HALF_PERIOD 2
//
///** How long we will protect the game from subsequent player presses to spam P to pause. */
//#define PAUSE_SPAM_PERIOD 0.5
//
///** The frame time for 60 FPS*/
//#define THRITY_FPS_FRAME_TIME 0.0666
//
///** The address for the pub-sub sockets. */
//#define PUB_SUB_ADDR "tcp://localhost:5551"
//
///** The address for the req-rep sockets. */
//#define REQ_REP_ADDR "tcp://localhost:5555"
//
//#define PLAYER_RADIUS 25.f
//
//#define DISCONNECT_STRING "0 0 0 0 0 0 0 0 0 0 0\n"
//
///** movement condition variable */
//std::condition_variable movementCV;
//std::condition_variable platformCV;
//std::condition_variable gravityCV;
//std::condition_variable mainLoopCV;
//std::mutex mutex;
//
//Timeline realTime = Timeline(1);
//Timeline gameTime = Timeline(&realTime, 1);
//
//bool running = true; // allows communication to threads.
//
///** Tells whether or not recording has begun. */
//bool recordingStarted = false;
//
///** Tells whether or not recording has ended. */
//bool recordingEnded = false;
//
///** Tellse whether or not the game is currently playing a replay.*/
//bool isPlayingReplay = false;
//
//bool pressedTBeforeR = true;
//
//sf::Vector2f oldPlayerPosition;
//
//// get an instance of the event manager.
//EventManager* manager = manager->getInstance();
//
//EventHandler* connectHandler;
//
//EventHandler* timeHandler;
//
//EventHandler* scriptHandler;
//
//static int totalGameObjects = 0;
//
//double currentTime = 0;
//
//int postRecordingOffset = 0;
//
//bool shouldTeleport = false;
//
//using json = nlohmann::json;
//
//void movementThreadFunction(Player& player, double& deltaT, bool& hasFocus, int& offset) {
//    /** unique lock for CV, doesn't actually synchornize anything. */
//    bool paused = false;
//
//
//    while (running) {
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
//        double totalXVelocity = 0;
//
//        if (hasFocus) {
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
//                // left key is pressed: move our character
//                totalXVelocity -= 200;
//            }
//            // if the user wants to move right
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
//                totalXVelocity += 200;
//            }
//
//            totalXVelocity = fmin(200, fmax(-200, totalXVelocity));
//
//
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
//
//
//
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
//
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
//        }
//
//
//        if (totalXVelocity != 0) {
//            player.setXVelocity(totalXVelocity);
//            //std::cout << "x velocity is " << totalXVelocity << std::endl;
//            //std::cout << "deltaT is " << deltaT << std::endl;
//
//            Event e(LATERAL_INPUT, player.guid, currentTime);
//
//            Variant x;
//
//            x.type = Variant::Type::FLOAT;
//            x.floatData = player.getXVelocity() * deltaT;
//
//            e.addArg(X_DIST, x);
//
//            manager->raise(e);
//        }
//
//        gravityCV.notify_one();
//        //std::cout << "player mvmnt thread finished, notified grav thread" << std::endl;
//        movementLock.unlock();
//    }
//
//}
//
//void collisionAndGravityThreadFunction(Player& player, std::vector<Platform*>& platformVector, std::vector<Spawn*>& spawnVector,
//    std::vector<Boundary*>& viewBoundsVector, std::vector<Boundary*>& killBoundsVector, double& deltaT, double& serverDT, int& offset) {
//
//    bool touchingBoundary = false;
//    bool cameFromLeft = false;
//    // This will not check collisions correctly if there are more platforms added to the game.
//    while (running) {
//
//
//        std::unique_lock<std::mutex> movementLock(mutex);
//        gravityCV.wait(movementLock);
//
//        bool foundCollision = false; // assume there has been no collision found yet.
//        int collisionIndex = 0;
//        for (int j = 0; j < platformVector.size(); j++) {
//            Platform* platform = platformVector.at(j);
//
//            if (player.getGlobalBounds().intersects(platform->getShape()->getGlobalBounds()) && player.getYVelocity() <= 0) { // Only count intersections if the player is falling
//                foundCollision = true;
//                collisionIndex = j;
//            }
//        }
//        if (foundCollision) { // If there was a collision at all between any of the parts
//            if (player.isAirborne()) {
//                // raise a collision of interest event here.
//                Event e(COLLISION, player.guid, currentTime);
//
//                Variant v;
//                v.type = Variant::Type::INTEGER;
//                v.intData = COLLISION_PLAT;
//                e.addArg(COLLISION_TYPE, v);
//                manager->raise(e);
//            }
//
//            Platform* intersect = platformVector.at(collisionIndex);
//            if (intersect->getXVelocity() != 0 || intersect->getYVelocity() != 0) {
//                // The platform has to be moving if the velocity isn't 0.
//                // The player moves with the platform.
//                // A position changed event will be raised here.
//                Event e(FRICTION, player.guid, currentTime);
//
//                Variant x;
//                Variant y;
//
//                x.type = Variant::Type::FLOAT;
//                x.floatData = serverDT * intersect->getXVelocity();
//                y.type = Variant::Type::FLOAT;
//                y.floatData = serverDT * intersect->getYVelocity();
//
//                e.addArg(X_DIST, x);
//                e.addArg(Y_DIST, y);
//                manager->raise(e);
//                //player.move(serverDT * intersect->getXVelocity(), serverDT * intersect->getYVelocity());
//            } // Otherwise the player should just stop moving.
//        }
//        else { // If there was no collision at all, don't raise a collision event.
//            player.setAirborne(true);
//            Event e(GRAVITY, player.guid, currentTime);
//
//            Variant y;
//
//            y.type = Variant::Type::FLOAT;
//            y.floatData = deltaT * player.getYVelocity() * -4.9 / 50;
//
//            player.setYVelocity(player.getYVelocity() + (JUMP_VELOCITY_FACTOR * deltaT));
//
//            e.addArg(Y_DIST, y);
//            manager->raise(e);
//        }
//
//        bool exitedSameSide = false;
//
//        // Now we have to see if we should translate all platforms according to the player position.
//        for (int i = 0; i < viewBoundsVector.size(); i++) {
//            Boundary* viewBound = viewBoundsVector.at(i);
//            int playerOffset = 0;
//
//
//            if (player.getGlobalBounds().intersects(viewBound->getShape()->getGlobalBounds()) && !touchingBoundary) {
//                touchingBoundary = true;
//                // should raise a collision event here.
//                // if the player touches the boundary a transform should happen.
//                if (player.getPosition().x + 12.5 < viewBound->getShape()->getPosition().x) { // If it collided from the left
//                    //offset = -800;
//                    playerOffset = -800;
//                    cameFromLeft = true;
//                    std::cout << "came from left!" << std::endl;
//                }
//                else {
//                    //offset = 0;
//                    playerOffset = 800;
//                    cameFromLeft = false;
//                    std::cout << "came from right!" << std::endl;
//                }
//                /*
//                Event e(TRANSPOSE, -1, currentTime);
//                Variant oOffset;
//                Variant pOffset;
//
//                oOffset.type = Variant::Type::INTEGER;
//                oOffset.intData = offset;
//                pOffset.type = Variant::Type::INTEGER;
//                pOffset.intData = playerOffset;
//
//
//                e.addArg(OBJ_OFFSET, oOffset);
//                e.addArg(PLAY_OFFSET, pOffset);
//
//
//                manager->raise(e);*/
//                //manager->handleEvents(deltaT); // handle all events we've collected at this point before handling more
//
//
//                //player.move(playerOffset, 0);
//            }
//            else if (!player.getGlobalBounds().intersects(viewBound->getShape()->getGlobalBounds()) && touchingBoundary) { // We need to know if it left from the left or right
//                if (cameFromLeft) { // if the player entered the boundary from the left,
//                    if (player.getPosition().x < viewBound->getShape()->getPosition().x) { // and if the player exited left,
//                        std::cout << "came from left, exited left, do nothing." << std::endl;
//                        offset = 0;
//                        playerOffset = 0;
//                        exitedSameSide = true; // don't raise an event if exited same side
//                        /*offset = 0;
//                        playerOffset = 800;*/
//
//                        // raise an event
//                        /*Event e(TRANSPOSE, -1, currentTime);
//                        Variant oOffset;
//                        Variant pOffset;
//
//                        oOffset.type = Variant::Type::INTEGER;
//                        oOffset.intData = offset;
//                        pOffset.type = Variant::Type::INTEGER;
//                        pOffset.intData = playerOffset;
//
//
//                        e.addArg(OBJ_OFFSET, oOffset);
//                        e.addArg(PLAY_OFFSET, pOffset);
//
//
//                        manager->raise(e);*/
//                        //player.move(playerOffset, 0);
//                    }
//                    else {
//                        std::cout << "came from left, exited right, transpose." << std::endl;
//                        offset = -800;
//                        playerOffset = -800;
//                    } // If it exits right nothing needs to change.
//                }
//                else { // then the player entered the boundary from the right.
//                    if (player.getPosition().x >= viewBound->getShape()->getPosition().x) { // if they left the boundary from the right,
//                        std::cout << "came from right, exited right, do nothing" << std::endl;
//                        offset = -800;
//                        playerOffset = 0;
//                        exitedSameSide = true;
//                        /*offset = -800;
//                        playerOffset = -800;*/
//
//                        // raise an event
//                        /*Event e(TRANSPOSE, -1, currentTime);
//                        Variant oOffset;
//                        Variant pOffset;
//
//                        oOffset.type = Variant::Type::INTEGER;
//                        oOffset.intData = offset;
//                        pOffset.type = Variant::Type::INTEGER;
//                        pOffset.intData = playerOffset;
//
//
//                        e.addArg(OBJ_OFFSET, oOffset);
//                        e.addArg(PLAY_OFFSET, pOffset);
//
//
//                        manager->raise(e);*/
//                        //player.move(playerOffset, 0);
//                    }
//                    else {
//                        std::cout << "came from right, exited left, do something" << std::endl;
//                        offset = 0;
//                        playerOffset = 800;
//                    }// If it enters right and exited left nothing needs to change.
//                }
//                if (!exitedSameSide) {
//                    Event e(TRANSPOSE, -1, currentTime);
//                    Variant oOffset;
//                    Variant pOffset;
//
//                    oOffset.type = Variant::Type::INTEGER;
//                    oOffset.intData = offset;
//                    pOffset.type = Variant::Type::INTEGER;
//                    pOffset.intData = playerOffset;
//
//
//                    e.addArg(OBJ_OFFSET, oOffset);
//                    e.addArg(PLAY_OFFSET, pOffset);
//
//
//                    manager->raise(e);
//                }
//                touchingBoundary = false;
//            }
//        }
//
//        // Now we have to look and see if the player is touching a kill bound.
//        for (int i = 0; i < killBoundsVector.size(); i++) {
//            Boundary* killBound = killBoundsVector.at(i);
//            if (player.getGlobalBounds().intersects(killBound->getShape()->getGlobalBounds())) {
//                // raise a died event.
//                Event e(DIED, player.guid, currentTime);
//
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = spawnVector.at(killBound->getSpawnIndex())->getShape()->getPosition().x;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = spawnVector.at(killBound->getSpawnIndex())->getShape()->getPosition().y;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//
//                //spawnVector.at(killBound->getSpawnIndex())->respawn(&player);
//                //player.setYVelocity(0);
//                //player.setAirborne(true);
//            }
//        }
//
//
//        //std::cout << "gravity thread done. unlocking." << std::endl;
//        movementLock.unlock();
//    }
//
//}
//void setupAndRegisterScripts() {
//    //dukglue_register_method(scriptManager.getContext(), &Player::setColor, "setPlayerColor");
//
//    // Handle events here instead of having to create them (a time-costly process)
//}
//
///**
//* A large amount of this code comes from the tutorials at https://www.sfml-dev.org/
//*
//*/
//int main() {
//    // establish a real time timeline.
//    //realTime = Timeline(1);
//
//    // establish game timeline based on real time timeline.
//    //gameTime = Timeline(&realTime, 1);
//    setupAndRegisterScripts();
//    gameTime.changeTick(1); // init to 1 on startup
//
//    double deltaT = 0; // init deltaT and current time
//
//    double timePassed = 0;
//
//    int offset = 0; // This is how the shapes will know to move their positions.
//
//    // create the window
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Hi Paola!");
//
//    // define a 800x100 static platform
//    StaticPlatform basePlatform(sf::Vector2f(800.f, 100.f));
//
//    // change the color to green
//    basePlatform.setFillColor(sf::Color(40, 209, 88));
//
//    // set the position of the rectangle at the bottom so it's like a platform
//    basePlatform.setPosition(sf::Vector2f(0.f, 500.f));
//
//    // define a moving platform
//    MovingPlatform movingPlatform(sf::Vector2f(100.f, 20.f));
//
//    // change the color to a nice yellow
//    movingPlatform.setFillColor(sf::Color(237, 208, 21));
//
//    // set the position of the platform to somewhere in the middle of the screen.
//    movingPlatform.setPosition(sf::Vector2f(300.f, 350.f));
//
//    movingPlatform.setVelocity(100);
//
//    // this texture comes from https://www.pinterest.com/pin/434175220311777176/
//    sf::Texture grass;
//    if (!grass.loadFromFile("grass.jpg")) {}
//    grass.setRepeated(true);
//    basePlatform.setTexture(&grass);
//
//    //bool isColliding = false;
//
//    // now add player.
//    Player player(PLAYER_RADIUS);
//
//    bool hasFocus = true;
//
//    player.setAirborne(true);
//    srand(time(NULL));
//    player.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
//    player.setPosition(sf::Vector2f(50.f, 420.f));
//
//    // Now index a map where the event handler can be found by the index.
//    player.guid = totalGameObjects;
//    totalGameObjects++;
//    manager->addToHandlers(player.guid, player.getEventHandler());
//
//    //***********************************************************************************************
//    /** CLIENT STATE */
//    //***********************************************************************************************
//#define NEW_CLIENT_ID "0 "
//    zmq::context_t context(1);
//
//    // Maintain a list of other client's players.
//    std::vector<Player*> playerList;
//    //playerList.push_back(player);
//
//    //std::vector<StaticPlatform> staticPlatformVector = {};
//    // basically a copy of the platforms from the server
//    std::vector<Platform*> platformVector = {};
//
//    std::vector<Spawn*> spawnVector = {}; // the spawns.
//
//    std::vector<Boundary*> viewBoundsVector = {}; // the view bounds.
//
//    std::vector<Boundary*> killBoundsVector = {}; // the kill bounds.
//
//    //static std::unordered_map<int, GameObject*>registry;
//
//    // First, create a subscription socket.
//    zmq::socket_t subscriberSocket(context, zmq::socket_type::sub);
//    // The socket needs to know which messages to filter out from the publisher.
//    subscriberSocket.set(zmq::sockopt::subscribe, "");
//    subscriberSocket.connect(PUB_SUB_ADDR);
//
//    // Next, create a reply socket to communicate with the server's Request socket.
//    zmq::socket_t requestSocket{ context, zmq::socket_type::req };
//    requestSocket.connect(REQ_REP_ADDR);
//
//    double serverDT = 0;
//
//    ScriptManager scriptManager = ScriptManager();
//
//    // The ID of this client.
//    //std::string clientID = CLIENT_ID;
//
//    //***********************************************************************************************
//    /** END CLIENT STATE */
//    //***********************************************************************************************
//
//    std::thread movementThread(movementThreadFunction, std::ref(player), std::ref(deltaT), std::ref(hasFocus), std::ref(offset));
//
//    //std::thread platformMovementThread(platformMovementThreadFunction, std::ref(movingPlatform), std::ref(deltaT));
//    // run the program as long as the window is open
//
//    std::thread collisionAndGravityThread(collisionAndGravityThreadFunction, std::ref(player), std::ref(platformVector), std::ref(spawnVector),
//        std::ref(viewBoundsVector), std::ref(killBoundsVector), std::ref(deltaT), std::ref(serverDT), std::ref(offset));
//
//    std::string clientID = NEW_CLIENT_ID;
//
//    // vectors to store event handlers.
//    std::vector<EventHandler*> collisions;
//    std::vector<EventHandler*> posChanges;
//    std::vector<EventHandler*> velChanges;
//    std::vector<EventHandler*> playerSpecific;
//    std::vector<EventHandler*> transpose; // every game object (including players) must be transposed when this event occurs.
//    std::vector<EventHandler*> connects;
//    std::vector<EventHandler*> initReplayPositions; // every game object must be in here so that the entire state can be saved before entering the replay.
//    std::vector<EventHandler*> gameTimeHandlers;
//    std::vector<EventHandler*> scriptHandlers;
//
//    EventHandler* playerHandler = player.getEventHandler();
//
//    timeHandler = new GameTimeHandler(&gameTime, &player);
//
//    scriptHandler = new ScriptHandler(&scriptManager);
//
//    //manager->reg(POSITION_CHANGED, std::ref(posChanges));
//    //manager->reg(VELOCITY_CHANGED, std::ref(velChanges));
//
//    // The client's own player handler index will be the first in the vector in the manager's map.
//    // This means if the client wants to update its own player's position or notify of a 
//    // collision, it only has to use the first event handler the vector returns.
//    collisions.push_back(playerHandler);
//    posChanges.push_back(playerHandler);
//    transpose.push_back(playerHandler);
//    playerSpecific.push_back(playerHandler);
//    initReplayPositions.push_back(playerHandler);
//
//    gameTimeHandlers.push_back(timeHandler);
//    scriptHandlers.push_back(scriptHandler);
//
//    //velChanges.push_back(playerHandler);
//    //playerSpecific.push_back(playerHandler); // The player gets a lot of events no other thing needs to care about,
//                                             // like pausing, changing time scale, etc.
//
//    // Other objects don't care about spawning or dying (right now).
//    manager->reg(GRAVITY, std::ref(playerSpecific));
//    manager->reg(FRICTION, std::ref(playerSpecific));
//    manager->reg(LATERAL_INPUT, std::ref(playerSpecific));
//    manager->reg(DIED, std::ref(playerSpecific));
//    manager->reg(SPAWNED, std::ref(playerSpecific));
//    manager->reg(JUMPED, std::ref(playerSpecific));
//
//    // register game time handlers.
//    manager->reg(PAUSED, std::ref(gameTimeHandlers));
//    manager->reg(UNPAUSED, std::ref(gameTimeHandlers));
//    manager->reg(CHANGED_TIMESCALE, std::ref(gameTimeHandlers));
//
//    // register script handlers! Don't know what to put here yet
//    manager->reg(INFO, std::ref(scriptHandlers));
//
//    bool first = true;
//    bool firstReg = true;
//    bool addedPlayer = false;
//
//    while (window.isOpen()) {
//        //std::unique_lock<std::mutex> movementLock(mutex);
//        // check all the window's events that were triggered since the last iteration of the loop
//        sf::Event event; // TODO get rid of this eventually
//
//
//        deltaT = gameTime.getTime() - currentTime;
//        if (deltaT > THRITY_FPS_FRAME_TIME || deltaT < 0) {
//            deltaT = 0;
//
//        }
//
//        timePassed += deltaT;
//
//        if (timePassed > 2.0) {
//            timePassed = 0;
//            dukglue_register_method(scriptManager.getContext(), &Player::setColor, "setPlayerColor");
//            scriptManager.loadScript("script.js");
//            scriptManager.runScript("run", T_PLAYER, 1, &player);
//        }
//        // At this point, the threads have completed their calcuation and every event
//        // that happened in this game loop iteration has populated the manager's queue.
//        movementCV.notify_all();
//        currentTime = gameTime.getTime();
//
//        hasFocus = window.hasFocus();
//        // clear the window with blue color
//        window.clear(sf::Color(20, 171, 222));
//
//
//
//        //***********************************************************************************************
//        /** CLIENT CODE */
//        //***********************************************************************************************
//
//        // Tacky, but send the message first to the server to tell it that we connected.
//        // We also want to do this as early as possible.
//        // We need to query the state of the game in order to know what to send.
//        // We'll start with an empty template.
//        // Since we need to send an event across the network, the clientConnected event will be constructed 
//        std::string initString = "";
//        if (!manager->isReplaying) {
//            initString.append(clientID);
//            initString.append("1 "); // We will always append a connected status when in this loop.
//            initString.append(player.to_string().append(" "));
//            initString.append(std::to_string(offset).append("\n"));
//        }
//        else {
//            initString.append(clientID);
//            initString.append("1 "); // We will always append a connected status when in this loop.
//            initString.append(player.to_string(oldPlayerPosition).append(" "));
//            initString.append(std::to_string(postRecordingOffset).append("\n"));
//        }
//
//
//        if (first) {
//            first = false;
//            connectHandler = new ConnectionHandler(&requestSocket, initString);
//            connects.push_back(connectHandler);
//
//            // register w/ manager.
//            manager->reg(NEW_CLIENT, connects);
//
//
//            // raise an event.
//            Event e(NEW_CLIENT, -1);
//
//            Variant v;
//            v.type = Variant::Type::INTEGER;
//            v.intData = 0;
//            e.addArg(ID, v);
//            manager->raise(e);
//
//            manager->handleEvents(); //request that the events be handled immediately.
//        }
//        else {
//            // The request is ready to send to the server. Hope it finds it well!
//            requestSocket.send(zmq::buffer(initString), zmq::send_flags::none);
//        }
//
//
//        // Next, the client will accept a return message from the server.
//        // The message that is returned is important! It contains the ID of this client
//        // (in case it changed).
//        zmq::message_t issuedClientID;
//        requestSocket.recv(issuedClientID, zmq::recv_flags::none);
//        // We need to cast this to a readable string now:
//        std::string replyString = std::string(static_cast<char*>(issuedClientID.data()), issuedClientID.size());
//        // We have to parse this string a bit:
//        std::istringstream bootlegScanner(replyString);
//        int currentInt;
//        double currentDouble;
//
//        bootlegScanner >> currentInt;
//        int id = currentInt;
//
//        bootlegScanner >> currentDouble;
//        serverDT = currentDouble;
//
//        std::string newID = std::to_string(currentInt).append(" ");
//        clientID = newID;
//
//        // This ID may not be different than before. Most of the time it won't be different.
//        // It only changes when a client disconnects.
//
//        // This is handy for getting the index of this client.
//        int clientOwnIndex = stoi(clientID) - 1;
//
//        // Now we are ready to receive subscriber messages that the server puts out.
//        zmq::message_t subscriberMessage;
//        subscriberSocket.recv(subscriberMessage, zmq::recv_flags::none);
//
//        // Great! So now, we've received the message.
//        // It's a string that WAS a JSON object, so we'll convert it to a JSON now:
//        std::string stringMessage = std::string(static_cast<char*>(subscriberMessage.data()), subscriberMessage.size());
//        json jasonMessage = json::parse(stringMessage);
//
//        // We should now have a JSON object. Get the plethora of lists from the json object:
//        std::vector<float> playerInfoVector = jasonMessage["playerInfo"].get<std::vector<float>>();
//        std::vector<int> disconnects = jasonMessage["disconnected"].get<std::vector<int>>();
//        std::vector<std::string> platformInfo = jasonMessage["platformInfo"].get<std::vector<std::string>>();
//        std::vector<std::string> spawnInfo = jasonMessage["spawnInfo"].get<std::vector<std::string>>();
//        std::vector<std::string> viewBoundsInfo = jasonMessage["viewBounds"].get<std::vector<std::string>>();
//        std::vector<std::string> killBoundsInfo = jasonMessage["killBounds"].get<std::vector<std::string>>();
//
//        // Now we should check if there has been a new player added or subtracted from the player info vector.
//        int playerListSize = playerList.size();
//        int infoSize = playerInfoVector.size() / 5;
//        if (playerListSize < infoSize) { // there was a new player added.
//            int diff = infoSize - playerListSize;
//            for (int i = diff; i > 0; i--) {
//
//                float newPlayerX = playerInfoVector.at((5 * i) - 5);
//                float newPlayerY = playerInfoVector.at((5 * i) - 4);
//                float newPlayerR = playerInfoVector.at((5 * i) - 3);
//                float newPlayerG = playerInfoVector.at((5 * i) - 2);
//                float newPlayerB = playerInfoVector.at((5 * i) - 1);
//
//                sf::Color clientPlayerColor = player.getFillColor();
//
//                //if (newPlayerR == clientPlayerColor.r && newPlayerG == clientPlayerColor.g && newPlayerB == clientPlayerColor.b) {
//                //    // we have the same player, don't add them to the list of players.
//                //    break;
//                //}
//                addedPlayer = true;
//
//                // create a local vector that allows the event to be registered and handled immediately
//                Player* newPlayer = new Player(PLAYER_RADIUS);
//                newPlayer->guid = totalGameObjects;
//                totalGameObjects++;
//                EventHandler* newPlayerHandler = newPlayer->getEventHandler();
//                manager->addToHandlers(newPlayer->guid, newPlayerHandler);
//                // i ends up being the offset for the missing players in this vector.
//                // That's why I multiply everything by 5.
//                // this is rather unsafe, since two players could end up having the same color.
//                // but 1 / 256^3 times does not seem very likely. 
//
//                // Since the position of the networked player has been updated, raise a position changed event.
//                // need to register events first.
//
//                posChanges.push_back(newPlayerHandler);
//                transpose.push_back(newPlayerHandler);
//                initReplayPositions.push_back(newPlayerHandler);
//
//                //newPlayer.setPosition(sf::Vector2f(newPlayerX, newPlayerY));
//                //std::cout << "set same player" << std::endl;
//                // the position changed for the new player, raise an event w/ index.
//                Event e(POSITION_CHANGED, newPlayer->guid, currentTime); // we just registered a handler for this player.
//                // Index in player event handlers is the same as order and size of local player list.
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = newPlayerX;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = newPlayerY;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//                //manager->handleEvents();
//                newPlayer->setFillColor(sf::Color(newPlayerR, newPlayerG, newPlayerB));
//                playerList.push_back(newPlayer);
//                //std::cout << "new player joined." << std::endl;
//            }
//
//
//        }
//        else if (playerListSize > infoSize) { // Otherwise, there was a disconnect that happened. Remove the local instances of the players that we have.
//            for (int i = 0; i < disconnects.size(); i++) {
//                int removedIndex = disconnects.at(0);
//                playerList.erase(playerList.begin() + removedIndex);
//                disconnects.erase(disconnects.begin());
//                posChanges.erase(posChanges.begin() + removedIndex); // need to update registration too.
//            }
//        }
//        else { // The final case, if we just need to update our local list of players.
//            for (int i = 0; i < playerListSize; i++) {
//                if (i == clientOwnIndex) { // If the client is trying to set what the server says about the player, we should skip it.
//                    continue;
//                }
//                float updatedPlayerX = playerInfoVector.at((5 * i)) + offset;
//                float updatedPlayerY = playerInfoVector.at((5 * i) + 1);
//
//                // would raise an event here with the index of the player's pos
//                // Each player (but the local player) will raise a position changed event
//                // with an index that corresponds to their registration index. Could
//                // foolproof by comparing pointers but the order is nice, so this is
//                // not needed.
//
//                if (playerList[i]->getPosition().x != updatedPlayerX || playerList[i]->getPosition().y != updatedPlayerY) {
//                    Event e(POSITION_CHANGED, playerList[i]->guid, currentTime);
//                    Variant x;
//                    Variant y;
//                    x.type = Variant::Type::FLOAT;
//                    x.floatData = updatedPlayerX;
//                    y.type = Variant::Type::FLOAT;
//                    y.floatData = updatedPlayerY;
//
//                    e.addArg(X_POS, x);
//                    e.addArg(Y_POS, y);
//                    manager->raise(e);
//                }
//
//                //manager->handleEvents();
//
//                //playerList.at(i).setPosition(sf::Vector2f(updatedPlayerX, updatedPlayerY));
//
//                // Definitely extra work, but since the code above may mess up around the first iterations,
//                // The colors might not be set correctly.
//                float updatedPlayerR = playerInfoVector.at((5 * i) + 2);
//                float updatedPlayerG = playerInfoVector.at((5 * i) + 3);
//                float updatedPlayerB = playerInfoVector.at((5 * i) + 4);
//                playerList.at(i)->setFillColor(sf::Color(updatedPlayerR, updatedPlayerG, updatedPlayerB));
//
//            }
//        }
//
//        if (platformVector.size() == 0) { // We haven't created the moving platforms locally yet!
//
//            for (int i = 0; i < platformInfo.size(); i++) {
//                std::istringstream bootlegScanner(platformInfo.at(i));
//                int currentInt;
//                float currentFloat;
//                std::string currentString;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                // new width
//                bootlegScanner >> currentFloat;
//                float newWidth = currentFloat;
//
//                // new height
//                bootlegScanner >> currentFloat;
//                float newHeight = currentFloat;
//
//                // new xVelocity
//                bootlegScanner >> currentFloat;
//                float xVel = currentFloat;
//
//                // new yVelocity
//                bootlegScanner >> currentFloat;
//                float yVel = currentFloat;
//
//                // new red
//                bootlegScanner >> currentFloat;
//                float newR = currentFloat;
//
//                // new green
//                bootlegScanner >> currentFloat;
//                float newG = currentFloat;
//
//                // new blue
//                bootlegScanner >> currentFloat;
//                float newB = currentFloat;
//
//                // new isVisible
//                bootlegScanner >> currentInt;
//                int isVisible = currentInt;
//
//                // new texture
//                bootlegScanner >> currentString;
//                std::string newText = currentString;
//
//                sf::RectangleShape* newShape = new sf::RectangleShape(sf::Vector2f(newWidth, newHeight));
//                // newShape->setPosition(sf::Vector2f(newXPos, newYPos));
//                newShape->setFillColor(sf::Color(newR, newG, newB));
//                newShape->setSize(sf::Vector2f(newWidth, newHeight));
//
//                Platform* newPlat = new Platform(newShape, isVisible, xVel, yVel, 0, newText);
//                newPlat->setTexture(newText);
//
//                EventHandler* currHandler = newPlat->getHandler();
//                newPlat->guid = totalGameObjects;
//                totalGameObjects++;
//                manager->addToHandlers(newPlat->guid, currHandler);
//
//                platformVector.push_back(newPlat);
//
//
//                // we just created the game world, register event types correctly:
//                // each platform cares about position changes
//
//
//                // Now that it's registered we need to create an event that will go with it.
//
//                Event e(POSITION_CHANGED, newPlat->guid, currentTime); // the index for this event is the current size of the position changes vector.
//
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = newXPos;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = newYPos;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//
//                posChanges.push_back(currHandler);
//                velChanges.push_back(currHandler);
//                transpose.push_back(currHandler);
//                initReplayPositions.push_back(currHandler);
//            }
//
//        }
//        else {
//            for (int i = 0; i < platformVector.size(); i++) {
//                std::istringstream bootlegScanner(platformInfo.at(i));
//                int currentInt;
//                float currentFloat;
//
//                // new X
//                bootlegScanner >> currentFloat;
//                float newX = currentFloat;
//
//                // new Y
//                bootlegScanner >> currentFloat;
//                float newY = currentFloat;
//
//                // new width
//                bootlegScanner >> currentFloat;
//                //float newWidth = currentFloat;
//
//                // new height
//                bootlegScanner >> currentFloat;
//                //float newHeight = currentFloat;
//
//                 // new xVelocity
//                bootlegScanner >> currentFloat;
//                float xVel = currentFloat;
//
//                // new yVelocity
//                bootlegScanner >> currentFloat;
//                float yVel = currentFloat;
//
//                // would raise an event here that sets the pos of the platform and the velocity, and this platform's index.
//                if ((platformVector[i]->getShape()->getPosition().x != newX || platformVector[i]->getShape()->getPosition().y != newY) && !manager->isReplaying) {
//                    Event e(POSITION_CHANGED, platformVector[i]->guid, currentTime); // offset into position changed handler is num players - 1 + i
//                    Variant x;
//                    Variant y;
//                    x.type = Variant::Type::FLOAT;
//                    x.floatData = newX + platformVector[i]->viewOffset;
//                    y.type = Variant::Type::FLOAT;
//                    y.floatData = newY;
//
//                    e.addArg(X_POS, x);
//                    e.addArg(Y_POS, y);
//
//                    manager->raise(e);
//                }
//                if ((platformVector[i]->getXVelocity() != xVel || platformVector[i]->getYVelocity() != yVel) && !manager->isReplaying) {
//                    // The velocity also changed!
//                    Event v(VELOCITY_CHANGED, platformVector[i]->guid, currentTime);
//
//                    Variant xV;
//                    Variant yV;
//                    xV.type = Variant::Type::FLOAT;
//                    xV.floatData = xVel;
//                    yV.type = Variant::Type::FLOAT;
//                    yV.floatData = yVel;
//
//                    v.addArg(X_VEL, xV);
//                    v.addArg(Y_VEL, yV);
//
//                    manager->raise(v);
//                }
//                //platformVector.at(i)->getShape()->setPosition(sf::Vector2f(newX + offset, newY));
//                //platformVector.at(i)->setXVelocity(xVel);
//                //platformVector.at(i)->setYVelocity(yVel);
//
//            }
//        }
//
//        if (spawnVector.size() == 0) { // We don't have any spawns yet, create them!
//            for (int i = 0; i < spawnInfo.size(); i++) {
//                std::istringstream bootlegScanner(spawnInfo.at(i));
//                float currentFloat;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                Spawn* newSpawn = new Spawn(newXPos, newYPos);
//
//                newSpawn->guid = totalGameObjects;
//                totalGameObjects++;
//                EventHandler* currHandler = newSpawn->getHandler();
//                manager->addToHandlers(newSpawn->guid, currHandler);
//
//                spawnVector.push_back(newSpawn);
//
//
//                // Now add the handlers. 
//                // Spawns only care about position updates. they don't move except when the viewpoint changes.
//
//
//                Event e(POSITION_CHANGED, posChanges.size(), currentTime); // this index corresponds to number of total things registered right now.
//
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = newXPos;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = newYPos;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//                posChanges.push_back(currHandler);
//                transpose.push_back(currHandler);
//                initReplayPositions.push_back(currHandler);
//            }
//        }
//        else {
//            for (int i = 0; i < spawnInfo.size(); i++) {
//                std::istringstream bootlegScanner(spawnInfo.at(i));
//                float currentFloat;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                // TODO would raise an event that changes the position of the spawn.
//                // spawnVector.at(i)->getShape()->setPosition(sf::Vector2f(newXPos + offset, newYPos));
//                if (spawnVector[i]->getShape()->getPosition().x != newXPos || spawnVector[i]->getShape()->getPosition().y != newYPos) {
//                    Event e(POSITION_CHANGED, spawnVector[i]->guid, currentTime); // offset into position changed handler is num players - 1 + i
//
//                    Variant x;
//                    Variant y;
//                    x.type = Variant::Type::FLOAT;
//                    x.floatData = newXPos + spawnVector[i]->viewOffset;
//                    y.type = Variant::Type::FLOAT;
//                    y.floatData = newYPos;
//
//                    e.addArg(X_POS, x);
//                    e.addArg(Y_POS, y);
//
//                    manager->raise(e);
//                }
//            }
//        }
//
//
//        if (viewBoundsVector.size() == 0) { // We dont' have any view bounds yet! create them.
//            for (int i = 0; i < viewBoundsInfo.size(); i++) {
//                std::istringstream bootlegScanner(viewBoundsInfo.at(i));
//                float currentFloat;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                // new width
//                bootlegScanner >> currentFloat;
//                float newWidth = currentFloat;
//
//                // new height
//                bootlegScanner >> currentFloat;
//                float newHeight = currentFloat;
//
//                Boundary* newViewBound = new Boundary(newWidth, newHeight, newXPos, newYPos, 0);
//
//                newViewBound->guid = totalGameObjects;
//                totalGameObjects++;
//                EventHandler* currHandler = newViewBound->getHandler();
//                manager->addToHandlers(newViewBound->guid, currHandler);
//
//                viewBoundsVector.push_back(newViewBound);
//
//                // Now add event handlers.
//                // view bounds care about position changes.
//
//                Event e(POSITION_CHANGED, viewBoundsVector[i]->guid, currentTime); // offset into position changed handler is num players - 1 + i
//
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = newXPos;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = newYPos;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//
//                posChanges.push_back(currHandler);
//                transpose.push_back(currHandler);
//                initReplayPositions.push_back(currHandler);
//            }
//        }
//        else {
//            for (int i = 0; i < viewBoundsInfo.size(); i++) {
//                std::istringstream bootlegScanner(viewBoundsInfo.at(i));
//                float currentFloat;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                //viewBoundsVector.at(i)->getShape()->setPosition(sf::Vector2f(newXPos + offset, newYPos));
//
//                if (viewBoundsVector[i]->getShape()->getPosition().x != newXPos || viewBoundsVector[i]->getShape()->getPosition().y != newYPos) {
//                    Event e(POSITION_CHANGED, viewBoundsVector[i]->guid, currentTime);
//
//                    Variant x;
//                    Variant y;
//                    x.type = Variant::Type::FLOAT;
//                    x.floatData = newXPos + viewBoundsVector[i]->viewOffset;
//                    y.type = Variant::Type::FLOAT;
//                    y.floatData = newYPos;
//
//                    e.addArg(X_POS, x);
//                    e.addArg(Y_POS, y);
//
//                    manager->raise(e);
//                }
//            }
//        }
//
//        if (killBoundsVector.size() == 0) { // We don't have any kill bounds, create them!
//            for (int i = 0; i < killBoundsInfo.size(); i++) {
//                std::istringstream bootlegScanner(killBoundsInfo.at(i));
//                float currentFloat;
//                int currentInt;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                // new width
//                bootlegScanner >> currentFloat;
//                float newWidth = currentFloat;
//
//                // new height
//                bootlegScanner >> currentFloat;
//                float newHeight = currentFloat;
//
//                // new spawn index
//                bootlegScanner >> currentInt;
//                int newIndex = currentInt;
//
//                Boundary* newKillBound = new Boundary(newWidth, newHeight, newXPos, newYPos, newIndex);
//
//                newKillBound->guid = totalGameObjects;
//                totalGameObjects++;
//                EventHandler* currHandler = newKillBound->getHandler();
//                manager->addToHandlers(newKillBound->guid, currHandler);
//
//                killBoundsVector.push_back(newKillBound);
//
//                // now raise an event that the position has been changed.
//                Event e(POSITION_CHANGED, newKillBound->guid, currentTime);
//
//                Variant x;
//                Variant y;
//                x.type = Variant::Type::FLOAT;
//                x.floatData = newXPos;
//                y.type = Variant::Type::FLOAT;
//                y.floatData = newYPos;
//
//                e.addArg(X_POS, x);
//                e.addArg(Y_POS, y);
//
//                manager->raise(e);
//
//                posChanges.push_back(currHandler);
//                transpose.push_back(currHandler);
//                initReplayPositions.push_back(currHandler);
//            }
//        }
//        else {
//            for (int i = 0; i < killBoundsInfo.size(); i++) {
//                std::istringstream bootlegScanner(killBoundsInfo.at(i));
//                float currentFloat;
//                int currentInt;
//
//                // new x pos
//                bootlegScanner >> currentFloat;
//                float newXPos = currentFloat;
//
//                // new y pos
//                bootlegScanner >> currentFloat;
//                float newYPos = currentFloat;
//
//                //killBoundsVector.at(i)->getShape()->setPosition(sf::Vector2f(newXPos + offset, newYPos));
//
//                if (killBoundsVector[i]->getShape()->getPosition().x != newXPos || killBoundsVector[i]->getShape()->getPosition().y != newYPos) {
//                    Event e(POSITION_CHANGED, killBoundsVector[i]->guid, currentTime);
//                    // offset is num players + num plats + num spawns + num view bounds + i
//
//                    Variant x;
//                    Variant y;
//                    x.type = Variant::Type::FLOAT;
//                    x.floatData = newXPos + killBoundsVector[i]->viewOffset;
//                    y.type = Variant::Type::FLOAT;
//                    y.floatData = newYPos;
//
//                    e.addArg(X_POS, x);
//                    e.addArg(Y_POS, y);
//
//                    manager->raise(e);
//                }
//            }
//        }
//        if (firstReg) {
//            firstReg = false;
//            manager->reg(COLLISION, std::ref(collisions));
//            manager->reg(POSITION_CHANGED, std::ref(posChanges));
//            manager->reg(VELOCITY_CHANGED, std::ref(velChanges));
//            manager->reg(TRANSPOSE, std::ref(transpose));
//            manager->reg(BEGIN_REC, std::ref(initReplayPositions));
//        }
//        if (addedPlayer) {
//            manager->reg(POSITION_CHANGED, std::ref(posChanges));
//            manager->reg(TRANSPOSE, std::ref(transpose));
//            manager->reg(BEGIN_REC, std::ref(initReplayPositions));
//        }
//        // will cause a chain of position changed events, started in event handler
//        /**
//        * So at this point, all events in the main thread have been raised and indexed.
//        */
//
//        if (shouldTeleport) {
//            shouldTeleport = false;
//            dukglue_register_method(scriptManager.getContext(), &EventManager::raisePtr, "raise");
//            dukglue_register_property(scriptManager.getContext(), nullptr, &Variant::setData, "data");
//            dukglue_register_method(scriptManager.getContext(), &Event::addArgPtr, "addArg");
//            Event e(POSITION_CHANGED, player.guid, 0);
//
//            Variant x;
//            Variant y;
//            x.type = Variant::Type::FLOAT;
//            x.floatData = 0;
//            y.type = Variant::Type::FLOAT;
//            y.floatData = 0;
//
//            e.addArg(X_POS, x);
//            e.addArg(Y_POS, y);
//
//            scriptManager.loadScript("script.js");
//            scriptManager.runScript("teleport", T_EVENT, 4, manager, &e, &x, &y);
//        }
//
//        if (recordingStarted) {
//            recordingStarted = false;
//            Event e(BEGIN_REC, -1, currentTime); // applies to all handlers listed in initReplayPositions
//            manager->raise(e);
//            std::cout << "raised BEGIN_REC event" << std::endl;
//            manager->handleEvents(); // ensures all events have been handled, and manager map has been set.
//            manager->isRecording = true;
//
//            // the manager handles this event, populates the queue, then empties it again.
//            // at this point, the initial positions have been saved, and the EventManager should be recording events as they happen.
//        }
//        else if (recordingEnded) {
//            recordingEnded = false;
//            Event e(END_REC, player.guid, currentTime);
//            manager->raise(e); // hopefully ensures that this event is raised last.
//            std::cout << "raised END_REC event" << std::endl;
//            manager->handleEvents(); // this causes a couple more events to be raised, but the queue should be empty after this one step.
//        }
//
//        if (manager->isReplaying) {
//            //std::cout << "deltaT: " << deltaT << " time scale: " << gameTime.getTick() << std::endl;
//            manager->handleEvents(deltaT);
//        }
//        else {
//            manager->handleEvents();
//        }
//
//
//
//
//        /**
//        // Now since we have all this information, update the positions of everything (but the
//        // platform, we'll do that in a second.
//        // Check for the own client's index first.
//
//        // If there are more players in the list than there are players that we know of here, we're going
//        // to need another space to store the player in the list.
//        int serverPlayerCount = playerInfoVector.size() / 5; // The format inside of the vector is the x and y and then the color of the player.
//        int localPlayerCount = playerList.size();
//        if (serverPlayerCount > localPlayerCount) {
//            while (localPlayerCount < serverPlayerCount) {
//                std::cout << "added another player!" << std::endl;
//                playerList.push_back(Player(25.f)); // This can account for any differences in total player counts.
//                localPlayerCount++;
//            }
//
//        }
//
//        // Now update the positions and colors of the local players.
//        for (int i = 0; i < serverPlayerCount; i++) {
//            playerList.at(i).setPosition(xCoordsList.at(i), yCoordsList.at(i));
//            playerList.at(i).setFillColor(sf::Color(redsList.at(i), greensList.at(i), bluesList.at(i)));
//        }
//        */
//
//
//        // And go back to the top!
//
//        //***********************************************************************************************
//        /** END CLIENT CODE */
//        //***********************************************************************************************
//
//
//        while (window.pollEvent(event)) {
//            // "close requested" event: we close the window
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//        }
//
//        // draw everything here...
//        // We need a loop to draw the players.
//        for (int i = 0; i < platformVector.size(); i++) {
//            sf::RectangleShape shape = *(platformVector.at(i)->getShape());
//            window.draw(shape);
//        }
//
//        for (int i = 0; i < playerList.size(); i++) {
//            if (i == clientOwnIndex) {
//                continue;
//            }
//            window.draw(*playerList.at(i));
//        }
//        window.draw(player);
//
//        // end the current frame
//        window.display();
//    }
//    //std::unique_lock<std::mutex> movementLock(mutex); // don't edit while another thread might be looking
//    running = false;
//
//    movementCV.notify_all(); // let all the threads know to terminate
//
//    movementThread.join();
//    //platformMovementThread.join();
//    collisionAndGravityThread.join();
//
//    // send a message to the server that this client has disconnected:
//    requestSocket.send(zmq::buffer(clientID.append(DISCONNECT_STRING)), zmq::send_flags::none);
//
//    return 0;
//}