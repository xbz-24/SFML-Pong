#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <cmath> // for sin and cos
#include <cstdlib> // for rand and srand
#include <ctime> // for time

const float SCALE = 30.f; 
const std::string path2ball = "Shiny_steel_ball.png";

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Pong", sf::Style::Default);
    
    sf::Texture ballTexture;
    if (!ballTexture.loadFromFile(path2ball)) {
        std::cerr << "Failed to load ball texture!" << std::endl;
        return -1;
    }

    sf::CircleShape shape(25.f);
    shape.setOrigin(25.f, 25.f);
    shape.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    shape.setTexture(&ballTexture);

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

        window.clear(sf::Color::Black);
        window.draw(shape);
        window.display();
    }
    return 0;
}
