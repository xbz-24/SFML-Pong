#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <cmath> 
#include <cstdlib> 
#include <ctime>

namespace Config{
    const float SCALE = 30.f;
    const float MIN_HORIZONTAL_SPEED = 50.5f;
    const float PADDLE_SPEED = 50.0f;
    const float MAX_BALL_SPEED = 60.0f;
    const float PADDLE_HEIGHT_SCALE = 0.25;
    const float ballSpeedMultiplier = 90.2f;
    const unsigned int SCREEN_WIDTH = 2560;
    const unsigned int SCREEN_HEIGHT= 1600;
    const unsigned int SCREEN_REFRESH_RATE = 60;
    const sf::Keyboard::Key LeftPaddleUpKey = sf::Keyboard::W;
    const sf::Keyboard::Key LeftPaddleDownKey = sf::Keyboard::S;
    const sf::Keyboard::Key RightPaddleUpKey = sf::Keyboard::O;
    const sf::Keyboard::Key RightPaddleDownKey = sf::Keyboard::K;
    const sf::Keyboard::Key ResetKey = sf::Keyboard::R;
    // Paddle positions, middle of the screen ususally
    const float leftXPosition = 50.0f;
    const float rightXPosition = SCREEN_WIDTH - 50.0f;
    const std::string path2ball = "../images/fireball32.png";
    const std::string path2bar = "../images/resized_tile_2.png";
    const std::string path2reset = "../images/refresh11.png";
    const float BallRadius = 30;
}

class Ball{
    public:
        Ball(sf::Texture &texture, float radius, sf::RenderWindow &window, b2World &world);
        void render(sf::RenderWindow &window);
        void reset(sf::RenderWindow &window);
        int adjustVelocity(sf::Sound &bounce, const sf::Sprite &leftPaddle, const sf::Sprite &rightPaddle, sf::RenderWindow &window, sf::Text &scoreText);
        void updateSpritePosition();
        unsigned int getPlayerScore();
        unsigned int getEnemyScore();
        b2Body* getBody();
        sf::CircleShape& getShape();

    private:
        sf::CircleShape shape;
        b2Body* ballBody;
        unsigned int playerScore;
        unsigned int enemyScore;
};
Ball::Ball(sf::Texture &texture, float radius , sf::RenderWindow &window, b2World &world){
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setTexture(&texture);
    shape.setPosition(std::round(window.getSize().x / 2), std::round(window.getSize().y / 2));

    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.bullet = true;
    ballBodyDef.position.Set(shape.getPosition().x / Config::SCALE, shape.getPosition().y / Config::SCALE);
    ballBody = world.CreateBody(&ballBodyDef);

    b2CircleShape ballCircle;
    ballCircle.m_radius = shape.getRadius() / Config::SCALE;

    b2FixtureDef ballFixtureDef;
    ballFixtureDef.shape = &ballCircle;
    ballFixtureDef.density = 1.f;
    ballFixtureDef.restitution = 1.0f;
    ballBody->CreateFixture(&ballFixtureDef);

    float angle = (45 + (rand() % 10 - 5)) * (b2_pi / 180);
    ballBody->SetLinearVelocity(b2Vec2(22 * cos(angle) * Config::ballSpeedMultiplier, 2 * sin(angle) * Config::ballSpeedMultiplier));
}
b2Body* Ball::getBody(){
    return ballBody;
}
sf::CircleShape& Ball::getShape(){
    return shape;
}
void Ball::updateSpritePosition(){
    shape.setPosition(std::round(Config::SCALE * ballBody->GetPosition().x), std::round(Config::SCALE * ballBody->GetPosition().y));
    shape.setRotation(ballBody->GetAngle() * 180 / b2_pi);
}

void Ball::render(sf::RenderWindow &window){
    window.draw(shape);
}

void Ball::reset(sf::RenderWindow &window){
    ballBody->SetTransform(b2Vec2(window.getSize().x / (2 * Config::SCALE), window.getSize().y / (2 * Config::SCALE)), 0);
    float angle = (rand() % 10 - 5) * (b2_pi / 180);
    ballBody->SetLinearVelocity(b2Vec2(10 * cos(angle) * Config::ballSpeedMultiplier, 1 * sin(angle) * Config::ballSpeedMultiplier));
}

