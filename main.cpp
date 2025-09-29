#include <SFML/Graphics.hpp>

// Controls
const sf::Keyboard::Key controls[4] = {
    sf::Keyboard::A,   // Player1 UP
    sf::Keyboard::Z,   // Player1 Down
    sf::Keyboard::Up,  // Player2 UP
    sf::Keyboard::Down // Player2 Down
};

// Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; // 60 fps

// Ball movement parameters
sf::Vector2f ball_velocity;
bool is_player_serving = true;
const float initial_velocity_x = 100.f; // horizontal velocity
const float initial_velocity_y = 60.f;  // vertical velocity
const float velocity_multiplier = 1.1f; // ball speeds up on bounce

// Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];

// Score system
sf::Font font;
sf::Text text;
int scoreLeft = 0;
int scoreRight = 0;

// Reset ball and paddles
void reset() {
    // reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);
    // reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
    // reset ball velocity depending on who serves
    ball_velocity = { (is_player_serving ? initial_velocity_x : -initial_velocity_x), initial_velocity_y };
    // swap serving player
    is_player_serving = !is_player_serving;

    // Update Score Text
    // set the string to display
    text.setString("Hello world");
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}

void init() {
    // Set size and origin of paddles
    for (sf::RectangleShape &p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); // Should be half the ball width and height

    // Load font-face from res dir
    if (!font.loadFromFile("res/fonts/RobotoMono-Regular.ttf")) {
        // error handling if font not found
    }
    // Setup score text
    text.setFont(font);
    text.setCharacterSize(25);
    text.setFillColor(sf::Color::White);

    // First reset of the game (also initializes text)
    reset();
}

void update(float dt, sf::RenderWindow &window) {
    // handle paddle movement (Player 1 only for now)
    float direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[0])) {
        direction--;
    }
    if (sf::Keyboard::isKeyPressed(controls[1])) {
        direction++;
    }
    paddles[0].move(sf::Vector2f(0.f, direction * paddleSpeed * dt));

      float direction1 = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[2])) {
        direction1--;
    }
    if (sf::Keyboard::isKeyPressed(controls[3])) {
        direction1++;
    }
    paddles[1].move(sf::Vector2f(0.f, direction1 * paddleSpeed * dt));
    // clamp paddles inside screen
    for (sf::RectangleShape &p : paddles) {
        const float py = p.getPosition().y;
        const float halfHeight = paddleSize.y * 0.5f;

        if (py - halfHeight < 0) { // top of screen
            p.setPosition(p.getPosition().x, halfHeight);
        }
        else if (py + halfHeight > gameHeight) { // bottom of screen
            p.setPosition(p.getPosition().x, gameHeight - halfHeight);
        }
    }

    // move ball
    ball.move(ball_velocity * dt);

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;

    // bottom wall
    if (by > gameHeight) {
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, -10.f));
    }
    // top wall
    else if (by < 0) {
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, 10.f));
    }
    // right wall (score for left player)
    else if (bx > gameWidth) {
        scoreLeft++;
        reset();
    }
    // left wall (score for right player)
    else if (bx < 0) {
        scoreRight++;
        reset();
    }
    // left paddle collision
    else if (
        bx < paddleSize.x + paddleOffsetWall &&
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5f) &&
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5f))
    {
        ball_velocity.x *= -velocity_multiplier;
        ball.move(sf::Vector2f(10.f, 0.f));
    }
    // right paddle collision
    else if (
        bx > gameWidth - paddleSize.x - paddleOffsetWall &&
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5f) &&
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5f))
    {
        ball_velocity.x *= -velocity_multiplier;
        ball.move(sf::Vector2f(-10.f, 0.f));
    }
}

void render(sf::RenderWindow &window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(text); // draw score
}

int main () {
    // create the window
    sf::RenderWindow window(sf::VideoMode({gameWidth, gameHeight}), "PONG");
    // initialise and load
    init();
    while (window.isOpen()) {
        // Calculate dt
        static sf::Clock clock;
        const float dt = clock.restart().asSeconds();

        // process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        update(dt, window);
        render(window);
        window.draw(text);
        window.display();
    }
}
