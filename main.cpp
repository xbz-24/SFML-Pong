#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <cmath> 
#include <cstdlib> 
#include <ctime> 

const float SCALE = 30.f; 
const float MIN_HORIZONTAL_SPEED = 50.5f;
const float PADDLE_SPEED = 50.0f;
const float MAX_BALL_SPEED = 60.0f;
const float PADDLE_HEIGHT_SCALE = 0.25; 
const float ballSpeedMultiplier = 90.2f;
const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT= 1080;
const unsigned int SCREEN_REFRESH_RATE = 60;
//  ../images/bola.png
const std::string path2ball = "../images/fireball32.png";
// ../images/paleta.png
const std::string path2bar = "../images/resized_tile_2.png"; 

const std::string path2reset = "../images/refresh11.png"; 

unsigned int playerScore = 0;
unsigned int enemyScore = 0;

void updateBackground(sf::Sprite& sprite1, sf::Sprite& sprite2, const sf::Texture& texture, float moveSpeed, float deltaTime);
bool loadTexture(sf::Texture &texture, const std::string &path, bool applyMask = false);
void initBall(sf::CircleShape &shape, sf::Texture &ballTexture, sf::RenderWindow &window);
void initPaddle(sf::Sprite &sprite, sf::Texture &barTexture, float xPosition, sf::RenderWindow &window);
void resetBall(b2Body* ballBody, sf::RenderWindow &window, sf::Text &scoreText);
void initResetButton(sf::Sprite &sprite, sf::Texture &texture, sf::RenderWindow &window, sf::Text &scoreText);
void handlePaddleMovement(sf::Sprite &sprite, sf::Texture &texture, b2Body* body, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, sf::RenderWindow &window);
void adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window, sf::Text &scoreText);
void updateSpritePosition(sf::Sprite &sprite, b2Body* body);
void updateSpritePosition(sf::CircleShape &shape, b2Body* body);
void updateScoreDisplay(sf::Text &scoreText);

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); 
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    //sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Pong", sf::Style::Fullscreen, settings);
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    for (const auto& mode : modes) {
        //std::cout << "Mode: " << mode.width << "x" << mode.height << std::endl;
    }
    //sf::RenderWindow window(modes[0], "Pong", sf::Style::Fullscreen, settings);
    //sf::RenderWindow window(sf::VideoMode(1920, 1080), "Pong", sf::Style::Fullscreen, settings);
    //sf::RenderWindow window(sf::VideoMode(1920, 1080), "Pong", sf::Style::Titlebar | sf::Style::Close, settings);
    //sf::RenderWindow window(sf::VideoMode(modes[0].height,modes[0].width,60), "Pong", sf::Style::Titlebar | sf::Style::Close, settings);

    // sf::VideoMode params: WIDTH, HEIGHT, BITS PER PIXEL
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pong", sf::Style::Titlebar | sf::Style::Close, settings);

    window.setPosition(sf::Vector2i(0,0)); // Positions the window at the top-left corner of the primary monitor
    window.setFramerateLimit(SCREEN_REFRESH_RATE);
    
    sf::Font font;
    if (!font.loadFromFile("../fonts/HackNerdFont-Regular.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10,10);
    sf::Clock fpsClock;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1, backgroundSprite2;
    //../images/80-retro-mountains.png
    if(!backgroundTexture.loadFromFile("../images/nature_6/origbig.png")){
        std::cerr << "Failed to load background texture!"<<std::endl;
        return -1;
    }
    backgroundSprite1.setTexture(backgroundTexture);
    backgroundSprite2.setTexture(backgroundTexture);

    // backgroundSprite.setScale(
    //     window.getSize().x / backgroundSprite.getLocalBounds().width,
    //     window.getSize().y / backgroundSprite.getLocalBounds().height
    // );
    backgroundSprite2.setPosition(sf::Vector2f(backgroundTexture.getSize().x, 0));

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(window.getSize().x - 100, 10);
    updateScoreDisplay(scoreText);  

    sf::Texture ballTexture;
    if (!loadTexture(ballTexture, path2ball, true)) return -1;
    sf::CircleShape shape(12.f);
    initBall(shape, ballTexture, window);

    sf::Texture barTexture;
    if (!loadTexture(barTexture, path2bar, true)) return -1;
    sf::Sprite leftBarSprite;
    initPaddle(leftBarSprite, barTexture, 24, window);
    sf::Sprite rightBarSprite;
    initPaddle(rightBarSprite, barTexture, window.getSize().x - 24, window);

    sf::Texture resetTexture;
    if (!loadTexture(resetTexture, path2reset)) return -1;
    sf::Sprite resetSprite;
    initResetButton(resetSprite, resetTexture, window, scoreText);

    b2Vec2 gravity(0.f, 0.f); 
    b2World world(gravity);

    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.bullet = true;
    ballBodyDef.position.Set(shape.getPosition().x / SCALE, shape.getPosition().y / SCALE);
    b2Body* ballBody = world.CreateBody(&ballBodyDef);

    b2CircleShape ballCircle;
    ballCircle.m_radius = shape.getRadius() / SCALE;

    b2FixtureDef ballFixtureDef;
    ballFixtureDef.shape = &ballCircle;
    ballFixtureDef.density = 1.f;
    ballFixtureDef.restitution = 1.0f;
    ballBody->CreateFixture(&ballFixtureDef);

    b2BodyDef leftBarBodyDef;
    leftBarBodyDef.type = b2_kinematicBody;
    leftBarBodyDef.position.Set(leftBarSprite.getPosition().x / SCALE, leftBarSprite.getPosition().y / SCALE);
    b2Body* leftBarBody = world.CreateBody(&leftBarBodyDef);

    b2PolygonShape leftBarShape;
    leftBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, 
                          (barTexture.getSize().y * PADDLE_HEIGHT_SCALE/ 2) / SCALE);
    leftBarBody->CreateFixture(&leftBarShape, 0.0f);

    b2BodyDef rightBarBodyDef;
    rightBarBodyDef.type = b2_kinematicBody;
    rightBarBodyDef.position.Set(rightBarSprite.getPosition().x / SCALE, rightBarSprite.getPosition().y / SCALE);
    b2Body* rightBarBody = world.CreateBody(&rightBarBodyDef);

    b2PolygonShape rightBarShape;
    rightBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, 
                           (barTexture.getSize().y * PADDLE_HEIGHT_SCALE/ 2) / SCALE);
    rightBarBody->CreateFixture(&rightBarShape, 0.0f);

    float angle = (45 + (rand() % 10 - 5)) * (b2_pi / 180);
    sf::Clock deltaClock;
    ballBody->SetLinearVelocity(b2Vec2(22 * cos(angle) * ballSpeedMultiplier,
                                        2 * sin(angle) * ballSpeedMultiplier)); 

    sf::Clock clock;
    float moveSpeed = -100.0f; 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } 
            else if (event.type == sf::Event::MouseButtonPressed && resetSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                resetBall(ballBody, window, scoreText);
                playerScore = 0;
                enemyScore = 0;
                updateScoreDisplay(scoreText);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetBall(ballBody, window, scoreText);
                playerScore = 0;
                enemyScore = 0;
                updateScoreDisplay(scoreText);
            }
        }

        float deltaTime = clock.restart().asSeconds();

        handlePaddleMovement(leftBarSprite, barTexture, leftBarBody, sf::Keyboard::W, sf::Keyboard::S,  window);
        handlePaddleMovement(rightBarSprite, barTexture, rightBarBody, sf::Keyboard::O, sf::Keyboard::K,  window);

        world.Step(1/60.f, 12, 8);

        adjustBallVelocity(ballBody, shape, window, scoreText);
        updateScoreDisplay(scoreText);

        updateSpritePosition(shape, ballBody);
        updateSpritePosition(leftBarSprite, leftBarBody);
        updateSpritePosition(rightBarSprite, rightBarBody);

        float frameTime = fpsClock.restart().asSeconds();
        float fps = 1.0f / frameTime;
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        updateBackground(backgroundSprite1, backgroundSprite2, backgroundTexture, moveSpeed, deltaTime);
        //window.clear(sf::Color(0, 0, 139));  // Dark blue color
        window.clear();
        window.draw(backgroundSprite1);
        window.draw(backgroundSprite2);

        window.draw(shape);
        window.draw(leftBarSprite);
        window.draw(rightBarSprite);
        window.draw(resetSprite);
        window.draw(scoreText);
        window.draw(fpsText);
        window.display();
    }
    return 0;
}
void updateBackground(sf::Sprite& sprite1, sf::Sprite& sprite2, const sf::Texture& texture, float moveSpeed, float deltaTime) {
    sprite1.move(moveSpeed * deltaTime, 0);
    sprite2.move(moveSpeed * deltaTime, 0);

    if (sprite1.getPosition().x + texture.getSize().x < 0) {
        sprite1.setPosition(sf::Vector2f(sprite2.getPosition().x + texture.getSize().x, 0));
    }

    if (sprite2.getPosition().x + texture.getSize().x < 0) {
        sprite2.setPosition(sf::Vector2f(sprite1.getPosition().x + texture.getSize().x, 0));
    }
}

void adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window, sf::Text &scoreText) {
    b2Vec2 velocity = ballBody->GetLinearVelocity();
    b2Vec2 position = ballBody->GetPosition();

    float ballTop = position.y * SCALE - shape.getRadius();
    float ballBottom = position.y * SCALE + shape.getRadius();
    float ballLeft = position.x * SCALE - shape.getRadius();
    float ballRight = position.x * SCALE + shape.getRadius();
    if (ballLeft <= 0 || ballRight >= window.getSize().x) {
        velocity.x = -velocity.x;
    }
    if (ballTop <= 0 || ballBottom >= window.getSize().y) {
        velocity.y = -velocity.y;
    }
    float speed = velocity.Length();
    if (speed > MAX_BALL_SPEED)
    {
        velocity.x = (velocity.x / speed) * MAX_BALL_SPEED;
        velocity.y = (velocity.y / speed) * MAX_BALL_SPEED;
        ballBody->SetLinearVelocity(velocity);
    }
    position = ballBody->GetPosition();
    velocity = ballBody->GetLinearVelocity();

    if (position.x * SCALE <= shape.getRadius()) {
        velocity.x = -1.1*velocity.x;
        enemyScore++;
        updateScoreDisplay(scoreText);  // Update the score display
    }
    if (position.x * SCALE >= window.getSize().x - shape.getRadius()) {
        velocity.x = -1.1*velocity.x;
        playerScore++;
        updateScoreDisplay(scoreText);  // Update the score display
    }
    if (position.y * SCALE <= shape.getRadius() || position.y * SCALE >= window.getSize().y - shape.getRadius()) {
        velocity.y = -velocity.y;
        velocity.x = (velocity.x >= 0 ? 1 : -1) * std::max(std::abs(velocity.x), MIN_HORIZONTAL_SPEED);
    }
    if (std::abs(velocity.x) < MIN_HORIZONTAL_SPEED){
        velocity.x = (velocity.x > 0 ? 1 : -1) * MIN_HORIZONTAL_SPEED;
    }

    ballBody->SetLinearVelocity(velocity);
}