int Ball::adjustVelocity(sf::Sound &bounce, const sf::Sprite &leftPaddle, const sf::Sprite &rightPaddle, sf::RenderWindow &window,  sf::Text &scoreText){
    b2Vec2 velocity = ballBody->GetLinearVelocity();
    b2Vec2 position = ballBody->GetPosition();

    float ballTop = position.y * Config::SCALE - shape.getRadius();
    float ballBottom = position.y * Config::SCALE + shape.getRadius();
    float ballLeft = position.x * Config::SCALE - shape.getRadius();
    float ballRight = position.x * Config::SCALE + shape.getRadius();
    if (ballLeft <= 0 || ballRight >= window.getSize().x){
        velocity.x = -velocity.x;
        bounce.play();
    }
    if (ballTop <= 0 || ballBottom >= window.getSize().y){
        velocity.y = -velocity.y;
        bounce.play();
    }
    float speed = velocity.Length();
    if (speed > Config::MAX_BALL_SPEED){
        velocity.x = (velocity.x / speed) * Config::MAX_BALL_SPEED;
        velocity.y = (velocity.y / speed) * Config::MAX_BALL_SPEED;
        ballBody->SetLinearVelocity(velocity);
    }
    position = ballBody->GetPosition();
    velocity = ballBody->GetLinearVelocity();

    if (position.x * Config::SCALE <= shape.getRadius()){
        velocity.x = -1.1*velocity.x;
        ballBody->SetLinearVelocity(velocity);
        return 2;
    }
    if (position.x * Config::SCALE >= window.getSize().x - shape.getRadius()){
        velocity.x = -1.1*velocity.x;
        ballBody->SetLinearVelocity(velocity);
        return 1;
    }
    if (position.y * Config::SCALE <= shape.getRadius() || position.y * Config::SCALE >= window.getSize().y - shape.getRadius()){
        velocity.y = -velocity.y;
        velocity.x = (velocity.x >= 0 ? 1 : -1) * std::max(std::abs(velocity.x), Config::MIN_HORIZONTAL_SPEED);
    }
    if (std::abs(velocity.x) < Config::MIN_HORIZONTAL_SPEED){
        velocity.x = (velocity.x > 0 ? 1 : -1) * Config::MIN_HORIZONTAL_SPEED;
    }
    ballBody->SetLinearVelocity(velocity);
    return 0;
}

unsigned int Ball::getPlayerScore(){
    return this->playerScore;
}

unsigned int Ball::getEnemyScore(){
    return this->enemyScore;
}

class Paddle{
    public:
        Paddle(sf::Texture &texture, float xPosition, sf::RenderWindow &window, b2World &world, sf::Keyboard::Key up, sf::Keyboard::Key down);
        void render(sf::RenderWindow& window);
        void handleMovement(sf::RenderWindow &window);
        sf::Sprite& getSprite();
        void updateSpritePosition();

    private:
        sf::Sprite sprite;
        sf::Keyboard::Key upKey;
        sf::Keyboard::Key downKey;
        b2Body* body;
};
Paddle::Paddle(sf::Texture &texture, float xPosition, sf::RenderWindow &window, b2World &world, sf::Keyboard::Key up, sf::Keyboard::Key down) : upKey(up), downKey(down){
    sprite.setTexture(texture);
    sprite.setScale(0.176, Config::PADDLE_HEIGHT_SCALE);
    sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
    sprite.setPosition(std::round(xPosition), std::round(window.getSize().y / 2));

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(xPosition / Config::SCALE, window.getSize().y / (2 * Config::SCALE));
    body = world.CreateBody(&bodyDef);
}
void Paddle::updateSpritePosition(){
    sprite.setPosition(std::round(Config::SCALE * body->GetPosition().x), std::round(Config::SCALE * body->GetPosition().y));
    sprite.setRotation(body->GetAngle() * 180 / b2_pi);
}
void Paddle::render(sf::RenderWindow &window){
   window.draw(sprite);
}

void Paddle::handleMovement(sf::RenderWindow &window){
    sf::Vector2f textureSize(static_cast<float>(sprite.getTexture()->getSize().x),
                             static_cast<float>(sprite.getTexture()->getSize().y));

    sf::Vector2f paddleSize(textureSize.x * sprite.getScale().x,
                            textureSize.y * sprite.getScale().y);


    float paddleTop = sprite.getPosition().y - paddleSize.y / 2;
    float paddleBottom = sprite.getPosition().y + paddleSize.y / 2;

    float windowHeight = static_cast<float>(window.getSize().y);

    if (sf::Keyboard::isKeyPressed(upKey) && paddleTop > 0)
    {
        body->SetLinearVelocity(b2Vec2(0, -Config::PADDLE_SPEED));
    }
    else if (sf::Keyboard::isKeyPressed(downKey) && paddleBottom < window.getSize().y)
    {
        body->SetLinearVelocity(b2Vec2(0, Config::PADDLE_SPEED));
    }
    else
    {
        body->SetLinearVelocity(b2Vec2(0, 0));
    }
}

sf::Sprite& Paddle::getSprite(){
    return sprite;
}

