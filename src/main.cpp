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

    label_data label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    label.text = (sf::Text());
    label.text.setFont(font);
    label.text.setString(text);
    label.text.setCharacterSize(charSize);
    label.text.setFillColor(textColor);
    label.text.setOutlineColor(textOutColor);
    label.text.setOutlineThickness(textOutlineThickness);
    auto textSize = label.text.getGlobalBounds();

    auto boxSize = sf::Vector2f(textSize.width + pad * 2.0, textSize.height - 5.0 + pad * 2.0);

    label.box = sf::RoundedRectangleShape(boxSize, 10, 20);
    label.box.setFillColor(boxColor);
    label.box.setPosition(pos);
    label.box.setOutlineColor(outColor);
    label.box.setOutlineThickness(outlineThickness);

    label.text.setPosition(sf::Vector2f(pos.x + boxSize.x / 2.0 - textSize.width / 2.0, pos.y + boxSize.y / 2.0 - textSize.height + 5.0));
    pos.y += boxSize.y + 5;
    return label;
}

auto create_textbox(
    sf::Font& font,
    sf::Vector2f& pos,
    float pad,
    sf::Vector2f size,
    int charSize = 18
) {

    label_data label = {
        sf::Text(),
        sf::RoundedRectangleShape()
    };
    label.text = (sf::Text());
    label.text.setFont(font);
    label.text.setCharacterSize(charSize);
    auto textSize = label.text.getGlobalBounds();
    textSize.height *= 1;

    auto boxSize = sf::Vector2f(textSize.width + pad * 2, textSize.height - 5 + pad * 2);

    label.box = sf::RoundedRectangleShape(size, 10, 20);
    label.box.setFillColor(sf::Color(100, 100, 100));
    label.box.setPosition(pos);
    label.box.setOutlineColor(sf::Color(255, 255, 255));
    label.box.setOutlineThickness(2);

    label.text.setPosition(sf::Vector2f(pos.x + pad - textSize.width / 2, pos.y + pad - textSize.height + 5));

    pos.y += boxSize.y + 5;
    return label;
}

void update_label_pos(label_data& label, sf::Vector2f pos) {
    label.box.setPosition(pos);
    auto textSize = label.text.getGlobalBounds();
    label.text.setPosition(sf::Vector2f(pos.x + label.box.getSize().x / 2 - textSize.width / 2, pos.y + label.box.getSize().y / 2.0 - textSize.height + 5));

}

struct text_input {
    label_data label;
    std::string value;
    std::string tooltip = "";
    bool isMultiline = false;
    int cursor = 0;
    sf::RectangleShape cursorLine = sf::RectangleShape(sf::Vector2f(2, 20));
    bool hasFocus = false;
    bool thumbPressed = false;
    sf::RoundedRectangleShape scrollThumb = sf::RoundedRectangleShape(sf::Vector2f(10, 10), 5, 20);
};

void set_thumb_values(text_input& input, float pad, bool set_pos = false) {
    auto containerBox = input.label.box.getGlobalBounds();
    auto textBox = input.label.text.getGlobalBounds();
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
        input.label.text.setString(input.value);
        auto scrollThumbBox = input.scrollThumb.getGlobalBounds();
        input.scrollThumb.setPosition(containerBox.left + containerBox.width - scrollThumbBox.width + pad, containerBox.top + pad);
    }
}

void set_thumb_pos(text_input& input, float pad, float ypos) {
    float lower = input.label.box.getGlobalBounds().top + pad;
    float upper =
        input.label.box.getGlobalBounds().top
        + input.label.box.getGlobalBounds().height
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
    auto containerBox = input.label.box.getGlobalBounds();
    auto textBox = input.label.text.getGlobalBounds();
    float diff = (containerBox.height - (2.0f * pad) - scrollThumbBox.height);
    if (diff == 0) {
        return 0;
    }

    float offset = textBox.height - (containerBox.height - 3.0f * pad);
    if (offset < 0) offset = 0;
    return offset * (scrollThumbBox.top - containerBox.top - pad) / diff;
}

