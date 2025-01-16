#include <constants.h>
#include <costs.h>
#include <print.h>
#include <rectshape.h>
#include <structures.h>
#include <heuristics.h>
#include <path_finding.h>
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
#include <fstream>
#include <experiments.h>
#include <json/json.h>
#include <GL/gl.h>
#include <set>

void draw(
    sf::Vector2u& size,
    sf::RenderWindow& window,
    float& containerSize,
    float& blockSize,
    block blocks[space_size][space_size],
    sf::Vector2f scale,
    sf::Vector2f translate
) {
    window.clear(sf::Color(225, 225, 225));

    for (int y = 0; y < space_size; y++) {
        for (int x = 0; x < space_size; x++) {
            int yindex = space_size - y - 1;
            point pos = {
                (x * 50.0 + x * 10.0 + size.x / 2.0 - containerSize / 2.0 + translate.x) * scale.x,
               (yindex * 50.0 + yindex * 10.0 + size.y / 2.0 - containerSize / 2.0 + translate.y) * scale.y
            };
            blocks[x][y].shape.setScale(scale);
            blocks[x][y].text.setScale(scale);
            blocks[x][y].shape.setPosition(pos.x, pos.y);
            auto t = blocks[x][y].shape.getLocalBounds();
            window.draw(blocks[x][y].shape);

            std::stringstream ss;
            ss << std::fixed << std::setprecision(0) << blocks[x][y].info.cost;
            blocks[x][y].text.setString(ss.str());

            auto textSize = blocks[x][y].text.getLocalBounds();

            blocks[x][y].text.setPosition(
                (pos.x + (t.width * scale.x) / 2.0 - (textSize.width * scale.x) / 2.0),
                (pos.y + (t.height * scale.y) / 2.0 - (textSize.height * scale.y) / 2.0)
            );

            window.draw(blocks[x][y].text);
        }
    }
}

auto create_label(
    sf::Font& font,
    sf::Vector2f& pos,
    float pad,
    std::string text
) {

    label_data _label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    _label.text = (sf::Text());
    _label.text.setFont(font);
    _label.text.setString(text);
    _label.text.setCharacterSize(18);
    auto textSize = _label.text.getGlobalBounds();
    textSize.height *= 1;

    auto boxSize = sf::Vector2f(textSize.width + pad * 2, textSize.height - 5 + pad * 2);

    _label.box = sf::RoundedRectangleShape(boxSize, 10, 20);
    _label.box.setFillColor(sf::Color(100, 100, 100));
    _label.box.setPosition(pos);
    _label.box.setOutlineColor(sf::Color(255, 255, 255));
    _label.box.setOutlineThickness(2);

    _label.text.setPosition(sf::Vector2f(pos.x + boxSize.x / 2 - textSize.width / 2, pos.y + boxSize.y / 2.0 - textSize.height + 5));
    pos.y += boxSize.y + 5;
    return _label;
}

auto create_textbox(
    sf::Font& font,
    sf::Vector2f& pos,
    float pad,
    sf::Vector2f size
) {

    label_data _label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    _label.text = (sf::Text());
    _label.text.setFont(font);
    _label.text.setCharacterSize(18);
    auto textSize = _label.text.getGlobalBounds();
    textSize.height *= 1;

    auto boxSize = sf::Vector2f(textSize.width + pad * 2, textSize.height - 5 + pad * 2);

    _label.box = sf::RoundedRectangleShape(size, 10, 20);
    _label.box.setFillColor(sf::Color(100, 100, 100));
    _label.box.setPosition(pos);
    _label.box.setOutlineColor(sf::Color(255, 255, 255));
    _label.box.setOutlineThickness(2);

    _label.text.setPosition(sf::Vector2f(pos.x + pad - textSize.width / 2, pos.y + pad - textSize.height + 5));

    pos.y += boxSize.y + 5;
    return _label;
}

struct text_input {
    label_data _label;
    std::string value;
    int cursor = 0;
    sf::RectangleShape cursorLine = sf::RectangleShape(sf::Vector2f(2, 20));
    bool hasFocus = false;
    bool thumbPressed = false;
    sf::RoundedRectangleShape scrollThumb = sf::RoundedRectangleShape(sf::Vector2f(10, 10), 5, 20);
};

