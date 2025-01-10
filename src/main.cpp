#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <structures.h>
#include <constants.h>
#include <costs.h>
#include <print.h>
#include <SFML/Graphics.hpp>
#include <optional>
#include <rectshape.h>
#include <chrono>
#include <thread>
#include <future>

using namespace std;

void draw(
    sf::Vector2u& size,
    sf::RenderWindow& window,
    float& containerSize,
    float& blockSize,
    block blocks[space_size][space_size]
) {
    window.clear(sf::Color(225, 225, 225));

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j].shape.setPosition(
                i * 50 + i * 10 + size.x / 2.0 - containerSize / 2.0,
                j * 50 + j * 10 + size.y / 2.0 - containerSize / 2.0
            );
            window.draw(blocks[i][j].shape);
            stringstream ss;
            ss << fixed << setprecision(0) << blocks[i][j].info.cost;
            blocks[i][j].text.setString(ss.str());
            sf::Vector2f textSize = blocks[i][j].text.getGlobalBounds().getSize();
            blocks[i][j].text.setPosition(
                i * 50 + i * 10 + size.x / 2.0 - containerSize / 2.0 + blockSize / 2.0 - textSize.x / 2.0 - 2,
                j * 50 + j * 10 + size.y / 2.0 - containerSize / 2.0 + blockSize / 2.0 - textSize.y / 2.0 - 5
            );
            window.draw(blocks[i][j].text);
        }
    }
    window.display();
}


void dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
) {
    stack<point> stack;
    stack.push(start);
    blocks[start.x][start.y].info.cost = 0;
    blocks[start.x][start.y].info.step = 0;
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            point next = { node.x + dirs[i].x, node.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (next.x == target.x && next.y == target.y) {
                cout << "Found target\n" << blocks[node.x][node.y].info.cost + 1 << "\n";
            }
            float costValue = cost(i, blocks[node.x][node.y].info.step + 1);
            if (blocks[next.x][next.y].info.cost == -1 ||
                blocks[node.x][node.y].info.cost + costValue < blocks[next.x][next.y].info.cost) {
                stack.push(next);

                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;

                for (int i = 0; i < space_size; i++) {
                    for (int j = 0; j < space_size; j++) {
                        if (i == next.x && j == next.y) {
                            blocks[i][j].shape.setFillColor(sf::Color(255, 0, 0));
                        } else if (blocks[i][j].info.cost != -1) {
                            blocks[i][j].shape.setFillColor(sf::Color(100, 200, 100));
                        } else {
                            blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
                        }
                    }
                }
                shouldDraw = true;

                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }
    }

    calculate_path(start, target, blocks);
}

int main() {
    cout << "Hello, World!\n";

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window", sf::Style::Default, settings);
    auto font = sf::Font();
    if (!font.loadFromFile("./assets/Roboto-Regular.ttf")) {
        cout << "Error loading font\n";
    } else {
        cout << "Font loaded\n";
    }

    sf::Vector2u size = window.getSize();
    float blockSize = 50;
    float pad = 10;
    float containerSize = (pad * space_size + blockSize * space_size);

    bool shouldDraw = true;

    block blocks[space_size][space_size];

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j] = { sf::RoundedRectangleShape(sf::Vector2f(50, 50), 10, 20), sf::Text(), {-1,-1} };
            blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
            blocks[i][j].text.setFont(font);
            blocks[i][j].text.setFillColor(sf::Color(255, 255, 255));
        }
    }

    thread t1(dfs, point{ 0, 0 }, point{ 5, 5 }, costs[0], blocks, ref(shouldDraw));

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                terminate();
            }
            if (event.type == sf::Event::Resized) {
                size = window.getSize();
                sf::View view(sf::FloatRect(0, 0, size.x, size.y));
                window.setView(view);
            }

            shouldDraw = true;
        }

        if (shouldDraw) {
            draw(size, window, containerSize, blockSize, blocks);
            shouldDraw = false;
        }
    }
    return 0;
}

