#include <iostream>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

#define FIELD_CELL_TYPE_NONE 0
#define FIELD_CELL_TYPE_BONUS -1
#define FIELD_CELL_TYPE_TARGET -2
#define FIELD_CELL_TYPE_WALL -3

#define SQUARE_DIRECTION_UP  0
#define SQUARE_DIRECTION_RIGHT  1
#define SQUARE_DIRECTION_DOWN  2
#define SQUARE_DIRECTION_LEFT  3

class Square;
class Square_plus;
class Game_manager{
public:
    Game_manager(int cnt = 3)
    {
        bonus_cnt = cnt;
    }
    static bool game_over;
    int bonus_cnt;
    void handle_keyboard(Square_plus &square);
};

Game_manager & operator >> (Game_manager & manager, int formal){
    Game_manager::game_over = true;
    return manager;
}

Game_manager operator + (Game_manager & manager_1, Game_manager & manager_2)
{
    return Game_manager(manager_1.bonus_cnt + manager_2.bonus_cnt);
}

bool Game_manager::game_over = false;

class Map{
public:
    friend class Square;
    friend class Square_plus;
    Map()
    {
        cell_size = 32;
        window_width = field_size_x * cell_size;
        window_height = field_size_y * cell_size;
        field = new int *[field_size_y];
        for (int i = 0; i < field_size_y; i++)
            field[i] = new int [field_size_x];
    }
    Map(int cell_size)
    {
        this->cell_size = cell_size;
        window_width = field_size_x * cell_size;
        window_height = field_size_y * cell_size;
        field = new int *[field_size_y];
        for (int i = 0; i < field_size_y; i++)
            field[i] = new int [field_size_x];
    }
    virtual ~Map()
    {
        for (int i = 0; i < field_size_y; i++) {
            delete [] field[i];
        }
        delete [] field;
    }
    int get_empty_cell();
    void add_bonus(int cnt = 3);
    void add_target();
    void add_wall(int cnt = 3);
    void clear_field(Square_plus &square);
    void draw_field(sf::RenderWindow &window);
    void make_move(Square_plus &square, Game_manager &game_manager);
    static int get_field_size_x()
    {
        return field_size_x;
    }
    static int get_field_size_y()
    {
        return field_size_y;
    }
    int get_window_width()
    {
        return window_width;
    }
    int get_window_height()
    {
        return window_height;
    }
private:
    static int field_size_x;
    static int field_size_y;
    int cell_size;
    int window_width;
    int window_height;
    int ** field;
};

class Square {
public:
    friend class Map;
    Square()
    {
        square_position_x = Map::field_size_x / 2;
        square_position_y = Map::field_size_y / 2;
        square_direction = SQUARE_DIRECTION_RIGHT;
    }
    int square_position_x;
    int square_position_y;
    int square_direction;
    virtual void respawn()
    {
        square_position_x = Map::field_size_x / 2;
        square_position_y = Map::field_size_y / 2;
    }
    virtual ~Square(){}
};

class Square_plus : public Square{
public:
    int life_cnt;
    Square_plus(int cnt = 2)
    {
        square_position_x = Map::field_size_x / 2;
        square_position_y = Map::field_size_y / 2;
        square_direction = SQUARE_DIRECTION_RIGHT;
        life_cnt = cnt;
    }
    void respawn()
    {
        square_position_x = 0;
        square_position_y = 0;
    }
    Square_plus & operator ++ ()
    {
        life_cnt++;
        return *this;
    }
    Square_plus & operator -- ()
    {
        life_cnt--;
        return *this;
    }
    Square_plus operator ++ (int)
    {
        Square_plus prev = *this;
        ++*this;
        return prev;
    }
    Square_plus operator -- (int)
    {
        Square_plus prev = *this;
        --*this;
        return prev;
    }
};

int Map :: field_size_x = 35;
int Map :: field_size_y = 25;

int Map::get_empty_cell()
{
    int cnt = 0;
    for (int j = 0; j < field_size_y; j++)
        for (int i = 0; i < field_size_x; i++)
            if (field[j][i] == FIELD_CELL_TYPE_NONE)
                cnt++;
    int index = rand()%cnt;
    int cur_index = 0;
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] == FIELD_CELL_TYPE_NONE) {
                if (cur_index == index) {
                    return j * field_size_x + i;
                }
                cur_index++;
            }
        }

    }
    throw("couldn't find empty cell");
}

void Map::add_bonus(int cnt)
{
    while(cnt--)
    {
        int pos = get_empty_cell();
        field[pos / field_size_x][pos % field_size_x] = FIELD_CELL_TYPE_BONUS;
    }
}

void Map::add_target()
{
    int pos = get_empty_cell();
    field[pos / field_size_x][pos % field_size_x] = FIELD_CELL_TYPE_TARGET;
}

