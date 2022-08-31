//Cells simulator Jaclav (c) 2022
//TODO add save via mktemp and load via argv[1]
//TODO Why they are fluctuates
//! Everything is scaled 1x1 grid = 5x5 px
#include <SFML/Graphics.hpp>
#include <vector>

enum State {Dead, Liquid, Solid, Gas, Sand, Source};

struct Particle {
    State state = Dead;
    int lastMoved = 0;
};

#define MOVECELL(newX, newY)\
cells[newX][newY] = cells[x][y];\
cells[x][y].state = Dead;\
cells[newX][newY].lastMoved = iterationNumber;

int main() {
    srand(clock());
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "SFML", sf::Style::Fullscreen);
    sf::Event event;
    //window.setFramerateLimit(2)

    sf::RectangleShape grain(sf::Vector2f(5, 5));
    grain.setFillColor(sf::Color::White);

    int MAX_X = window.getSize().x / 5, MAX_Y = window.getSize().y / 5;
    Particle **cells = new Particle*[MAX_X];
    for(int x = 0; x < MAX_X; x++)
        cells[x] = new Particle[MAX_Y];


    Particle water{Liquid, 0};
    Particle solid{Solid, 0};
    Particle gas{Gas, 0};
    Particle sand{Sand, 0};
    Particle source{Source, 0};
    Particle empty{Dead, 0};//Empty is solid because it takes less CPU
    Particle *choosen = &water;

    bool canMove = true, nextFrame = false;
    int oldIterationNumber = 0, numberOfCells = 0;

    sf::Font font;
    font.loadFromFile("Ubuntu-M.ttf");
    sf::Text text("Iteration: ", font);

    for(int iterationNumber = 0; window.isOpen(); iterationNumber++) {
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            if(sf::Keyboard::isKeyPressed((sf::Keyboard::Space))) {
                if(canMove)
                    oldIterationNumber = iterationNumber;//freez
                else
                    iterationNumber = oldIterationNumber;//unfreeze

                canMove = !canMove;
                window.pollEvent(event);
                break;
            }
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::F))) {
                nextFrame = true;
                canMove = true;
                iterationNumber = oldIterationNumber++;
            }
            while(event.type == sf::Event::LostFocus) {
                canMove = true;
                oldIterationNumber = iterationNumber;
                window.pollEvent(event);
                sf::sleep(sf::milliseconds(100));
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
                choosen = &water;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
                choosen = &solid;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
                choosen = &gas;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
                choosen = &sand;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
                choosen = &source;

            if(event.key.control && sf::Keyboard::isKeyPressed((sf::Keyboard::R))) {
                for(int x = 0; x < MAX_X; x++)
                    for(int y = 0; y < MAX_Y; y++)
                        cells[x][y] = empty;
            }
        }
        //set area 1x1 grid
        if(sf::Mouse::getPosition().x / 5 < MAX_X && sf::Mouse::getPosition().y / 5 < MAX_Y) {
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
                    cells[sf::Mouse::getPosition().x / 5][sf::Mouse::getPosition().y / 5].state == Dead)
                cells[sf::Mouse::getPosition().x / 5][sf::Mouse::getPosition().y / 5] = (*choosen);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                cells[sf::Mouse::getPosition().x / 5][sf::Mouse::getPosition().y / 5] = empty;
        }

        //bold - set area 5x5 grid
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))) {
            struct Particle *boldChoosen;
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
                boldChoosen = choosen;
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
                boldChoosen = &empty;

            for(int x = -2; x <= 2; x++)
                for(int y = -2; y <= 2; y++)
                    if(sf::Mouse::getPosition().x / 5 + x < MAX_X && sf::Mouse::getPosition().y / 5 + y < MAX_Y &&
                            sf::Mouse::getPosition().x / 5 + x > 0 && sf::Mouse::getPosition().y / 5 + y > 0 &&
                            (cells[sf::Mouse::getPosition().x / 5 + x][sf::Mouse::getPosition().y / 5 + y].state == Dead || boldChoosen == &empty))
                        cells[sf::Mouse::getPosition().x / 5 + x][sf::Mouse::getPosition().y / 5 + y] = (*boldChoosen);
        }
        if(!canMove)
            iterationNumber = oldIterationNumber;
        ////end of events
        window.clear();

        numberOfCells = 0;
        for(int y = 0; y < MAX_Y; y++) {
            for(int x = 0; x < MAX_X; x++) {
                if(cells[x][y].state == Dead)
                    continue;
                else
                    numberOfCells++;

                if(y + 1 == MAX_Y) {
                    cells[x][y] = empty;
                    continue;
                }

                if(cells[x][y].state == Liquid) {
                    grain.setFillColor(sf::Color::Blue);

                    if(y + 1 < MAX_Y && cells[x][y].lastMoved + (rand() % 2) + 5 < iterationNumber && canMove) {
                        if(cells[x][y + 1].state == Dead) {
                            MOVECELL(x, y + 1);
                            continue;
                        }
                        else if(x + 1 < MAX_X && cells[x + 1][y].state == Dead && rand() % 2 == 0) {
                            MOVECELL(x + 1, y);
                            continue;
                        }
                        else if(x - 1 >= 0 && cells[x - 1][y].state == Dead && rand() % 2 == 0) {
                            MOVECELL(x - 1, y);
                            continue;
                        }
                    }
                }
                else if(cells[x][y].state == Sand) {
                    grain.setFillColor(sf::Color::Yellow);

                    if(y + 1 < MAX_Y && cells[x][y].lastMoved + 8 < iterationNumber && canMove) {
                        if(cells[x][y + 1].state == Dead) {
                            MOVECELL(x, y + 1);
                            continue;
                        }
                        else if(x + 1 < MAX_X && cells[x + 1][y + 1].state == Dead && cells[x + 1][y].state == Dead && rand() % 2 == 0) {
                            MOVECELL(x + 1, y + 1);
                            continue;
                        }
                        else if(x - 1 >= 0 && cells[x - 1][y + 1].state == Dead && cells[x - 1][y].state == Dead && rand() % 2 == 0) {
                            MOVECELL(x - 1, y + 1);
                            continue;
                        }
                    }
                }
                else if(cells[x][y].state == Gas) {
                    grain.setFillColor(sf::Color::Green);

                    if(cells[x][y].lastMoved + (rand() % 20) + 3 < iterationNumber && canMove) {
                        switch((rand() % 100) / 25) {
                        case 0:
                            if(x + 1 < MAX_X && cells[x + 1][y].state == Dead) {
                                MOVECELL(x + 1, y);
                                continue;
                            }
                            break;
                        case 1:
                            if(x - 1 > 0 && cells[x - 1][y].state == Dead) {
                                MOVECELL(x - 1, y);
                                continue;
                            }
                            break;
                        case 2:
                            if(y + 1 < MAX_Y && cells[x][y + 1].state == Dead) {
                                MOVECELL(x, y + 1);
                                continue;
                            }
                            break;
                        case 3:
                            if(y - 1 > 0) {
                                //Gas must be above liquid
                                if(cells[x][y - 1].state == Dead) {
                                    MOVECELL(x, y - 1);
                                    continue;
                                }
                                else if(cells[x][y - 1].state == Liquid || cells[x][y - 1].state == Sand) {
                                    std::swap(cells[x][y], cells[x][y - 1]);
                                    continue;
                                }
                            }
                        }
                    }
                }
                else if(cells[x][y].state == Source) {
                    grain.setFillColor(sf::Color::Cyan);
                    if(cells[x][y].lastMoved + 1 < iterationNumber && canMove) {
                        if(y + 2 < MAX_Y && y - 1 > 0 && cells[x][y + 1].state == Dead) {
                            int i = 1;
                            for(; cells[x][y - i].state == Source && y - i > 1; i++) {

                            }
                            cells[x][y + 1].state = cells[x][y - i].state;
                            cells[x][y + 1].lastMoved = iterationNumber;
                        }
                    }
                }
                else //solid
                    grain.setFillColor(sf::Color::White);

                grain.setPosition(x * 5, y * 5);
                window.draw(grain);
            }
        }

        text.setString("Iteration: " + std::to_string(iterationNumber) + "\tNumber of live cells: " + std::to_string(numberOfCells));
        text.setPosition(0, text.getLocalBounds().height);
        window.draw(text);
        window.display();
        if(nextFrame) {
            canMove = false;
            oldIterationNumber = iterationNumber;
            nextFrame = false;
        }
    }

    for(int x = 0; x < MAX_X; x++)
        delete [] cells[x];
    delete [] cells;

    return 0;
}

#undef SWAPCELLS