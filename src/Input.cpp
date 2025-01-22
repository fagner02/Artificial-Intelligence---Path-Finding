#include <Input.h>
#include <TextBox.h>

TextBox* Input::add(TextBox* input) {
    inputs.push_back(input);
    input->inputs = this;
    return input;
}

void Input::MousePressed(sf::Vector2i mousePos) {
    inputFocused = false;
    for (auto& input : inputs) {
        if (input->label->box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            input->label->box.setOutlineColor(sf::Color(130, 130, 130));
            input->hasFocus = true;
            inputFocused = true;
            isHovering = true;
            if (input->scrollThumb.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                thumbPressed = true;
                charPressed = false;
                isHovering = false;
            } else {
                for (int j = 0; j < input->value.size(); j++) {
                    auto pos = input->label->text.findCharacterPos(j);
                    if (pos.x < mousePos.x && pos.y < mousePos.y) {
                        if (input->value[j] == '\n') {
                            selectedChars.x = j;
                            input->cursor = j;
                        } else {
                            selectedChars.x = j + 1;
                            input->cursor = j + 1;
                        }
                        continue;
                    }
                    if (pos.x > mousePos.x && pos.y > mousePos.y) {
                        break;
                    }
                }
                selectedChars.y = -1;
                charPressed = true;
                thumbPressed = false;
                cursorChanged(input);
            }
        } else {
            input->label->box.setOutlineColor(sf::Color::White);
            input->hasFocus = false;
        }
    }
    if (!inputFocused) {
        selecting = false;
        charPressed = false;
    }
}

void Input::MouseMove(sf::Vector2i pos) {
    isHovering = false;
    for (auto& input : inputs) {
        if (thumbPressed && input->hasFocus) {
            float ypos = input->scrollThumb.getGlobalBounds().top + (pos.y - pos.y);
            input->setThumbPos(ypos);
            break;
        } else if (charPressed) {
            if (input->hasFocus) {
                selecting = true;
                for (int j = 0; j < input->value.size(); j++) {
                    auto charPos = input->label->text.findCharacterPos(j);
                    if (charPos.x < pos.x && charPos.y < pos.y) {
                        selectedChars.y = j + 1;
                        continue;
                    }
                    if (charPos.x > pos.x && charPos.y > pos.y) {
                        break;
                    }
                }
                cursorChanged(input);
            }
        }
        if (input->label->box.getGlobalBounds().contains(pos.x, pos.y) && !input->scrollThumb.getGlobalBounds().contains(pos.x, pos.y)) {
            isHovering = true;
        }
    }
}

void Input::MouseReleased(sf::Vector2i pos) {
    thumbPressed = false;
    charPressed = false;

    if (inputFocused) {
        for (auto& input : inputs) {
            if (input->label->box.getGlobalBounds().contains(pos.x, pos.y) && !input->scrollThumb.getGlobalBounds().contains(pos.x, pos.y)) {
                isHovering = true;
                break;
            }
        }
    }

}