void Map::add_wall(int cnt)
{
    while(cnt--)
    {
        int pos = get_empty_cell();
        field[pos / field_size_x][pos % field_size_x] = FIELD_CELL_TYPE_WALL;
    }

}

void Map::clear_field(Square_plus &square)
{
    for (int j = 0; j < field_size_y; j++)
        for (int i = 0; i < field_size_x; i++)
            field[j][i] = FIELD_CELL_TYPE_NONE;
    field[square.square_position_y][square.square_position_x] = 1;
    add_bonus();
    add_target();
    add_wall();
}

void Map::draw_field(sf::RenderWindow &window)
{
    sf::Texture none_texture;
    none_texture.loadFromFile("images/none.png");
    sf::Sprite none;
    none.setTexture(none_texture);

    sf::Texture square_texture;
    square_texture.loadFromFile("images/square.png");
    sf::Sprite square;
    square.setTexture(square_texture);

    sf::Texture bonus_texture;
    bonus_texture.loadFromFile("images/bonus.png");
    sf::Sprite bonus;
    bonus.setTexture(bonus_texture);

    sf::Texture target_texture;
    target_texture.loadFromFile("images/target.png");
    sf::Sprite target;
    target.setTexture(target_texture);

    sf::Texture wall_texture;
    wall_texture.loadFromFile("images/wall.png");
    sf::Sprite wall;
    wall.setTexture(wall_texture);

    target.setTexture(target_texture);
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            switch (field[j][i]) {
                case FIELD_CELL_TYPE_NONE:
                    none.setPosition(i * cell_size,  j * cell_size);
                    window.draw(none);
                    break;
                case FIELD_CELL_TYPE_BONUS:
                    bonus.setPosition(i * cell_size,  j * cell_size);
                    window.draw(bonus);
                    break;
                case FIELD_CELL_TYPE_TARGET:
                    target.setPosition(i * cell_size,  j * cell_size);
                    window.draw(target);
                    break;
                case FIELD_CELL_TYPE_WALL:
                    wall.setPosition(i * cell_size,  j * cell_size);
                    window.draw(wall);
                    break;
                default:
                    square.setPosition(i * cell_size, j * cell_size);
                    window.draw(square);
            }
        }
    }
}

void Map::make_move(Square_plus &square, Game_manager &game_manager)
{
    field[square.square_position_y][square.square_position_x] = 0;
    switch (square.square_direction){
        case SQUARE_DIRECTION_UP:
            square.square_position_y--;
            if (square.square_position_y < 0){
                square.square_position_y = field_size_y - 1;
            }
            break;
        case SQUARE_DIRECTION_RIGHT:
            square.square_position_x++;
            if (square.square_position_x > field_size_x - 1) {
                square.square_position_x = 0;
            }
            break;
        case SQUARE_DIRECTION_DOWN:
            square.square_position_y++;
            if (square.square_position_y > field_size_y - 1) {
                square.square_position_y = 0;
            }
            break;
        case SQUARE_DIRECTION_LEFT:
            square.square_position_x--;
            if (square.square_position_x < 0){
                square.square_position_x = field_size_x - 1;
            }
            break;
    }
    if (field[square.square_position_y][square.square_position_x] != FIELD_CELL_TYPE_NONE){
        switch (field[square.square_position_y][square.square_position_x]){
            case FIELD_CELL_TYPE_BONUS:
                game_manager.bonus_cnt--;
                square++;
                break;
            case FIELD_CELL_TYPE_TARGET:
                if (game_manager.bonus_cnt == 0)
                    game_manager >> 0;
                add_target();
                break;
            case FIELD_CELL_TYPE_WALL:
                 square--;
                if (square.life_cnt == 0)
                    game_manager >> 0;
                break;
            default:
                  game_manager >> 0;
        }

    }
    field[square.square_position_y][square.square_position_x] = 1;
}

void Game_manager::handle_keyboard(Square_plus &square)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        square.square_direction = SQUARE_DIRECTION_UP;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        square.square_direction = SQUARE_DIRECTION_RIGHT;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        square.square_direction = SQUARE_DIRECTION_DOWN;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        square.square_direction = SQUARE_DIRECTION_LEFT;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        game_over = true;
}

int main()
{
    std::cout<<"Here the game starts!"<<std::endl;
    srand(time(NULL));
    Map map;
    Square_plus square;
    Game_manager game_manager;
    sf::RenderWindow window(sf::VideoMode(map.get_window_width(), map.get_window_height()), "The hardest game in the world!", sf::Style::Close);
    map.clear_field(square);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        map.make_move(square, game_manager);
        if (Game_manager::game_over)
        {
            window.close();
        }
        window.clear(sf::Color(183, 212, 168));
        map.draw_field(window);
        game_manager.handle_keyboard(square);
        window.display();
        sf::sleep(sf::milliseconds(100));
    }
    return 0;
}