class ResetButton{
    public:
        ResetButton(sf::Texture &texture, sf::RenderWindow &window)
        {
            sprite.setTexture(texture);
            sprite.setScale(0.12, 0.12); // Resize to 20% of original size
            sprite.setOrigin(texture.getSize().x / 2, 0);
            sprite.setPosition(std::round(window.getSize().x / 2), 5);
        }

        void render(sf::RenderWindow &window)
        {
            window.draw(sprite);
        }

        bool isClicked(sf::Event &event)
        {
            return sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y);
        }
    private:
        sf::Sprite sprite;
};

class Game{
    public:
        Game();
        void run();
        void updateScoreDisplay(sf::Text &scoreText);
    private:
        sf::RenderWindow window;
        b2World world;
        Ball ball;
        Paddle leftPaddle, rightPaddle;
        ResetButton resetButton;
        sf::Clock clock;
        sf::Font font;
        sf::Text scoreText;
        sf::Sound bounceSound;
        sf::CircleShape ballSprite;
        sf::Sprite resetButtonSprite;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite1, backgroundSprite2;
        sf::Texture ballTexture, paddleTexture, resetTexture;
        sf::SoundBuffer bounceBuffer;
        float moveSpeed;
        unsigned int playerScore;
        unsigned int enemyScore;
        void processEvents();
        void update(float deltaTime);
        void render();
        void updateBackground(float deltaTime);
        bool checkResetButtonClick(const sf::Event &event);
        void adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window, sf::Text &scoreText, sf::Sound& bounce, const sf::Sprite& leftPaddle, const sf::Sprite& rightPaddle);
        void updateBackground(sf::Sprite& sprite1, sf::Sprite& sprite2, const sf::Texture& texture, float moveSpeed, float deltaTime);
};

Game::Game() : world(b2Vec2(0.0f, 0.0f)), ball(ballTexture, Config::BallRadius, window, world),
    leftPaddle(paddleTexture, Config::leftXPosition, window, world, Config::LeftPaddleUpKey, Config::LeftPaddleDownKey),
    rightPaddle(paddleTexture, Config::rightXPosition, window, world, Config::RightPaddleUpKey, Config::RightPaddleDownKey),
    resetButton(resetTexture, window){

    window.create(sf::VideoMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT), "Pong", sf::Style::Titlebar | sf::Style::Close);
    if(!backgroundTexture.loadFromFile("../images/nature_6/origbig_1600_height.png"))
    {
        std::cerr<<"Failed to load background texture!"<<std::endl;
    }
    backgroundSprite1.setTexture(backgroundTexture);
    backgroundSprite2.setTexture(backgroundTexture);

    backgroundSprite2.setPosition(sf::Vector2f(backgroundTexture.getSize().x, 0));

    window.setPosition(sf::Vector2i(0,0));
    window.setFramerateLimit(Config::SCREEN_REFRESH_RATE);

    if(!resetTexture.loadFromFile(Config::path2reset))
    {
        std::cerr << "Failed to load reset button texture" << std::endl;
    }
    resetButton = ResetButton(resetTexture, window);
    resetButtonSprite.setTexture(resetTexture);

    if (!ballTexture.loadFromFile(Config::path2ball)) {
        std::cerr << "Failed to load ball texture" << std::endl;
    }
    ball = Ball(ballTexture, Config::BallRadius, window, world);
    //ballSprite.setTexture(&ballTexture);

    if (!paddleTexture.loadFromFile(Config::path2bar)) {
        std::cerr << "Failed to load paddle texture" << std::endl;
    }

    leftPaddle = Paddle(paddleTexture, Config::leftXPosition, window, world, Config::LeftPaddleUpKey, Config::LeftPaddleDownKey);
    rightPaddle = Paddle(paddleTexture, Config::rightXPosition, window, world, Config::RightPaddleUpKey, Config::RightPaddleDownKey);

}