void updateScoreDisplay(sf::Text &scoreText) {
    scoreText.setString(std::to_string(playerScore) + " - " + std::to_string(enemyScore));
}

void updateSpritePosition(sf::Sprite &sprite, b2Body* body) {
    sprite.setPosition(std::round(SCALE * body->GetPosition().x), std::round(SCALE * body->GetPosition().y));
    sprite.setRotation(body->GetAngle() * 180/b2_pi);
}

bool loadTexture(sf::Texture &texture, const std::string &path, bool applyMask) {
    if (applyMask) {
        sf::Image image;
        if (!image.loadFromFile(path)) {
            std::cerr << "Failed to load image from " << path << "!" << std::endl;
            return false;
        }
        image.createMaskFromColor(sf::Color::White, 0);
        if (!texture.loadFromImage(image)) {
            std::cerr << "Failed to load texture from image!" << std::endl;
            return false;
        }
    } else {
        if (!texture.loadFromFile(path)) {
            std::cerr << "Failed to load texture from " << path << "!" << std::endl;
            return false;
        }
    }
    texture.setSmooth(false);
    return true;
}

void initBall(sf::CircleShape &shape, sf::Texture &ballTexture, sf::RenderWindow &window) {
    shape.setOrigin(12.f, 12.f);
    shape.setPosition(std::round(window.getSize().x / 2), std::round(window.getSize().y / 2));
    shape.setTexture(&ballTexture);
}

