#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

sf::Image mainImage;
sf::Image nextImage;

bool imageReady = false;

int WIDTH{200};
int HEIGHT{200};

double dA = 1.0f;
double dB = 0.5f;
double feed = 0.055f;
double k = 0.062f;

void
setPixel(const int& x, const int& y, const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b, const sf::Uint8& a = 255) {
    nextImage.setPixel(x, y, sf::Color{r, g, b, a});
}

void
setPixel(const int& x, const int& y, const sf::Color& clr) {
    setPixel(x, y, clr.r, clr.g, clr.b, clr.a);
}

sf::Color getPixel(int x, int y) { return nextImage.getPixel(x , y); }

sf::Vector2f vec(sf::Vector2u v) { return {(float)v.x, (float)v.y}; }
sf::Vector2f vec(sf::Vector2i v) { return {(float)v.x, (float)v.y}; }

struct Cell {
    double a, b;
};

std::vector<std::vector<Cell>> grid;
std::vector<std::vector<Cell>> next;

double laplaceA(int x, int y)
{
    double sum = 0;
    sum += grid[x][y].a * -1.0;
    sum += grid[x - 1][y].a * 0.2;
    sum += grid[x + 1][y].a * 0.2;
    sum += grid[x][y + 1].a * 0.2;
    sum += grid[x][y - 1].a * 0.2;
    sum += grid[x - 1][y - 1].a * 0.05;
    sum += grid[x + 1][y - 1].a * 0.05;
    sum += grid[x + 1][y + 1].a * 0.05;
    sum += grid[x - 1][y + 1].a * 0.05;
    return sum;
}

double laplaceB(int x, int y)
{
    double sum = 0;
    sum += grid[x][y].b * -1.0;
    sum += grid[x - 1][y].b * 0.2;
    sum += grid[x + 1][y].b * 0.2;
    sum += grid[x][y + 1].b * 0.2;
    sum += grid[x][y - 1].b * 0.2;
    sum += grid[x - 1][y - 1].b * 0.05;
    sum += grid[x + 1][y - 1].b * 0.05;
    sum += grid[x + 1][y + 1].b * 0.05;
    sum += grid[x - 1][y + 1].b * 0.05;
    return sum;
}

int main(int argc, const char* argv[])
{

    if (argc > 2){
        WIDTH = std::stoi(argv[1]);
        HEIGHT = std::stoi(argv[2]);
    }
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "App", sf::Style::Default, settings);
    // window.setFramerateLimit(20);

    mainImage.create(WIDTH, HEIGHT);
    nextImage.create(WIDTH, HEIGHT);

    sf::Texture fullTexture;
    fullTexture.loadFromImage(mainImage);

    grid.resize(WIDTH);
    next.resize(WIDTH);

    for (int i = 0; i< WIDTH; i++)
    {
        grid[i].resize(HEIGHT);
        next[i].resize(HEIGHT);
        for (int j = 0; j < HEIGHT; j++)
        {
            grid[i][j].a = 1;
            grid[i][j].b = 0;
            next[i][j].a = grid[i][j].a;
            next[i][j].b = grid[i][j].b;
        }
    }

    for (int i = WIDTH/2 - 25; i < WIDTH/2 + 25; ++i) {
        for (int j = HEIGHT/2 - 25; j < HEIGHT/2 + 25; ++j) {
            grid[i][j].a = 0;
            grid[i][j].b = 1;
            next[i][j].a = grid[i][j].a;
            next[i][j].b = grid[i][j].b;
         }
    }

    sf::RectangleShape rect(vec(window.getSize()));
    rect.setTexture(&fullTexture);

    sf::Clock clk;

    bool paused = false;
    int timeStep = 10;
    float d = 1000;
    while (window.isOpen())
    {
        auto dt = clk.restart();
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Key::P)
                    paused = !paused;
                break;
                case sf::Event::Closed:
                    window.close();
                break;
            }
        }
        if (!paused)
        {
            // copy image when ready
            if (imageReady)
            {
                imageReady = false;
                mainImage.copy(nextImage, 0 ,0);
            }

            for (int x = 0; x < timeStep; ++x) {
                for (int i = 1; i< WIDTH - 1; i++)
                {
                    for (int j = 1; j < HEIGHT - 1; j++)
                    {
                        double& a = grid[i][j].a;
                        double& b = grid[i][j].b;
                        next[i][j].a =  a +
                                        ((dA * laplaceA(i, j)) -
                                        (a * b * b) +
                                        (feed * (1 - a)));
                        next[i][j].b =  b +
                                        ((dB * laplaceB(i, j)) +
                                        (a * b * b) -
                                        ((k + feed) * b));

                        if (next[i][j].a > 1)
                            next[i][j].a = 1;
                        if (next[i][j].b > 1)
                            next[i][j].b = 1;

                        if (next[i][j].a < 0)
                            next[i][j].a = 0;
                        if (next[i][j].b < 0)
                            next[i][j].b = 0;
                    }
                }
                grid.swap(next);
            }


            for (int i = 0; i< WIDTH; i++)
            {
                for (int j = 0; j < HEIGHT; j++)
                {
                    double a = (double)(grid[i][j].a);
                    double b = (double)(grid[i][j].b);

                    int c = (int)floor((a - b) * 255);
                    if (c > 255)
                        c = 255;
                    else if (c < 0)
                        c = 0;

                    setPixel(i, j, c, c, c, 255);
                }
            }
            imageReady = true;

            fullTexture.update(nextImage);
        }


        window.clear();
        window.draw(rect);
        window.display();
    }
    return 0;
}