void Game::run(){
    while(window.isOpen()){
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents(){
   sf::Event event;
   while(window.pollEvent(event)){
       if(event.type == sf::Event::Closed){
           window.close();
       }
       else if(event.type == sf::Event::KeyPressed){
           if(event.key.code == Config::ResetKey){
                ball.reset(window);
                playerScore = 0;
                enemyScore = 0;
                updateScoreDisplay(scoreText);
           }
       }
   }
}

void Game::update(float deltaTime){
    leftPaddle.handleMovement(window);
    rightPaddle.handleMovement(window);

    updateBackground(backgroundSprite1,backgroundSprite2,backgroundTexture, -5.0f, deltaTime);

    world.Step(1/60.f,8,3);

    int scoreUpdate = ball.adjustVelocity(bounceSound, leftPaddle.getSprite(), rightPaddle.getSprite(), window, scoreText);

    if(scoreUpdate == 1){
        playerScore++;
        updateScoreDisplay(scoreText);
    }
    else if(scoreUpdate == 2){
        enemyScore++;
        updateScoreDisplay(scoreText);
    }

    //ball.checkCollisions(window, bounceSound);

    adjustBallVelocity(ball.getBody(), ball.getShape(), window, scoreText, bounceSound, leftPaddle.getSprite(), rightPaddle.getSprite());

    ball.updateSpritePosition();
    leftPaddle.updateSpritePosition();
    rightPaddle.updateSpritePosition();

    //void Game::adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window, sf::Text &scoreText, sf::Sound& bounce, const sf::Sprite& leftPaddle, const sf::Sprite& rightPaddle){
    //adjustBallVelocity(bounceSound, leftPaddle.getSprite(), rightPaddle.getSprite(), window, scoreText);

}

void Game::render(){
    window.clear();
    window.draw(backgroundSprite1);
    window.draw(backgroundSprite2);

    ball.render(window);
    leftPaddle.render(window);
    rightPaddle.render(window);
    resetButton.render(window);
    window.display();
}

void Game::updateScoreDisplay(sf::Text &scoreText){
       scoreText.setString(std::to_string(playerScore) + " - " + std::to_string(enemyScore));
}

void Game::updateBackground(sf::Sprite& sprite1, sf::Sprite& sprite2, const sf::Texture& texture, float moveSpeed, float deltaTime){
    backgroundSprite1.move(moveSpeed * deltaTime, 0);
    backgroundSprite2.move(moveSpeed * deltaTime, 0);

    if (backgroundSprite1.getPosition().x + backgroundTexture.getSize().x < 0){
        backgroundSprite1.setPosition(sf::Vector2f(backgroundSprite2.getPosition().x + backgroundTexture.getSize().x, 0));
    }

    if (backgroundSprite2.getPosition().x + backgroundTexture.getSize().x < 0){
        backgroundSprite2.setPosition(sf::Vector2f(backgroundSprite1.getPosition().x + backgroundTexture.getSize().x, 0));
    }
}

bool Game::checkResetButtonClick(const sf::Event &event){
    return resetButtonSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y);
}

void Game::adjustBallVelocity(b2Body* ballBody, sf::CircleShape &shape, sf::RenderWindow &window, sf::Text &scoreText, sf::Sound& bounce, const sf::Sprite& leftPaddle, const sf::Sprite& rightPaddle){
    b2Vec2 velocity = ballBody->GetLinearVelocity();
    b2Vec2 position = ballBody->GetPosition();

    float ballTop = position.y * Config::SCALE - shape.getRadius();
    float ballBottom = position.y * Config::SCALE + shape.getRadius();
    float ballLeft = position.x * Config::SCALE - shape.getRadius();
    float ballRight = position.x * Config::SCALE + shape.getRadius();
    if (ballLeft <= 0 || ballRight >= window.getSize().x){
        velocity.x = -velocity.x;
        bounce.play();
    }
    if (ballTop <= 0 || ballBottom >= window.getSize().y){
        velocity.y = -velocity.y;
        bounce.play();
    }
    float speed = velocity.Length();
    if (speed > Config::MAX_BALL_SPEED){
        velocity.x = (velocity.x / speed) * Config::MAX_BALL_SPEED;
        velocity.y = (velocity.y / speed) * Config::MAX_BALL_SPEED;
        ballBody->SetLinearVelocity(velocity);
    }
    position = ballBody->GetPosition();
    velocity = ballBody->GetLinearVelocity();

    if (position.x * Config::SCALE <= shape.getRadius()){
        velocity.x = -1.1*velocity.x;
        enemyScore++;
        updateScoreDisplay(scoreText);
    }
    if (position.x * Config::SCALE >= window.getSize().x - shape.getRadius()){
        velocity.x = -1.1*velocity.x;
        playerScore++;
        updateScoreDisplay(scoreText);
    }
    if (position.y * Config::SCALE <= shape.getRadius() || position.y * Config::SCALE >= window.getSize().y - shape.getRadius()){
        velocity.y = -velocity.y;
        velocity.x = (velocity.x >= 0 ? 1 : -1) * std::max(std::abs(velocity.x), Config::MIN_HORIZONTAL_SPEED);
    }
    if (std::abs(velocity.x) < Config::MIN_HORIZONTAL_SPEED){
        velocity.x = (velocity.x > 0 ? 1 : -1) * Config::MIN_HORIZONTAL_SPEED;
    }
    ballBody->SetLinearVelocity(velocity);
}
int main(){
    Game game;
    game.run();
}