void draw_char_selection(text_input& input, sf::RenderWindow& window, int k) {
    auto pos = input.label.text.findCharacterPos(k);
    auto glyph = input.label.text.getFont()->getGlyph(input.value[k], input.label.text.getCharacterSize(), false);
    sf::RectangleShape rect(sf::Vector2f(glyph.bounds.width + 2, 20));
    rect.setFillColor(sf::Color(100, 100, 200));
    rect.setPosition(pos.x, pos.y);
    window.draw(rect);
}

void delete_selected(text_input& input, sf::Vector2f selectedChars) {
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
}

std::string get_select(text_input& input, sf::Vector2f selectedChars) {
    if (selectedChars.x > selectedChars.y) {
        return input.value.substr(selectedChars.y, selectedChars.x - selectedChars.y);
    } else {
        return input.value.substr(selectedChars.x, selectedChars.y - selectedChars.x);
    }
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

    std::vector<text_input> inputs = {
        {create_textbox(font, pos, pad, { 300, 40 }), "", "Nome do arquivo de saida", false}
    };

    texts.push_back(create_label(font, pos, pad, "utilizar input.txt", 20, sf::Color::White, sf::Color::White,
        sf::Color::White, sf::Color::White, 5, 0));
    texts.push_back(create_label(font, pos, pad, "utilizar entrada aleatoria", 20, sf::Color::White, sf::Color::White,
        sf::Color::White, sf::Color::White, 5, 0));

    texts[10].visible = false;
    texts[11].visible = false;


    set_thumb_values(inputs[0], pad, true);

    point start = { 0, 0 };
    point target = { 0, 15 };
    std::set<point> constraints = { point{0, 20}, point{2, 22} };

    std::vector<button> buttons = {
        {&texts[0], [&]() {
            selectingAlgorithm = true;
            selectedAlgorithm = 0;
         }},
        {&texts[1],[&]() {
            selectingAlgorithm = true;
            selectedAlgorithm = 1;
        } },
        {&texts[2],[&]() {
           selectingAlgorithm = true;
            selectedAlgorithm = 2;
        }},
        {&texts[3],[&]() {
            selectingAlgorithm = true;
            selectedAlgorithm = 3;
        }},
        {&texts[4],[&]() {
            selectingAlgorithm = true;
            selectedAlgorithm = 4;
        }},
        {&texts[5],[&]() {
            selectingExperiment = true;
            selectedExperiment = 0;
        }},
        {&texts[6],[&]() {
            selectingExperiment = true;
            selectedExperiment = 1;
        }},
        {&texts[7],[&]() {
            selectingExperiment = true;
            selectedExperiment = 2;
        }},
        {&texts[8],[&]() {
            selectingExperiment = true;
            selectedExperiment = 3;
        }},
        {&texts[9],[&]() {
            selectingExperiment = true;
            selectedExperiment = 4;
        }},
        {&texts[10], [&]() {
            texts[10].visible = false;
            texts[11].visible = false;

            std::vector<point> start_points;
            std::vector<point> target_points;
            std::vector<int> cost_ids;
            std::vector<int> heuristic_ids;
            std::vector<std::vector<int>> orders;
            std::vector<std::set<point>> constraints;
            std::ifstream file("input.txt");
            int count;
            file >> count;
            std::cout << count << "\n";
            for (int i = 0;i < count;i++) {
                start_points.push_back(point());
                target_points.push_back(point());
                cost_ids.push_back(0);
                heuristic_ids.push_back(0);
                orders.push_back(std::vector<int>());
                constraints.push_back(std::set<point>());
                file >> start_points[i].x >> start_points[i].y;
                file >> target_points[i].x >> target_points[i].y;
                file >> cost_ids[i];
                file >> heuristic_ids[i];
                for (int j = 0;j < 4;j++) {
                    int k;
                    file >> k;
                    orders[i].push_back(k);
                }
                int goal_count;
                file >> goal_count;
                for (int j = 0;j < goal_count;j++) {
                    point p;
                    file >> p.x >> p.y;
                    constraints[i].insert(p);
                }
                file.close();
            }
            if (selectingAlgorithm) {
                selectingAlgorithm = false;
                selectingExperiment = false;

                auto a1 = std::thread(
                    [start_points, target_points, count, constraints, &blocks, selectedAlgorithm, cost_ids, &shouldDraw]() {
                        for (int i = 0; i < count;i++) {
                            fill_blocks(blocks, constraints[i], start_points[i], target_points[i]);
                            switch (selectedAlgorithm) {
                                case 0: {
                                    std::cout << dijkstra(start_points[i], target_points[i], costs[cost_ids[i]], blocks, std::ref(shouldDraw), 0, constraints[i], true) << "\n";
                                    break;
                                }
                                case 1: {
                                    std::cout << bfs(start_points[i], target_points[i], costs[cost_ids[i]], blocks, std::ref(shouldDraw), 0, constraints[i], { 0,1,2,3 }, true) << "\n";
                                    break;
                                }
                                case 2: {
                                    std::cout << dfs(start_points[i], target_points[i], costs[cost_ids[i]], 0, constraints[i], blocks, std::ref(shouldDraw), { 0,1,2,3 }, true) << "\n";
                                    break;
                                }
                                case 3: {
                                    std::cout << greedy_search(start_points[i], target_points[i], costs[cost_ids[i]], heuristic_fns[0], cost_ids[i], 0, constraints[i], blocks, std::ref(shouldDraw), true) << "\n";
                                    break;
                                }
                                case 4: {
                                    std::cout << a_star(start_points[i], target_points[i], costs[cost_ids[i]], heuristic_fns[0], cost_ids[i], 0, constraints[i], blocks, std::ref(shouldDraw), true) << "\n";
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                    }
                );
                a1.detach();
            }
            if (selectingExperiment) {
                selectingAlgorithm = false;
                selectingExperiment = false;
                auto a1 = std::thread(
                    [&]() {
                        switch (selectedExperiment) {
                            case 0: {
                                experiment1(start_points, target_points);
                                break;
                            }
                            case 1: {
                                experiment2(start_points, target_points);
                                break;
                            }
                            case 2: {
                                experiment3(start_points, target_points);
                                break;
                            }
                            case 3: {
                                experiment4(start_points, target_points, orders);
                                break;
                            }
                            case 4: {
                                experiment5(start_points, target_points, orders, constraints);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                );
                a1.detach();
            }
        } },
        { &texts[11],[&]() {
            texts[10].visible = false;
            texts[11].visible = false;
            if (selectingAlgorithm) {
                selectingAlgorithm = false;
                selectingExperiment = false;
                auto a1 = std::thread(
                    [&blocks, &shouldDraw, selectedAlgorithm]() {
                        srand((unsigned)time(NULL));
                        point start = { rand() % 31, rand() % 31 };
                        point target = { rand() % 31, rand() % 31 };
                        std::set<point> constraints = {};
                        while (constraints.size() < 4) {
                            constraints.insert({ rand() % 31, rand() % 31 });
                        }
                        fill_blocks(blocks, constraints, start, target);
                        switch (selectedAlgorithm) {
                        case 0: {
                            std::cout << dijkstra(start, target, costs[0], blocks, std::ref(shouldDraw), 0, constraints, true) << "\n";
                            break;
                        }
                        case 1: {
                            std::cout << bfs(start, target, costs[0], blocks, std::ref(shouldDraw), 0, constraints, { 0,1,2,3 }, true) << "\n";
                            break;
                        }
                        case 2: {
                            std::cout << dfs(start, target, costs[0], 0, constraints, blocks, std::ref(shouldDraw), { 0,1,2,3 }, true) << "\n";
                            break;
                        }
                        case 3: {
                            std::cout << greedy_search(start, target, costs[0], heuristic_fns[0], 0, 0, constraints, blocks, std::ref(shouldDraw), true) << "\n";
                            break;
                        }
                        case 4: {
                            std::cout << a_star(start, target, costs[0], heuristic_fns[0], 0, 0, constraints, blocks, std::ref(shouldDraw), true) << "\n";
                            break;
                        }
                        default:
                            break;
                        }
                    }
                );
                a1.detach();
            }
            if (selectingExperiment) {
                selectingAlgorithm = false;
                selectingExperiment = false;
                auto a1 = std::thread(
                    [&blocks, &shouldDraw, &selectedExperiment, &loading]() {
                        loading = true;
                        switch (selectedExperiment) {
                            case 0: {
                                experiment1();
                                break;
                            }
                            case 1: {
                                experiment2();
                                break;
                            }
                            case 2: {
                                experiment3();
                                break;
                            }
                            case 3: {
                                experiment4();
                                break;
                            }
                            case 4: {
                                experiment5();
                                break;
                            }
                            default:
                                break;
                        }
                        loading = false;
                    }
                );
                a1.detach();
            }
        } }
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
                                auto pos = input.label.text.findCharacterPos(j);
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
                        if (input.label.box.getGlobalBounds().contains(mousePos.x, mousePos.y) &&
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
                        if (input.label.box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            input.label.box.setOutlineColor(sf::Color(130, 130, 130));
                            input.hasFocus = true;
                            focused = true;
                            if (input.scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                thumbPressed = true;
                                input.thumbPressed = true;
                                lastMousePos = sf::Vector2f(mousePos);
                            } else {
                                for (int j = 0; j < input.value.size(); j++) {
                                    auto pos = input.label.text.findCharacterPos(j);
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
                            input.label.box.setOutlineColor(sf::Color::White);
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
                                        set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top - input.label.text.getCharacterSize());
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
                                        set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top + input.label.text.getCharacterSize());
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
                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top + input.label.text.getCharacterSize());
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

                                set_thumb_pos(input, pad, input.scrollThumb.getGlobalBounds().top - input.label.text.getCharacterSize());
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::V && event.key.control) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                std::string clipboard = sf::Clipboard::getString();
                                if (selecting) {
                                    delete_selected(input, selectedChars);
                                    selecting = false;
                                    charPressed = false;
                                } else {
                                    input.value.insert(input.value.begin() + input.cursor, clipboard.begin(), clipboard.end());
                                    input.cursor += clipboard.size();
                                    input.label.text.setString(input.value);
                                }
                                set_thumb_values(input, pad);
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::C && event.key.control) {
                        for (auto& input : inputs) {
                            if (input.hasFocus) {
                                if (selecting) {
                                    std::string selection = get_select(input, selectedChars);
                                    sf::Clipboard::setString(selection);
                                    std::cout << selection;
                                }
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
            if (event.type == sf::Event::TextEntered) {
                for (auto& input : inputs) {
                    if (input.hasFocus) {
                        if (event.text.unicode == 13) {
                            if (input.isMultiline) {
                                input.value.insert(input.value.begin() + input.cursor, '\n');
                                input.cursor++;
                            }
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
                        input.label.text.setString(input.value);
                        set_thumb_values(input, pad);
                    }
                }
            }
            shouldDraw = true;
        }

        if (shouldDraw) {
            draw(size, window, containerSize, blockSize, blocks, scale, translate);
            if (selectingAlgorithm || selectingExperiment) {
                texts[10].visible = true;
                texts[11].visible = true;
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
            }
            for (int i = 0; i < texts.size();i++) {
                if (texts[i].visible) {
                    window.draw(texts[i].box);
                    window.draw(texts[i].text);
                }
            }
            for (auto& input : inputs) {
                window.draw(input.label.box);

                auto textBox = input.label.text.getGlobalBounds();

                auto containerBox = input.label.box.getGlobalBounds();
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
                input.label.text.setPosition(containerBox.left + pad, containerBox.top + pad - sub);
                if (input.value.size() == 0) {
                    input.label.text.setString(input.tooltip);
                    input.label.text.setFillColor(sf::Color(150, 150, 150));
                    window.draw(input.label.text);
                    input.label.text.setFillColor(sf::Color(255, 255, 255));
                } else {
                    window.draw(input.label.text);
                }
                if (input.hasFocus) {
                    auto charSize = input.label.text.findCharacterPos(input.cursor);
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
