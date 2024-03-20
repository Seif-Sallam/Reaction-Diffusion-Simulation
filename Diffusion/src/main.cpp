#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <fmt/core.h>

#define LOCK_GUARD(X) const std::lock_guard<std::mutex> lk_##X(X);

sf::Image mainImage;
sf::Image nextImage;

int WIDTH{};
int HEIGHT{};

double dA = 1.0f;
double dB = 0.5f;
double feed = 0.055f;
double kill = 0.062f;

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

std::mutex mtx;
std::vector<bool> threadFinished;

bool isWorking = true;

inline void
doWork(int idx, int startX, int startY, int endX, int endY)
{
    if (startX == 0)
        startX += 1;
    if (endX == WIDTH)
        endX = WIDTH - 1;

    if (startY == 0)
        startY += 1;
    if (endY == HEIGHT)
        endY = HEIGHT - 1;

    while (true)
    {
        {
            LOCK_GUARD(mtx);
            if (!isWorking)
                break;

            if (threadFinished[idx])
                continue;
            threadFinished[idx] = false;
        }

        // Processing
        for (int i = startX; i < endX; i++)
        {
            for (int j = startY; j < endY; j++)
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
                                ((kill + feed) * b));

                if (next[i][j].a > 1)
                    next[i][j].a = 1;
                if (next[i][j].b > 1)
                    next[i][j].b = 1;

                if (next[i][j].a < 0)
                    next[i][j].a = 0;
                if (next[i][j].b < 0)
                    next[i][j].b = 0;

                // set the image pixel color
                double a2 = (double)(next[i][j].a);
                double b2 = (double)(next[i][j].b);

                int c = (int)floor((a2 - b2) * 255);
                if (c > 255)
                    c = 255;
                else if (c < 0)
                    c = 0;

                setPixel(i, j, c, c, c, 255);
            }
        }

        {
            LOCK_GUARD(mtx);
            threadFinished[idx] = true;
        }
    }
}

#define ARG_OPTION_DEF(X, VAL, D) fmt::print("\t- {}: {}, Default: {}\n", X, VAL, D)

void
helpMessage()
{
    fmt::print("Usage:\n");
    fmt::print("Diffusion.exe [*Options*]\n");
    fmt::print("* Options:\n");
    ARG_OPTION_DEF("width", "Number", 200);
    ARG_OPTION_DEF("height", "Number", 200);
    ARG_OPTION_DEF("popX", "Number", "Width / 2");
    ARG_OPTION_DEF("popY", "Number", "Height / 2");
    ARG_OPTION_DEF("length", "Number", 25);
    ARG_OPTION_DEF("cores", "Number", "Maximum number of cores in your CPU");
    ARG_OPTION_DEF("debug", "0/1", 0);
    ARG_OPTION_DEF("dA", "Decimal", 1.0f);
    ARG_OPTION_DEF("dB", "Decimal", 0.5f);
    ARG_OPTION_DEF("feed", "Decimal", 0.055f);
    ARG_OPTION_DEF("kill", "Decimal", 0.062f);
}

bool
stepAndAssert(int& i, const int& steps, const int& argc)
{
    i += steps;
    if (argc <= i) {
        helpMessage();
        return false;
    }
    return true;
}
#define CHECK_ARGV(RES, I) if (std::string(argv[I]) == std::string("--") + #RES) {\
    if (!stepAndAssert(I, 1, argc))\
        return 1;\
    RES = std::stoi(argv[I]);\
}

#define CHECK_ARGV_D(RES, I) if (std::string(argv[I]) == std::string("--") + #RES) {\
    if (!stepAndAssert(I, 1, argc))\
        return 1;\
    RES = std::stod(argv[I]);\
}

int main(int argc, const char* argv[])
{
    int width{200};
    int height{200};
    int popX{width / 2}, popY{height / 2}, length{25};
    bool debug = false;
    int cores = std::thread::hardware_concurrency();
    for (auto i = 1; i < argc; ++i)
    {
        CHECK_ARGV(width, i)
        else CHECK_ARGV(height, i)
        else CHECK_ARGV(popX, i)
        else CHECK_ARGV(popY, i)
        else CHECK_ARGV(length, i)
        else CHECK_ARGV(cores, i)
        else CHECK_ARGV(debug, i)
        else CHECK_ARGV_D(dA, i)
        else CHECK_ARGV_D(dB, i)
        else CHECK_ARGV_D(feed, i)
        else CHECK_ARGV_D(kill, i)
        else if (std::string(argv[i]) == "--help") {
            helpMessage();
            return 0;
        }

    }
    WIDTH = width;
    HEIGHT = height;

    if (popY > HEIGHT || popX > WIDTH || popX < 0 || popY < 0)
    {
        fmt::print("Invalid parameters");
        return 1;
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "App", sf::Style::Default, settings);

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

    for (int i = popX - length; i < popX + length; ++i) {
        for (int j = popY - length; j < popY + length; ++j) {
            grid[i][j].a = 0;
            grid[i][j].b = 1;
            next[i][j].a = grid[i][j].a;
            next[i][j].b = grid[i][j].b;
         }
    }

    sf::RectangleShape rect(vec(window.getSize()));
    rect.setTexture(&fullTexture);

    sf::Clock clk;

    float d = 1000;

    std::vector<std::thread> allThreads;

    int rowCount = int(sqrt(cores));
    int colCount = int(cores / rowCount);

    int blockSizeX = WIDTH / rowCount;
    int blockSizeY = HEIGHT / colCount;

    fmt::print("Num of cores: {}\n", cores);
    fmt::print("Width: {}, Height: {}\n", WIDTH, HEIGHT);
    fmt::print("PopX: {}, PopY: {}, Length\n", popX, popY, length);
    fmt::print("Row Count: {}, Col Count: {}\n", rowCount, colCount);
    fmt::print("X size: {}, Y Size: {}\n", blockSizeX, blockSizeY);

    for (int i = 0; i < rowCount; ++i)
    {
        for (int j = 0; j < colCount; ++j)
        {
            int startX{}, startY{}, endX{}, endY{};
            startX = blockSizeX * i;
            endX = startX + blockSizeX;
            startY = blockSizeY * j;
            endY = startY + blockSizeY;
            int idx = i * colCount + j;
            if (debug)
                fmt::print("idx: ({})\n\t- X: ({}, {}), Y: ({}, {})\n", idx, startX, endX, startY, endY);
            threadFinished.push_back(true);
            allThreads.push_back(std::thread(doWork, idx, startX, startY, endX, endY));
        }
    }

    int maxUpdates = 1;
    int times = 0;

    while (window.isOpen())
    {
        auto dt = clk.restart();
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    isWorking = false;
                    window.close();
                break;
            }
        }

        {
            bool done = true;
            LOCK_GUARD(mtx);

            for (size_t i = 0; i < threadFinished.size(); ++i)
            {
                auto& finished = threadFinished[i];
                done = done & finished;
            }

            if (done)
            {
                for (auto& finished : threadFinished)
                    finished = false;

                grid.swap(next);

                times += 1;
                times = times % (maxUpdates + 1);
            }

            if (done && times == maxUpdates)
            {
                mainImage.copy(nextImage, 0 ,0);
                fullTexture.update(nextImage);
                if (debug)
                    fmt::print("\rtime: {:.10f} ms", dt.asSeconds() * 1000.0f);
            }
        }

        window.clear();
        window.draw(rect);
        window.display();
    }

    for (auto& thread : allThreads)
        thread.join();
    return 0;
}