void initPaddle(sf::Sprite &sprite, sf::Texture &barTexture, float xPosition, sf::RenderWindow &window) {
    sprite.setTexture(barTexture);
    sprite.setScale(0.176, PADDLE_HEIGHT_SCALE);
    sprite.setOrigin(barTexture.getSize().x / 2, barTexture.getSize().y / 2);
    sprite.setPosition(std::round(xPosition), std::round(window.getSize().y / 2));
}

void resetBall(b2Body* ballBody, sf::RenderWindow &window, sf::Text &scoreText) {
    ballBody->SetTransform(b2Vec2(window.getSize().x / (2 * SCALE), window.getSize().y / (2 * SCALE)), 0);
    float angle = (rand() % 10 - 5) * (b2_pi / 180);
    ballBody->SetLinearVelocity(b2Vec2(10 * cos(angle) * ballSpeedMultiplier,
                                        1 * sin(angle) * ballSpeedMultiplier));
    playerScore = 0;
    enemyScore = 0;
    updateScoreDisplay(scoreText);
}

void initResetButton(sf::Sprite &sprite, sf::Texture &texture, sf::RenderWindow &window, sf::Text &scoreText) {
    sprite.setTexture(texture);
    sprite.setScale(0.12, 0.12); // Resize the button to 20% of its original size
    sprite.setOrigin(texture.getSize().x / 2, 0); // Center the sprite horizontally
    sprite.setPosition(std::round(window.getSize().x / 2), 5);  // Positioning the reset button very close to the center top
}

void handlePaddleMovement(sf::Sprite &sprite, sf::Texture &texture, b2Body* body, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey,  sf::RenderWindow &window) {
    sf::Vector2f paddleSize(texture.getSize().x * sprite.getScale().x, texture.getSize().y * sprite.getScale().y);

    float paddleTop = sprite.getPosition().y - paddleSize.y / 2;
    float paddleBottom = sprite.getPosition().y + paddleSize.y / 2;

    if (sf::Keyboard::isKeyPressed(upKey) && paddleTop > 0) {
        body->SetLinearVelocity(b2Vec2(0, -PADDLE_SPEED));
    }
    else if (sf::Keyboard::isKeyPressed(downKey) && paddleBottom < window.getSize().y) {
        body->SetLinearVelocity(b2Vec2(0, PADDLE_SPEED));
    }
    else {
        body->SetLinearVelocity(b2Vec2(0, 0));
    }
}

void updateSpritePosition(sf::CircleShape &shape, b2Body* body) {
    shape.setPosition(std::round(SCALE * body->GetPosition().x), std::round(SCALE * body->GetPosition().y));
    shape.setRotation(body->GetAngle() * 180/b2_pi);
}