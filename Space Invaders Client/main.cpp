#include "GameStateManager.h"
#include "GameObject.h"

#include <zmq.hpp>
#include <json.hpp>

/** The frame time for 60 FPS*/
#define THRITY_FPS_FRAME_TIME 0.0666

#define PLAYER_VELOCITY 250

// request reply address.
#define REQ_REP_ADDR "tcp://localhost:5555"

bool running = true;

static int numGameObjects = 0;

Timeline realTime = Timeline(1);
Timeline gameTime = Timeline(&realTime, 1);

EventHandler* timeHandler;

EventHandler* connectionHandler;

double playerVelocity;

void readInput(GameObject& player, double& deltaT, bool& hasFocus, double& currentTime) {


    EventManager* manager = manager->getInstance();

    //std::cout << "deltaT passed in is " << deltaT << std::endl;

    // if the user wants to move left
    //std::cout << "Moved player, xVel: " << player.getXVelocity() << " deltaT: " << deltaT << std::endl;
    //player.move(player.getXVelocity() * deltaT, 0);

    // Even if the player did not end up moving, we will still tell the event manager the position
    // of the player moved.

    if (hasFocus) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            // raise an event that says to move the character
            Event e(KEYBOARD_INPUT, player.guid, 0);
            Variant v;
            Variant d;

            v.type = Variant::Type::DOUBLE;
            v.doubleData = -playerVelocity;

            d.type = Variant::Type::DOUBLE;
            d.doubleData = deltaT;

            e.addArg(X_VEL, v);
            e.addArg(DELTAT, d);
            manager->raise(e);
        }
        // if the user wants to move right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            Event e(KEYBOARD_INPUT, player.guid, 0);
            Variant v;
            Variant d;

            v.type = Variant::Type::DOUBLE;
            v.doubleData = playerVelocity;

            d.type = Variant::Type::DOUBLE;
            d.doubleData = deltaT;

            e.addArg(X_VEL, v);
            e.addArg(DELTAT, d);
            manager->raise(e);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            GameStateManager* state = state->getInstance();
            if (state->canPlayerShoot()) {
                // shoot laser if the player can
                Event e(SHOOT, player.guid, 0);
                manager->raise(e);
            }
            // otherwise do nothing.
        }
    }

}
void changePlayerVelocity(double newVelocity) {
    if (newVelocity < 20) {
        newVelocity = 20;
    }
    else if (newVelocity > 1000) {
        newVelocity = 1000;
    }
    playerVelocity = newVelocity;
}

int main() {
    sf::Event event;
    sf::RenderWindow window(sf::VideoMode(800, 900), "Hi Paola!");

    double deltaT = 0;
    double currentTime = 0;

    bool hasFocus = true;

    playerVelocity = PLAYER_VELOCITY; // init player velocity to default in case script disappears.

    // Networking
    // establish context.
    zmq::context_t context(1);
    // Next, create a reply socket to communicate with the server's Request socket.
    zmq::socket_t requestSocket{ context, zmq::socket_type::req };
    requestSocket.connect(REQ_REP_ADDR);

    connectionHandler = new ConnectionHandler(&requestSocket, &numGameObjects);

    // init data structures to store game objects.
    // also create game objects to init world.
    GameStateManager* state = state->getInstance();
    EventManager* manager = manager->getInstance();


    ScriptManager scripter = ScriptManager();
    dukglue_register_function(scripter.getContext(), &changePlayerVelocity, "setPlayerVelocity");
    dukglue_register_method(scripter.getContext(), &Player::setInvincible, "makeInvincible");

    // init event handler storage.
    std::vector<EventHandler*> playerHandlers;
    std::vector<EventHandler*> connectionHandlers;

    // create a player and init location.
    GameObject* player = new GameObject(PLAYER);
    player->setPosition(sf::Vector2f(40, 750));
    player->guid = numGameObjects++;
    manager->addToHandlers(player->guid, player->getHandler());
    playerHandlers.push_back(player->getHandler());
    state->addPlayer(player);

    manager->reg(PLAYER_DIED, std::ref(playerHandlers));
    
    connectionHandlers.push_back(connectionHandler);
    manager->reg(UPDATE_SCORE, std::ref(connectionHandlers));
    manager->reg(PLAYER_LOST_LIFE, std::ref(connectionHandlers));
    manager->reg(WON, std::ref(connectionHandlers));

    // create enemies and init locations.
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            GameObject* enemy = new GameObject(ENEMY);
            enemy->setPosition(sf::Vector2f(50 * j + 30, i * 55 + 20)); // no rhyme or reason to these.
            enemy->guid = numGameObjects++;
            manager->addToHandlers(enemy->guid, enemy->getHandler());

            state->addEnemy(enemy);
        }
    }


    

    while (window.isOpen()) {
        // check to see if the window has focus.
        hasFocus = window.hasFocus();
        // clear the window with black color.
        window.clear(sf::Color(0, 0, 0));

        // get game time.
        deltaT = gameTime.getTime() - currentTime;
        if (deltaT > THRITY_FPS_FRAME_TIME || deltaT < 0) {
            deltaT = 0;

        }

        // load in and run scripts.
        scripter.loadScript("script.js");
        scripter.runScript("playerBehavior", VELOCITY_CHANGE, 0);
        scripter.runScript("setInvincible", INVINCIBLE, 1, player->getPlayer());


        readInput(std::ref(*player), std::ref(deltaT), std::ref(hasFocus), std::ref(currentTime)); // read input.

        currentTime = gameTime.getTime();

        state->checkCollisions(currentTime);
        state->doMovement(deltaT);
        state->makeEnemiesShoot(currentTime);

        // all events that are necessary have been added.
        manager->handleEvents();

        state->drawGameBoard(&window);

        window.display(); // actually display the items.
        // all dead objects are deallocated.
        state->cleanUpObjects();

        state->checkWinConditions(currentTime);

        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
    running = false;

}