void set_thumb_values(text_input& input, float pad, bool set_pos = false) {
    auto containerBox = input._label.box.getGlobalBounds();
    auto textBox = input._label.text.getGlobalBounds();
    containerBox.width -= pad * 2.0f;
    containerBox.height -= pad * 2.0f;
    float ratio = (textBox.height + 50) / containerBox.height;
    if (ratio < 1) {
        ratio = 1;
    }
    input.scrollThumb.setSize(sf::Vector2f(10, containerBox.height * 1.0f / ratio));
    auto scrollThumbBox = input.scrollThumb.getGlobalBounds();
    if (set_pos) input.scrollThumb.setPosition(containerBox.left + containerBox.width - scrollThumbBox.width + pad, containerBox.top + pad);
}

void set_thumb_pos(text_input& input, float pad, float ypos) {
    float lower = input._label.box.getGlobalBounds().top + pad;
    float upper = input._label.box.getGlobalBounds().top + input._label.box.getGlobalBounds().height - pad -
        input.scrollThumb.getGlobalBounds().height;
    if (ypos < lower) {
        ypos = lower;
    }
    if (ypos > upper) {
        ypos = upper;
    }
    input.scrollThumb.setPosition(input.scrollThumb.getPosition().x, ypos);
}
float get_scroll_sub(text_input& input, float pad) {
    auto scrollThumbBox = input.scrollThumb.getGlobalBounds();
    auto containerBox = input._label.box.getGlobalBounds();
    return (scrollThumbBox.top - containerBox.top - pad) / (containerBox.height - 2.0f * pad);
}