void Input::KeyPressed(sf::Event::KeyEvent key) {
    if (!inputFocused) { return; }
    if (key.code == sf::Keyboard::Left) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                if (input->cursor > 0) {
                    input->cursor--;
                    if (input->value[input->cursor] == '\n') {
                        input->setThumbPos(input->scrollThumb.getGlobalBounds().top - input->label->text.getCharacterSize());
                    }

                    cursorChanged(input);
                }
            }
        }
    }
    if (key.code == sf::Keyboard::Right) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                if (input->cursor < input->value.size()) {
                    if (input->value[input->cursor] == '\n') {
                        input->setThumbPos(input->scrollThumb.getGlobalBounds().top + input->label->text.getCharacterSize());
                    }
                    input->cursor++;

                    cursorChanged(input);
                }
            }
        }
    }
    if (key.code == sf::Keyboard::Down) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                char c = input->value[input->cursor];
                auto index1 = input->value.find_last_of('\n', input->cursor - 1);
                if (index1 == std::string::npos || input->cursor == 0) {
                    index1 = 0;
                }
                auto index2 = input->value.find_first_of('\n', input->cursor);
                if (index2 == std::string::npos) {
                    index2 = input->value.size();
                }
                auto index3 = input->value.find_first_of('\n', index2 + 1);
                if (index3 == std::string::npos) {
                    index3 = input->value.size();
                }

                input->cursor = index2 + (input->cursor - index1) + (index1 == 0 ? 1 : 0);
                if (input->cursor > index3) {
                    input->cursor = index3;
                }

                cursorChanged(input);
                input->setThumbPos(input->scrollThumb.getGlobalBounds().top + input->label->text.getCharacterSize());
            }
        }
    }
    if (key.code == sf::Keyboard::Up) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                char c = input->value[input->cursor];
                auto index1 = input->value.find_last_of('\n', input->cursor + (c == '\n' ? -1 : -1));
                if (index1 == std::string::npos || input->cursor == 0) {
                    index1 = 0;
                }
                auto index2 = input->value.find_last_of('\n', index1 - 1);
                if (index2 == std::string::npos) {
                    index2 = 0;
                }
                input->cursor = index2 + (input->cursor - index1) + (index2 == 0 ? -1 : 0);
                if (input->cursor > index1) {
                    input->cursor = index1;
                }
                if (input->cursor < 0) {
                    input->cursor = 0;
                }

                cursorChanged(input);
                input->setThumbPos(input->scrollThumb.getGlobalBounds().top - input->label->text.getCharacterSize());
            }
        }
    }
    if (key.code == sf::Keyboard::V && key.control) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                if (selecting) {
                    input->deleteSelected(selectedChars);
                    selecting = false;
                    charPressed = false;
                }
                std::string clipboard = sf::Clipboard::getString().toAnsiString();
                std::cout << clipboard;
                input->value.insert(input->value.begin() + input->cursor, clipboard.begin(), clipboard.end());
                input->cursor += clipboard.size();

                cursorChanged(input);
                input->setThumbValues();
            }
        }
    }
    if (key.code == sf::Keyboard::C && key.control) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                if (selecting) {
                    std::wstring selection = input->getSelect(selectedChars);
                    sf::Clipboard::setString(selection);
                }
            }
        }
    }
    if (key.code == sf::Keyboard::X && key.control) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                if (selecting) {
                    std::wstring selection = input->getSelect(selectedChars);
                    sf::Clipboard::setString(selection);
                    input->deleteSelected(selectedChars);
                    selecting = false;
                    charPressed = false;

                    cursorChanged(input);
                }
            }
        }
    }
    if (key.code == sf::Keyboard::A && key.control) {
        for (auto& input : inputs) {
            if (input->hasFocus) {
                selectedChars.x = 0;
                selectedChars.y = input->value.size();
                selecting = true;
                charPressed = true;
            }
        }
    }
}

void Input::TextEntered(sf::Uint32 unicode) {
    for (auto& input : inputs) {
        if (input->hasFocus) {
            if (unicode == 13) {
                if (input->isMultiline) {
                    input->value.insert(input->value.begin() + input->cursor, '\n');
                    input->cursor++;
                }
            } else if (unicode == 8) {
                if (selecting) {
                    input->deleteSelected(selectedChars);
                    selecting = false;
                    charPressed = false;
                } else if (input->cursor > 0) {
                    input->value.erase(input->value.begin() + input->cursor - 1);
                    input->cursor--;
                }
            } else if (unicode > 31) {
                input->value.insert(input->value.begin() + input->cursor, static_cast<char>(unicode));
                input->cursor++;
            }
            if (input->value.size() == 0) {
                std::cout << "sizeeee\n";
            }
            cursorChanged(input);
        }
    }
}

void Input::cursorChanged(TextBox* input) {
    input->setText(input->value);
    input->setThumbValues();
}