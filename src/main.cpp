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
    std::string text,
    int charSize = 18,
    sf::Color outColor = sf::Color(255, 255, 255),
    sf::Color textColor = sf::Color(255, 255, 255),
    sf::Color boxColor = sf::Color(100, 100, 100),
    sf::Color textOutColor = sf::Color(255, 255, 255),
    int outlineThickness = 2,
    int textOutlineThickness = 0
) {

    label_data _label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    _label.text = (sf::Text());
    _label.text.setFont(font);
    _label.text.setString(text);
    _label.text.setCharacterSize(charSize);
    _label.text.setFillColor(textColor);
    _label.text.setOutlineColor(textOutColor);
    _label.text.setOutlineThickness(textOutlineThickness);
    auto textSize = _label.text.getGlobalBounds();

    auto boxSize = sf::Vector2f(textSize.width + pad * 2.0, textSize.height - 5.0 + pad * 2.0);

    _label.box = sf::RoundedRectangleShape(boxSize, 10, 20);
    _label.box.setFillColor(boxColor);
    _label.box.setPosition(pos);
    _label.box.setOutlineColor(outColor);
    _label.box.setOutlineThickness(outlineThickness);

    _label.text.setPosition(sf::Vector2f(pos.x + boxSize.x / 2.0 - textSize.width / 2.0, pos.y + boxSize.y / 2.0 - textSize.height + 5.0));
    pos.y += boxSize.y + 5;
    return _label;
}