int main() {
    std::cout << "Hello, World!\n";

    sf::ContextSettings settings;

    settings.antialiasingLevel = 5;
    sf::RenderWindow window(sf::VideoMode({ 1200, 1000 }), "My window", sf::Style::Default, settings);

    window.setPosition(sf::Vector2i(0, 0));
    auto font = sf::Font();
    if (!font.loadFromFile("./assets/Roboto-Regular.ttf")) {
        std::cout << "Error loading font\n";
    } else {
        std::cout << "Font loaded\n";
    }

    sf::Vector2u size = window.getSize();
    float blockSize = 50;
    float pad = 10;
    float containerSize = (pad * space_size + blockSize * space_size);

    bool shouldDraw = true;

    block blocks[space_size][space_size];

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j] = block{
                sf::RoundedRectangleShape(sf::Vector2f(50, 50), 10, 20),
                sf::Text(),
                visited_info{-1, -1, -1}
            };
            blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
            blocks[i][j].text.setFont(font);
            blocks[i][j].text.setFillColor(sf::Color(255, 255, 255));
            blocks[i][j].text.setCharacterSize(18);
        }
    }

    // Json::Value root;
    // std::ifstream file("./e.json");
    // file >> root;

    // std::cout << "initial: [\n";
    // for (int i = 0; i < root["initial"].size(); i++) {
    //     std::cout << "[" << root["initial"][i][0] << ", " << root["initial"][i][1] << "],\n";
    // }
    // std::cout << "]\n";

    // return 0;


    // experiment1();
    // experiment2();
    // experiment3();
    // experiment4();
    // experiment5();

    bool focused = false;
    bool thumbPressed = false;
    sf::Vector2f lastMousePos;

    sf::Cursor textCursor;
    textCursor.loadFromSystem(sf::Cursor::Text);
    sf::Cursor arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);

    std::vector<label_data> texts;
    sf::Vector2f pos(10, 10);
    texts.push_back(create_label(font, pos, pad, "Dijkstra"));
    texts.push_back(create_label(font, pos, pad, "Bfs"));
    texts.push_back(create_label(font, pos, pad, "Dfs"));
    texts.push_back(create_label(font, pos, pad, "Greedy"));
    texts.push_back(create_label(font, pos, pad, "A*"));

    std::vector<text_input> inputs = {
        {create_textbox(font, pos, pad, { 200, 60 }), "a\nb"}
    };

    set_thumb_values(inputs[0], pad, true);

    point start = { 0, 0 };
    point target = { 0, 15 };
    std::set<point> goals = { point{0, 20}, point{2, 22} };

    std::vector<button> buttons = {
        {&texts[0], [&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks);
                std::cout << dijkstra(start, target, cost_all10, blocks, std::ref(shouldDraw), 0, true) << "\n";
            });
            a1.detach();
         }},
        {&texts[1],[&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks);
                std::cout << bfs(start, target, cost_all10, blocks, std::ref(shouldDraw), 0, {0,1,2,3}, true) << "\n";
            });
            a1.detach();
        } },
        {&texts[2],[&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks);
                std::cout << dfs(start, target, cost_all10, blocks, std::ref(shouldDraw), 0, { 0,1,2,3 }, true) << "\n";
            });
            a1.detach();
        }},
        {&texts[3],[&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks);
                std::cout << greedy_search(start, target, cost_all10, heuristic1, blocks, std::ref(shouldDraw), 0, 1, true) << "\n";
            });
            a1.detach();
        }},
        {&texts[4],[&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks, goals,  start, target);
                std::cout << a_star(start, target, cost_all10, heuristic1, blocks, std::ref(shouldDraw), 0, 1, true, goals) << "\n";
            });
            a1.detach();
        }}
    };
    sf::Vector2f scale = { 0.5,0.5 };
    sf::Vector2f translate = { size.x * scale.x,size.y * scale.y };

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                std::terminate();
            }
            if (event.type == sf::Event::Resized) {
                size = window.getSize();
                sf::View view(sf::FloatRect(0, 0, size.x, size.y));
                window.setView(view);
                translate = { size.x * scale.x, size.y * scale.y };
            }
            if (event.type == sf::Event::TextEntered) {
                for (int i = 0; i < inputs.size(); i++) {
                    if (inputs[i].hasFocus) {
                        if (event.text.unicode == 13) {
                            inputs[i].value.insert(inputs[i].value.begin() + inputs[i].cursor, '\n');
                            inputs[i].cursor++;
                        } else if (event.text.unicode == 8) {
                            if (inputs[i].cursor > 0) {
                                inputs[i].value.erase(inputs[i].value.begin() + inputs[i].cursor - 1);
                                inputs[i].cursor--;
                            }
                        } else {
                            inputs[i].value.insert(inputs[i].value.begin() + inputs[i].cursor, static_cast<char>(event.text.unicode));
                            inputs[i].cursor++;
                        }
                        set_thumb_values(inputs[i], pad);
                        inputs[i]._label.text.setString(inputs[i].value);
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (thumbPressed) {
                    for (int i = 0; i < inputs.size(); i++) {
                        if (inputs[i].thumbPressed) {
                            float ypos = inputs[i].scrollThumb.getGlobalBounds().top + (mousePos.y - lastMousePos.y);
                            set_thumb_pos(inputs[i], pad, ypos);
                            break;
                        }
                    }
                    lastMousePos = sf::Vector2f(mousePos);
                } else {
                    for (int i = 0; i < buttons.size(); i++) {
                        if (buttons[i]._label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            buttons[i]._label->box.setFillColor(sf::Color(150, 150, 150));
                        } else {
                            buttons[i]._label->box.setFillColor(sf::Color(100, 100, 100));
                        }
                    }
                    for (int i = 0; i < inputs.size(); i++) {
                        if (inputs[i]._label.box.getGlobalBounds().contains(mousePos.x, mousePos.y) &&
                            !inputs[i].scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)
                            ) {
                            window.setMouseCursor(textCursor);
                        } else {
                            window.setMouseCursor(arrowCursor);
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < buttons.size(); i++) {
                        if (buttons[i]._label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            buttons[i].pressed = true;
                            buttons[i]._label->box.setFillColor(sf::Color(200, 200, 200));
                        }
                    }
                    for (int i = 0; i < inputs.size(); i++) {
                        if (inputs[i]._label.box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            // inputs[i]._label->box.setFillColor(sf::Color(130, 130, 130));
                            inputs[i].hasFocus = true;
                            focused = true;
                            if (inputs[i].scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                thumbPressed = true;
                                inputs[i].thumbPressed = true;
                                lastMousePos = sf::Vector2f(mousePos);
                            } else {
                                for (int j = 0; j < inputs[i].value.size(); j++) {
                                    auto pos = inputs[i]._label.text.findCharacterPos(j);
                                    if (pos.x < mousePos.x && pos.y < mousePos.y) {
                                        inputs[i].cursor = j + 1;
                                        continue;
                                    }
                                    if (pos.x > mousePos.x && pos.y > mousePos.y) {
                                        break;
                                    }
                                }
                            }
                        } else {
                            inputs[i]._label.box.setFillColor(sf::Color(100, 100, 100));
                            inputs[i].hasFocus = false;
                            focused = false;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (thumbPressed) {
                        thumbPressed = false;
                        for (int i = 0; i < inputs.size(); i++) {
                            if (inputs[i].thumbPressed) {
                                inputs[i].thumbPressed = false;
                                break;
                            }
                        }
                    } else {
                        for (int i = 0; i < buttons.size(); i++) {
                            if (buttons[i].pressed && buttons[i]._label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                buttons[i]._label->box.setFillColor(sf::Color(100, 100, 100));
                                buttons[i].fn();
                                buttons[i].pressed = false;
                            } else {
                                buttons[i]._label->box.setFillColor(sf::Color(100, 100, 100));
                                buttons[i].pressed = false;
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (focused) {
                    if (event.key.code == sf::Keyboard::Left) {
                        for (int i = 0; i < inputs.size(); i++) {
                            if (inputs[i].hasFocus) {
                                if (inputs[i].cursor > 0) {
                                    inputs[i].cursor--;

                                }
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::Right) {
                        for (int i = 0; i < inputs.size(); i++) {
                            if (inputs[i].hasFocus) {
                                if (inputs[i].cursor < inputs[i].value.size()) {
                                    inputs[i].cursor++;
                                }
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                char c = input.value[input.cursor];
                                auto index1 = input.value.find_last_of('\n', input.cursor - 1);
                                if (index1 == std::string::npos || input.cursor == 0) {
                                    index1 = 0;
                                }
                                auto index2 = input.value.find_first_of('\n', input.cursor);
                                if (index2 == std::string::npos) {
                                    index2 = input.value.size();
                                }
                                auto index3 = input.value.find_first_of('\n', index2 + 1);
                                if (index3 == std::string::npos) {
                                    index3 = input.value.size();
                                }

                                input.cursor = index2 + (input.cursor - index1) + (index1 == 0 ? 1 : 0);
                                if (input.cursor > index3) {
                                    input.cursor = index3;
                                }
                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top + 10.0f);
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::Up) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                char c = input.value[input.cursor];
                                auto index1 = input.value.find_last_of('\n', input.cursor + (c == '\n' ? -1 : -1));
                                if (index1 == std::string::npos || input.cursor == 0) {
                                    index1 = 0;
                                }
                                auto index2 = input.value.find_last_of('\n', index1 - 1);
                                if (index2 == std::string::npos) {
                                    index2 = 0;
                                }
                                input.cursor = index2 + (input.cursor - index1) + (index2 == 0 ? -1 : 0);
                                if (input.cursor > index1) {
                                    input.cursor = index1;
                                }
                                if (input.cursor < 0) {
                                    input.cursor = 0;
                                }

                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top - 10.0f);
                            }
                        }
                    }

                } else {
                    if (event.key.code == sf::Keyboard::Equal) {
                        scale.x += 0.01;
                        scale.y += 0.01;
                    }
                    if (event.key.code == sf::Keyboard::Dash) {
                        scale.x -= 0.01;
                        scale.y -= 0.01;
                    }
                    if (event.key.code == sf::Keyboard::Up) {
                        translate.y -= 50;
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        translate.y += 50;
                    }
                    if (event.key.code == sf::Keyboard::Left) {
                        translate.x -= 50;
                    }
                    if (event.key.code == sf::Keyboard::Right) {
                        translate.x += 50;
                    }
                }
            }

            shouldDraw = true;
        }

        if (shouldDraw) {
            draw(size, window, containerSize, blockSize, blocks, scale, translate);

            for (int i = 0; i < texts.size();i++) {
                window.draw(texts[i].box);
                window.draw(texts[i].text);
                glDisable(GL_SCISSOR_TEST);
            }
            for (int i = 0; i < inputs.size(); i++) {
                window.draw(inputs[i]._label.box);

                auto textBox = inputs[i]._label.text.getGlobalBounds();

                auto containerBox = inputs[i]._label.box.getGlobalBounds();
                containerBox.height -= pad * 2.0f;
                containerBox.width -= pad * 2.0f;
                float sub = get_scroll_sub(inputs[i], pad);

                glEnable(GL_SCISSOR_TEST);
                glScissor(
                    containerBox.left + pad,
                    size.y - containerBox.top - containerBox.height - pad,
                    containerBox.width,
                    containerBox.height
                );
                inputs[i]._label.text.setPosition(containerBox.left + pad, containerBox.top + pad - sub * textBox.height);
                window.draw(inputs[i]._label.text);
                glDisable(GL_SCISSOR_TEST);
                window.draw(inputs[i].scrollThumb);
                if (inputs[i].hasFocus) {
                    auto charSize = inputs[i]._label.text.findCharacterPos(inputs[i].cursor);
                    inputs[i].cursorLine.setPosition(charSize.x, charSize.y);
                    window.draw(inputs[i].cursorLine);
                }
            }
            window.display();
            shouldDraw = false;
        }
    }
    return 0;
}
