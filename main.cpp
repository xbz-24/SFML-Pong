#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include <cmath> 
#include <cstdlib> 
#include <ctime> 

const float SCALE = 30.f; 
const std::string path2ball = "bola.png";
const std::string path2bar = "paleta.png"; 
const std::string path2reset = "refresh11.png"; 

int playerScore = 0;
int enemyScore = 0;
sf::Font font;
sf::Text scoreText;

bool loadTexture(sf::Texture &texture, const std::string &path, bool applyMask = false);
void initBall(sf::CircleShape &shape, sf::Texture &ballTexture, sf::RenderWindow &window);
void initPaddle(sf::Sprite &sprite, sf::Texture &barTexture, float xPosition, sf::RenderWindow &window);
void resetBall(b2Body* ballBody, sf::RenderWindow &window);
void initResetButton(sf::Sprite &sprite, sf::Texture &texture, sf::RenderWindow &window);
void handlePaddleMovement(sf::Sprite &sprite, sf::Texture &texture, b2Body* body, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, sf::RenderWindow &window);
void adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window);
void updateSpritePosition(sf::Sprite &sprite, b2Body* body);
void updateSpritePosition(sf::CircleShape &shape, b2Body* body);
void updateScoreDisplay();

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); 

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Pong", sf::Style::Default, settings);
    
    if (!font.loadFromFile("/Users/xbz/Library/Fonts/HackNerdFont-Regular.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(window.getSize().x - 100, 10);
    updateScoreDisplay();

   
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
    initResetButton(resetSprite, resetTexture, window);

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
    leftBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, (barTexture.getSize().y * 0.3584 / 2) / SCALE);
    leftBarBody->CreateFixture(&leftBarShape, 0.0f);

    b2BodyDef rightBarBodyDef;
    rightBarBodyDef.type = b2_kinematicBody;
    rightBarBodyDef.position.Set(rightBarSprite.getPosition().x / SCALE, rightBarSprite.getPosition().y / SCALE);
    b2Body* rightBarBody = world.CreateBody(&rightBarBodyDef);

    b2PolygonShape rightBarShape;
    rightBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, (barTexture.getSize().y * 0.3584 / 2) / SCALE);
    rightBarBody->CreateFixture(&rightBarShape, 0.0f);

    float angle = (45 + (rand() % 10 - 5)) * (b2_pi / 180);
    ballBody->SetLinearVelocity(b2Vec2(2 * cos(angle), 2 * sin(angle)));


  while (window.isOpen()) {
  sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } 
            else if (event.type == sf::Event::MouseButtonPressed && resetSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                resetBall(ballBody, window);
                playerScore = 0;
                enemyScore = 0;
                updateScoreDisplay();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetBall(ballBody, window);
                playerScore = 0;
                enemyScore = 0;
                updateScoreDisplay();
            }
        }

        handlePaddleMovement(leftBarSprite, barTexture, leftBarBody, sf::Keyboard::W, sf::Keyboard::S, window);
        handlePaddleMovement(rightBarSprite, barTexture, rightBarBody, sf::Keyboard::Up, sf::Keyboard::Down, window);

        world.Step(1/60.f, 12, 8);

        adjustBallVelocity(ballBody, shape, window);

        updateSpritePosition(shape, ballBody);
        updateSpritePosition(leftBarSprite, leftBarBody);
        updateSpritePosition(rightBarSprite, rightBarBody);

        window.clear(sf::Color(0, 0, 139));  // Dark blue color
        window.draw(shape);
        window.draw(leftBarSprite);
        window.draw(rightBarSprite);
        window.draw(resetSprite);
        window.draw(scoreText);
        window.display();
    }
    return 0;
}

void adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window) {
    b2Vec2 position = ballBody->GetPosition();
    b2Vec2 velocity = ballBody->GetLinearVelocity();

    if (position.x * SCALE <= shape.getRadius()) {
        velocity.x = -velocity.x;
        enemyScore++;
        updateScoreDisplay();  // Update the score display
    }
    if (position.x * SCALE >= window.getSize().x - shape.getRadius()) {
        velocity.x = -velocity.x;
        playerScore++;
        updateScoreDisplay();  // Update the score display
    }
    if (position.y * SCALE <= shape.getRadius() || position.y * SCALE >= window.getSize().y - shape.getRadius()) {
        velocity.y = -velocity.y;
    }

    ballBody->SetLinearVelocity(velocity);
}

void updateScoreDisplay() {
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
    sprite.setScale(0.176, 0.3584);
    sprite.setOrigin(barTexture.getSize().x / 2, barTexture.getSize().y / 2);
    sprite.setPosition(std::round(xPosition), std::round(window.getSize().y / 2));
}

void resetBall(b2Body* ballBody, sf::RenderWindow &window) {
    ballBody->SetTransform(b2Vec2(window.getSize().x / (2 * SCALE), window.getSize().y / (2 * SCALE)), 0);
    float angle = (45 + (rand() % 10 - 5)) * (b2_pi / 180);
    ballBody->SetLinearVelocity(b2Vec2(2 * cos(angle), 2 * sin(angle)));
    playerScore = 0;
    enemyScore = 0;
    updateScoreDisplay();
}

void initResetButton(sf::Sprite &sprite, sf::Texture &texture, sf::RenderWindow &window) {
    sprite.setTexture(texture);
    sprite.setScale(0.12, 0.12); // Resize the button to 20% of its original size
    sprite.setOrigin(texture.getSize().x / 2, 0); // Center the sprite horizontally
    sprite.setPosition(std::round(window.getSize().x / 2), 5);  // Positioning the reset button very close to the center top
}

void handlePaddleMovement(sf::Sprite &sprite, sf::Texture &texture, b2Body* body, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, sf::RenderWindow &window) {
    if (sf::Keyboard::isKeyPressed(upKey) && sprite.getPosition().y - (texture.getSize().y * 0.3584 / 2) > 0) {
        body->SetLinearVelocity(b2Vec2(0, -2.5));  // Move up
    }
    else if (sf::Keyboard::isKeyPressed(downKey) && sprite.getPosition().y + (texture.getSize().y * 0.3584 / 2) < window.getSize().y) {
        body->SetLinearVelocity(b2Vec2(0, 2.5));  // Move down
    }
    else {
        body->SetLinearVelocity(b2Vec2(0, 0));  // Stop moving
    }
}

void updateSpritePosition(sf::CircleShape &shape, b2Body* body) {
    shape.setPosition(std::round(SCALE * body->GetPosition().x), std::round(SCALE * body->GetPosition().y));
    shape.setRotation(body->GetAngle() * 180/b2_pi);
}