auto create_textbox(
    sf::Font& font,
    sf::Vector2f& pos,
    float pad,
    sf::Vector2f size,
    int charSize = 18
) {

    label_data _label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    _label.text = (sf::Text());
    _label.text.setFont(font);
    _label.text.setCharacterSize(charSize);
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

void update_label_pos(label_data& label, sf::Vector2f pos) {
    label.box.setPosition(pos);
    auto textSize = label.text.getGlobalBounds();
    label.text.setPosition(sf::Vector2f(pos.x + label.box.getSize().x / 2 - textSize.width / 2, pos.y + label.box.getSize().y / 2.0 - textSize.height + 5));

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
    float ratio = (textBox.height + 5.0f) / containerBox.height;
    if (ratio < 1) {
        ratio = 1;
    }
    float height = containerBox.height / ratio;
    if (height < 10.0f) height = 10.0f;
    input.scrollThumb.setSize(sf::Vector2f(10.0f, height));
    if (set_pos) {
        input._label.text.setString(input.value);
        auto scrollThumbBox = input.scrollThumb.getGlobalBounds();
        input.scrollThumb.setPosition(containerBox.left + containerBox.width - scrollThumbBox.width + pad, containerBox.top + pad);
    }
}

void set_thumb_pos(text_input& input, float pad, float ypos) {
    float lower = input._label.box.getGlobalBounds().top + pad;
    float upper =
        input._label.box.getGlobalBounds().top
        + input._label.box.getGlobalBounds().height
        - pad - input.scrollThumb.getGlobalBounds().height;

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
    auto textBox = input._label.text.getGlobalBounds();
    float diff = (containerBox.height - (2.0f * pad) - scrollThumbBox.height);
    if (diff == 0) {
        return 0;
    }

    float offset = textBox.height - (containerBox.height - 3.0f * pad);
    if (offset < 0) offset = 0;
    return offset * (scrollThumbBox.top - containerBox.top - pad) / diff;
}

void draw_char_selection(text_input& input, sf::RenderWindow& window, int k) {
    auto pos = input._label.text.findCharacterPos(k);
    auto glyph = input._label.text.getFont()->getGlyph(input.value[k], input._label.text.getCharacterSize(), false);
    sf::RectangleShape rect(sf::Vector2f(glyph.bounds.width + 2, 20));
    rect.setFillColor(sf::Color(100, 100, 200));
    rect.setPosition(pos.x, pos.y);
    window.draw(rect);
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

    bool focused = false;
    bool thumbPressed = false;
    bool charPressed = false;
    bool selecting = false;
    bool loading = false;
    bool selectingExperiment = false;
    bool selectingAlgorithm = false;
    int selectedAlgorithm = -1;
    int selectedExperiment = -1;
    sf::Vector2f selectedChars;

    sf::Vector2f lastCharPos;
    sf::Vector2f firstCharPos;
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
    texts.push_back(create_label(font, pos, pad, "Experimento 1"));
    texts.push_back(create_label(font, pos, pad, "Experimento 2"));
    texts.push_back(create_label(font, pos, pad, "Experimento 3"));
    texts.push_back(create_label(font, pos, pad, "Experimento 4"));
    texts.push_back(create_label(font, pos, pad, "Experimento 5"));
    texts.push_back(create_label(font, pos, pad, "utilizar input.txt", 20, sf::Color::White, sf::Color::White,
        sf::Color::White, sf::Color::White, 5, 0));
    texts.push_back(create_label(font, pos, pad, "utilizar entrada aleatoria", 20, sf::Color::White, sf::Color::White,
        sf::Color::White, sf::Color::White, 5, 0));

    texts[10].visible = false;
    texts[11].visible = false;

    std::vector<text_input> inputs = {
        {create_textbox(font, pos, pad, { 200, 60 }), "a\nb"}
    };

    set_thumb_values(inputs[0], pad, true);

    point start = { 0, 0 };
    point target = { 0, 15 };
    std::set<point> constraints = { point{0, 20}, point{2, 22} };

    std::vector<button> buttons = {
        {&texts[0], [&]() {
            selectingAlgorithm = true;
            selectedAlgorithm = 0;
            texts[10].visible = true;
            texts[11].visible = true;
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
                std::cout << greedy_search(start, target, cost_all10, heuristic1, 0, 1, constraints, blocks, std::ref(shouldDraw), true) << "\n";
            });
            a1.detach();
        }},
        {&texts[4],[&]() {
            auto a1 = std::thread([&]() {
                fill_blocks(blocks, constraints,  start, target);
                std::cout << a_star(start, target, cost_all10, heuristic1, 0, 1, constraints, blocks, std::ref(shouldDraw), true) << "\n";
            });
            a1.detach();
        }},
        {&texts[5],[&]() {
            selectingExperiment = true;
            selectedExperiment = 0;
            texts[10].visible = true;
            texts[11].visible = true;
            // auto a1 = std::thread(
            //     [&]() {
            //         loading = true;
            //         experiment1();
            //         loading = false;
            //     }
            // );
            // a1.detach();
        }},
        {&texts[10], [&]() {
            if (selectingAlgorithm) {
                std::ifstream file("input.txt");
                int count;
                file >> count;
                std::cout << count << "\n";
                for (int i = 0;i < count;i++) {
                    point _start;
                    point _target;
                    int _cost_id;
                    int _heuristic_id;
                    std::vector<int> _order;
                    std::set<point> _constraints;
                    file >> _start.x >> _start.y;
                    file >> _target.x >> _target.y;
                    std::cout << _start.x << " " << _start.y << "\n";
                    std::cout << _target.x << " " << _target.y << "\n";
                    file >> _cost_id;
                    file >> _heuristic_id;
                    std::cout << _cost_id << " " << _heuristic_id << "\n";
                    for (int j = 0;j < 4;j++) {
                        int k;
                        file >> k;
                        std::cout << k << " ";
                        _order.push_back(k);
                    }
                    std::cout << "\n";
                    int goal_count;
                    file >> goal_count;
                    for (int j = 0;j < goal_count;j++) {
                        point p;
                        file >> p.x >> p.y;
                        _constraints.insert(p);
                    }
                    file.close();
                    auto a1 = std::thread(
                        [_start, _target, &blocks, selectedAlgorithm, _cost_id, &shouldDraw]() {
                            fill_blocks(blocks);
                            switch (selectedAlgorithm) {
                            case 0: {
                                std::cout << _target.x << " " << _target.y << "\n";
                                std::cout << dijkstra(_start, _target, costs[_cost_id], blocks, std::ref(shouldDraw), 0, true) << "\n";
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    );
                    a1.detach();
                }
            }
        }},
        {&texts[11],[&]() {

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
                for (auto& input : inputs) {
                    if (input.hasFocus) {
                        if (event.text.unicode == 13) {
                            input.value.insert(input.value.begin() + input.cursor, '\n');
                            input.cursor++;
                        } else if (event.text.unicode == 8) {
                            if (selecting) {
                                if (selectedChars.y > selectedChars.x) {
                                    input.value.erase(input.value.begin() + selectedChars.x, input.value.begin() + selectedChars.y);
                                    input.cursor = selectedChars.x;
                                } else {
                                    input.value.erase(input.value.begin() + selectedChars.y - 1, input.value.begin() + selectedChars.x);
                                    input.cursor = selectedChars.y - 1;
                                    if (selectedChars.y - 1 > input.value.size()) {
                                        input.cursor = input.value.size();
                                    }
                                }
                                selecting = false;
                                charPressed = false;
                            } else if (input.cursor > 0) {
                                input.value.erase(input.value.begin() + input.cursor - 1);
                                input.cursor--;
                            }
                        } else {
                            input.value.insert(input.value.begin() + input.cursor, static_cast<char>(event.text.unicode));
                            input.cursor++;
                        }
                        input._label.text.setString(input.value);
                        set_thumb_values(input, pad);
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved && !loading) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (thumbPressed) {
                    for (auto& input : inputs) {
                        if (input.thumbPressed) {
                            float ypos = input.scrollThumb.getGlobalBounds().top + (mousePos.y - lastMousePos.y);
                            set_thumb_pos(input, pad, ypos);
                            break;
                        }
                    }
                    lastMousePos = sf::Vector2f(mousePos);
                } else if (charPressed) {
                    for (auto& input : inputs) {
                        if (input.hasFocus) {
                            for (int j = 0; j < input.value.size(); j++) {
                                auto pos = input._label.text.findCharacterPos(j);
                                if (pos.x < mousePos.x && pos.y < mousePos.y) {
                                    selectedChars.y = j + 1;
                                    continue;
                                }
                                if (pos.x > mousePos.x && pos.y > mousePos.y) {
                                    break;
                                }
                            }
                        }
                        selecting = true;
                    }
                } else {
                    for (int i = 0; i < buttons.size(); i++) {
                        if (buttons[i]._label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            buttons[i]._label->box.setFillColor(sf::Color(150, 150, 150));
                        } else {
                            buttons[i]._label->box.setFillColor(sf::Color(100, 100, 100));
                        }
                    }
                    for (auto& input : inputs) {
                        if (input._label.box.getGlobalBounds().contains(mousePos.x, mousePos.y) &&
                            !input.scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)
                            ) {
                            window.setMouseCursor(textCursor);
                        } else {
                            window.setMouseCursor(arrowCursor);
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && !loading) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < buttons.size(); i++) {
                        if (buttons[i]._label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            buttons[i].pressed = true;
                            buttons[i]._label->box.setFillColor(sf::Color(200, 200, 200));
                        }
                    }
                    for (auto& input : inputs) {
                        if (input._label.box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            input._label.box.setOutlineColor(sf::Color(130, 130, 130));
                            input.hasFocus = true;
                            focused = true;
                            if (input.scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                thumbPressed = true;
                                input.thumbPressed = true;
                                lastMousePos = sf::Vector2f(mousePos);
                            } else {
                                for (int j = 0; j < input.value.size(); j++) {
                                    auto pos = input._label.text.findCharacterPos(j);
                                    if (pos.x < mousePos.x && pos.y < mousePos.y) {
                                        if (input.value[j] == '\n') {
                                            selectedChars.x = j;
                                            input.cursor = j;
                                        } else {
                                            selectedChars.x = j + 1;
                                            input.cursor = j + 1;
                                        }
                                        continue;
                                    }
                                    if (pos.x > mousePos.x && pos.y > mousePos.y) {
                                        break;
                                    }
                                }
                                selectedChars.y = -1;
                                charPressed = true;
                            }
                        } else {
                            input._label.box.setOutlineColor(sf::Color::White);
                            input.hasFocus = false;
                            focused = false;
                            selecting = false;
                            charPressed = false;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && !loading) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (thumbPressed) {
                        thumbPressed = false;
                        for (auto& input : inputs) {
                            if (input.thumbPressed) {
                                input.thumbPressed = false;
                                break;
                            }
                        }
                    } else if (charPressed) {
                        charPressed = false;
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
            if (event.type == sf::Event::KeyPressed && !loading) {
                if (focused) {
                    if (event.key.code == sf::Keyboard::Left) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                if (input.cursor > 0) {
                                    input.cursor--;
                                    if (input.value[input.cursor] == '\n') {
                                        set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top - input._label.text.getCharacterSize());
                                    }
                                }
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::Right) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                if (input.cursor < input.value.size()) {
                                    if (input.value[input.cursor] == '\n') {
                                        set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top + input._label.text.getCharacterSize());
                                    }
                                    input.cursor++;
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
                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top + input._label.text.getCharacterSize());
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

                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top - input._label.text.getCharacterSize());
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
            sf::Text instructions;
            instructions.setFont(font);
            instructions.setString("Use as setas para mover o mapa\nUse + e - para aumentar e diminuir o zoom");
            instructions.setCharacterSize(20);
            instructions.setFillColor(sf::Color(10, 10, 10));
            instructions.setOutlineColor(sf::Color(255, 255, 255));
            instructions.setOutlineThickness(5);
            auto instructionsSize = instructions.getGlobalBounds();
            instructions.setPosition(size.x / 2.0 - instructionsSize.width / 2.0, size.y / 2.0 - instructionsSize.height - 10);
            instructionsSize = instructions.getGlobalBounds();
            window.draw(instructions);
            update_label_pos(texts[10], sf::Vector2f(instructions.getPosition().x, instructionsSize.top + instructionsSize.height + 10));
            auto textBox = texts[10].box.getGlobalBounds();
            update_label_pos(texts[11], sf::Vector2f(instructions.getPosition().x, textBox.top + textBox.height + 10));

            for (int i = 0; i < texts.size();i++) {
                if (texts[i].visible) {
                    window.draw(texts[i].box);
                    window.draw(texts[i].text);
                }
            }
            for (auto& input : inputs) {
                window.draw(input._label.box);

                auto textBox = input._label.text.getGlobalBounds();

                auto containerBox = input._label.box.getGlobalBounds();
                containerBox.height -= pad * 2.0f;
                containerBox.width -= pad * 2.0f;
                float sub = get_scroll_sub(input, pad);

                glEnable(GL_SCISSOR_TEST);
                glScissor(
                    containerBox.left + pad,
                    size.y - containerBox.top - containerBox.height - pad,
                    containerBox.width,
                    containerBox.height
                );

                if (selecting) {
                    if (selectedChars.y != -1) {
                        for (int k = selectedChars.x; k < selectedChars.y; k++) {
                            draw_char_selection(input, window, k);
                        }
                        for (int k = selectedChars.x - 1; k > selectedChars.y - 2; k--) {
                            draw_char_selection(input, window, k);
                        }
                    }
                }
                input._label.text.setPosition(containerBox.left + pad, containerBox.top + pad - sub);
                window.draw(input._label.text);
                if (input.hasFocus) {
                    auto charSize = input._label.text.findCharacterPos(input.cursor);
                    input.cursorLine.setPosition(charSize.x, charSize.y);
                    window.draw(input.cursorLine);
                }
                glDisable(GL_SCISSOR_TEST);
                window.draw(input.scrollThumb);
            }
            if (loading) {
                sf::Text loadingText;
                loadingText.setFont(font);
                loadingText.setString("Executando...");
                loadingText.setCharacterSize(35);
                loadingText.setFillColor(sf::Color(100, 100, 100));
                loadingText.setOutlineColor(sf::Color(255, 255, 255));
                loadingText.setOutlineThickness(5);
                auto textSize = loadingText.getGlobalBounds();
                loadingText.setPosition(size.x / 2.0 - textSize.width / 2.0, size.y / 2.0 - textSize.height / 2.0);
                window.draw(loadingText);
            }
            window.display();
            shouldDraw = false;
        }
    }
    return 0;
}
