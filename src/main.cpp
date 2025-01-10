#include <constants.h>
#include <costs.h>
#include <print.h>
#include <rectshape.h>
#include <structures.h>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <stack>
#include <thread>
#include <tuple>
#include <vector>

using namespace std;

void draw(
    sf::Vector2u& size,
    sf::RenderWindow& window,
    float& containerSize,
    float& blockSize,
    block blocks[space_size][space_size],
    label texts[]
) {
    window.clear(sf::Color(225, 225, 225));

    for (int y = 0; y < space_size; y++) {
        for (int x = 0; x < space_size; x++) {
            int yindex = space_size - y - 1;
            point pos = {
                x * 50 + x * 10 + size.x / 2.0 - containerSize / 2.0,
                yindex * 50 + yindex * 10 + size.y / 2.0 - containerSize / 2.0
            };

            blocks[x][y].shape.setPosition(pos.x, pos.y);
            window.draw(blocks[x][y].shape);

            stringstream ss;
            ss << fixed << setprecision(0) << blocks[x][y].info.cost;
            blocks[x][y].text.setString(ss.str());

            auto textSize = blocks[x][y].text.getGlobalBounds();

            blocks[x][y].text.setPosition(
                pos.x + blockSize / 2.0 - textSize.width / 2.0,
                pos.y + blockSize / 2.0 - textSize.height / 2.0
            );

            window.draw(blocks[x][y].text);
        }
    }
    for (int i = 0; i < 2;i++) {
        window.draw(texts[i].box);
        window.draw(texts[i].text);
    }
    window.display();
}

void set_block_colors(
    block blocks[space_size][space_size],
    point next,
    bool& shouldDraw
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                blocks[i][j].shape.setFillColor(
                    sf::Color(255, 0, 0));
            } else if (blocks[i][j].info.cost != -1) {
                blocks[i][j].shape.setFillColor(
                    sf::Color(100, 200, 100));
            } else {
                blocks[i][j].shape.setFillColor(
                    sf::Color(100, 100, 100));
            }
        }
    }
    shouldDraw = true;

    this_thread::sleep_for(chrono::milliseconds(10));

}

void dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size], bool& shouldDraw
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
                cout << "Found target\n"
                    << blocks[node.x][node.y].info.cost + 1 << "\n";
            }
            float costValue = cost(i, blocks[node.x][node.y].info.step + 1);
            if (blocks[next.x][next.y].info.cost == -1 || blocks[node.x][node.y].info.cost + costValue < blocks[next.x][next.y].info.cost) {
                stack.push(next);

                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;

                set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    calculate_path(start, target, blocks, target, shouldDraw);
}

auto create_label(
    sf::Font& font,
    sf::Vector2f& pos,
    float pad
) {

    label _label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    _label.text = (sf::Text());
    _label.text.setFont(font);
    _label.text.setString("scr");
    _label.text.setCharacterSize(18);
    auto textSize = _label.text.getGlobalBounds();
    textSize.height *= 1;

    auto boxSize = sf::Vector2f(textSize.width + pad * 2, textSize.height * 0.75 + pad * 2);

    _label.box = sf::RoundedRectangleShape(boxSize, 10, 20);
    _label.box.setFillColor(sf::Color(100, 100, 100));

    _label.box.setPosition(pos);
    _label.text.setPosition(sf::Vector2f(pos.x + boxSize.x / 2 - textSize.width / 2, pos.y + boxSize.y / 2.0 - textSize.height * 1.25));
    pos.y += boxSize.y + 5;
    return _label;
}

int main() {
    cout << "Hello, World!\n";

    sf::ContextSettings settings;
    settings.antialiasingLevel = 5;
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window",
        sf::Style::Default, settings);
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
            blocks[i][j] = {
                sf::RoundedRectangleShape(sf::Vector2f(50, 50), 10, 20),
                sf::Text(),
                {-1, -1}
            };
            blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
            blocks[i][j].text.setFont(font);
            blocks[i][j].text.setFillColor(sf::Color(255, 255, 255));
            blocks[i][j].text.setCharacterSize(18);
        }
    }

    label texts[2];
    sf::Vector2f pos(10, 10);

    texts[0] = create_label(font, pos, pad);
    texts[1] = create_label(font, pos, pad);

    thread t1(dfs, point{ 0, 0 }, point{ 5, 5 }, costs[3], blocks, ref(shouldDraw));

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
            draw(size, window, containerSize, blockSize, blocks, texts);
            shouldDraw = false;
        }
    }
    return 0;
}
