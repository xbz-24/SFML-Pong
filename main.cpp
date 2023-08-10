#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <cmath> // for sin and cos
#include <cstdlib> // for rand and srand
#include <ctime> // for time

const float SCALE = 30.f; 
const std::string path2ball = "Shiny_steel_ball.png";
const std::string path2bar = "pallet5.png";  // Path to the paddle texture

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Pong", sf::Style::Default);
    
    // Load ball texture
    sf::Texture ballTexture;
    if (!ballTexture.loadFromFile(path2ball)) {
        std::cerr << "Failed to load ball texture!" << std::endl;
        return -1;
    }

    sf::CircleShape shape(12.f);  // Reduced the ball size by 40% and then by an additional 20%
    shape.setOrigin(12.f, 12.f);
    shape.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    shape.setTexture(&ballTexture);

    // Load paddle texture
    sf::Texture barTexture;
    if (!barTexture.loadFromFile(path2bar)) {
        std::cerr << "Failed to load paddle texture!" << std::endl;
        return -1;
    }

    // Left paddle setup
    sf::Sprite leftBarSprite;
    leftBarSprite.setTexture(barTexture);
    leftBarSprite.setScale(0.176, 0.448);  // Adjusted the size of the paddle by an additional 20%
    leftBarSprite.setOrigin(barTexture.getSize().x / 2, barTexture.getSize().y / 2);
    leftBarSprite.setPosition(24, window.getSize().y / 2);

    // Right paddle setup
    sf::Sprite rightBarSprite;
    rightBarSprite.setTexture(barTexture);
    rightBarSprite.setScale(0.176, 0.448);  // Adjusted the size of the paddle by an additional 20%
    rightBarSprite.setOrigin(barTexture.getSize().x / 2, barTexture.getSize().y / 2);
    rightBarSprite.setPosition(window.getSize().x - 24, window.getSize().y / 2);

    b2Vec2 gravity(0.f, 0.f); 
    b2World world(gravity);

    // Ball physics
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
    ballFixtureDef.restitution = 1.0f; // Perfect bounce
    ballBody->CreateFixture(&ballFixtureDef);

    // Left paddle physics
    b2BodyDef leftBarBodyDef;
    leftBarBodyDef.type = b2_kinematicBody;  // The paddle will be moved manually
    leftBarBodyDef.position.Set(leftBarSprite.getPosition().x / SCALE, leftBarSprite.getPosition().y / SCALE);
    b2Body* leftBarBody = world.CreateBody(&leftBarBodyDef);

    b2PolygonShape leftBarShape;
    leftBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, (barTexture.getSize().y * 0.448 / 2) / SCALE);
    leftBarBody->CreateFixture(&leftBarShape, 0.0f);

    // Right paddle physics
    b2BodyDef rightBarBodyDef;
    rightBarBodyDef.type = b2_kinematicBody;  // The paddle will be moved manually
    rightBarBodyDef.position.Set(rightBarSprite.getPosition().x / SCALE, rightBarSprite.getPosition().y / SCALE);
    b2Body* rightBarBody = world.CreateBody(&rightBarBodyDef);

    b2PolygonShape rightBarShape;
    rightBarShape.SetAsBox((barTexture.getSize().x * 0.176 / 2) / SCALE, (barTexture.getSize().y * 0.448 / 2) / SCALE);
    rightBarBody->CreateFixture(&rightBarShape, 0.0f);

    // Introduce a slight randomness to the ball's initial angle
    float angle = (45 + (rand() % 10 - 5)) * (b2_pi / 180); // Angle in radians
    ballBody->SetLinearVelocity(b2Vec2(2 * cos(angle), 2 * sin(angle)));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } 
            else if (event.type == sf::Event::Resized) {
                // Handle window resizing if needed
            }
        }

        // Handle user input to move the left paddle
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && leftBarSprite.getPosition().y - (barTexture.getSize().x * 0.448 / 2) > 0) {
            leftBarBody->SetLinearVelocity(b2Vec2(0, -2.5));  // Move up at reduced speed
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && leftBarSprite.getPosition().y + (barTexture.getSize().x * 0.448 / 2) < window.getSize().y) {
            leftBarBody->SetLinearVelocity(b2Vec2(0, 2.5));  // Move down at reduced speed
        }
        else {
            leftBarBody->SetLinearVelocity(b2Vec2(0, 0));  // Stop moving
        }

        // Handle user input to move the right paddle
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && rightBarSprite.getPosition().y - (barTexture.getSize().x * 0.448 / 2) > 0) {
            rightBarBody->SetLinearVelocity(b2Vec2(0, -2.5));  // Move up at reduced speed
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && rightBarSprite.getPosition().y + (barTexture.getSize().x * 0.448 / 2) < window.getSize().y) {
            rightBarBody->SetLinearVelocity(b2Vec2(0, 2.5));  // Move down at reduced speed
        }
        else {
            rightBarBody->SetLinearVelocity(b2Vec2(0, 0));  // Stop moving
        }

        world.Step(1/60.f, 12, 8);

        // Adjust the ball's velocity based on its position
        b2Vec2 position = ballBody->GetPosition();
        b2Vec2 velocity = ballBody->GetLinearVelocity();

        if (position.x * SCALE <= shape.getRadius() || position.x * SCALE >= window.getSize().x - shape.getRadius()) {
            velocity.x = -velocity.x;
        }
        if (position.y * SCALE <= shape.getRadius() || position.y * SCALE >= window.getSize().y - shape.getRadius()) {
            velocity.y = -velocity.y;
        }

        ballBody->SetLinearVelocity(velocity);

        shape.setPosition(SCALE * ballBody->GetPosition().x, SCALE * ballBody->GetPosition().y);
        shape.setRotation(ballBody->GetAngle() * 180/b2_pi);

        leftBarSprite.setPosition(SCALE * leftBarBody->GetPosition().x, SCALE * leftBarBody->GetPosition().y);
        rightBarSprite.setPosition(SCALE * rightBarBody->GetPosition().x, SCALE * rightBarBody->GetPosition().y);

        window.clear(sf::Color::Black);  // Background color set to black
        window.draw(shape);
        window.draw(leftBarSprite);  // Draw the left paddle
        window.draw(rightBarSprite);  // Draw the right paddle
        window.display();
    }
    return 0;
}
