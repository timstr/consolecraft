#include <array>
#include <chrono>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define PI 3.14159265358979323846264338

void restrictToRange(int width, int height, int &x, int &y){
    if (x < 0){
        x = 0;
    }
    if (x >= width){
        x = width - 1;
    }
    if (y < 0){
        y = 0;
    }
    if (y >= height){
        y = height - 1;
    }
}

bool testRange(int width, int height, int x, int y){
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

void mygotoxy(short x, short y){
    std::cout << "\033[" << y << ';' << x << 'H';
}

char mygetch() {
    char buf = 0;
    auto old = termios{};
    if (tcgetattr(0, &old) < 0) {
        throw std::runtime_error("tcgetattr failed");
    }
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) {
        throw std::runtime_error("tcsetattr failed");
    }
    if (read(0, &buf, 1) < 0) {
        throw std::runtime_error("read failed");
    }
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) {
        throw std::runtime_error("tcsetattr failed the second time");
    }
    return buf;
}

class Color {
public:
    static Color BLACK;
    static Color RED;
    static Color GREEN;
    static Color YELLOW;
    static Color BLUE;
    static Color MAGENTA;
    static Color CYAN;
    static Color WHITE;
    static Color GREY;
    static Color BRIGHT_RED;
    static Color BRIGHT_GREEN;
    static Color BRIGHT_YELLOW;
    static Color BRIGHT_BLUE;
    static Color BRIGHT_MAGENTA;
    static Color BRIGHT_CYAN;
    static Color BRIGHT_WHITE;

    std::uint8_t colorDigit() const noexcept {
        return this->m_colorDigit;
    }

    bool isBright() const noexcept {
        return this->m_isBright;
    }

private:
    Color(std::uint8_t colorDigit, bool isBright) noexcept
        : m_colorDigit(colorDigit)
        , m_isBright(isBright)
    {}

    std::uint8_t m_colorDigit;
    bool m_isBright;
};

Color Color::BLACK = Color(0, false);
Color Color::RED = Color(1, false);
Color Color::GREEN = Color(2, false);
Color Color::YELLOW = Color(3, false);
Color Color::BLUE = Color(4, false);
Color Color::MAGENTA = Color(5, false);
Color Color::CYAN = Color(6, false);
Color Color::WHITE = Color(7, false);
Color Color::GREY = Color(0, true);
Color Color::BRIGHT_RED = Color(1, true);
Color Color::BRIGHT_GREEN = Color(2, true);
Color Color::BRIGHT_YELLOW = Color(3, true);
Color Color::BRIGHT_BLUE = Color(4, true);
Color Color::BRIGHT_MAGENTA = Color(5, true);
Color Color::BRIGHT_CYAN = Color(6, true);
Color Color::BRIGHT_WHITE = Color(7, true);


void textColor(Color bg_color, Color text_color){
    auto bg_code = 40 + bg_color.colorDigit() + (bg_color.isBright() ? 60 : 0);
    auto text_code = 30 + text_color.colorDigit() + (text_color.isBright() ? 60 : 0);
    std::cout << "\033[" << bg_code << "m\033[" << text_code << 'm';
}

void clearScreen(){
    textColor(Color::BLACK, Color::WHITE);
    std::cout << "\033[2J";
}

void drawLine(int start_x, int start_y, int end_x, int end_y, char const* symbol, Color symbol_color){
    textColor(Color::BLACK, symbol_color);
    int distance = ((end_x - start_x) + (end_y - start_y));
    for (int i = 0; i <= distance; i++){
        int symbol_x = start_x + ((i * (end_x - start_x)) / distance);
        int symbol_y = start_y + ((i * (end_y - start_y)) / distance);
        mygotoxy(symbol_x, symbol_y);
        std::cout << symbol;
    }
}

void drawBox(int sx, int sy, int ex, int ey, char const* symbol, Color symbol_color, char const* title, bool empty = false){
    drawLine(sx, sy, sx, ey, symbol, symbol_color);
    drawLine(sx, sy, ex, sy, symbol, symbol_color);
    drawLine(ex, sy, ex, ey, symbol, symbol_color);
    drawLine(sx, ey, ex, ey, symbol, symbol_color);
    if (empty){
        for (int box_x = 1; box_x < ex - sx; box_x++){
            for (int box_y = 1; box_y < ey - sy; box_y++){
                mygotoxy(sx + box_x, sy + box_y);
                std::cout << " ";
            }
        }
    }
    mygotoxy(sx + 1, sy);
    std::cout << title;
}

template<size_t width, size_t height>
void generateWorld(std::array<std::array<int, height>, width>& terrain) {
    //turn everything into grass
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            terrain.at(x).at(y) = 2;
        }
    }

    //add patches of sand
    for (int i = 0; i <= 50; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 10; j++){
            restrictToRange(width, height, x, y);
            int temp_x, temp_y;
            double degrees;

            for (int size = 1; size <= rand() % 5; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        if (terrain.at(temp_x).at(temp_y) == 2){
                            terrain.at(temp_x).at(temp_y) = 3;
                        }
                    }
                }
            }

            x += rand() % 3 - 1;
            y += rand() % 3 - 1;
        }
    }

    //add large rough patches of stone
    for (int i = 0; i <= 200; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 50; j++){
            restrictToRange(width, height, x, y);
            int temp_x, temp_y;
            double degrees;

            int size = rand() % 4 + 1;

            for (int k = 0; k <= size * PI; k++){
                degrees = k * 360 / size * PI;
                temp_x = x + (size * (cos(degrees * (PI / 180))));
                temp_y = y + (size * (sin(degrees * (PI / 180))));
                if (testRange(width, height, temp_x, temp_y)){
                    if (terrain.at(temp_x).at(temp_y) == 2){
                        terrain.at(temp_x).at(temp_y) = 1;
                    }
                }
            }
            x += rand() % 3 - 1;
            y += rand() % 3 - 1;
        }
    }

    //add strains of stone
    for (int i = 0; i <= 100; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 100; j++){
            restrictToRange(width, height, x, y);
            int temp_x, temp_y;
            double degrees;

            int size = 1;

            for (int k = 0; k <= size * PI; k++){
                degrees = k * 360 / size * PI;
                temp_x = x + (size * (cos(degrees * (PI / 180))));
                temp_y = y + (size * (sin(degrees * (PI / 180))));
                if (testRange(width, height, temp_x, temp_y)){
                    if (terrain.at(temp_x).at(temp_y) == 2){
                        terrain.at(temp_x).at(temp_y) = 1;
                    }
                }
            }
            x += rand() % 5 - 2;
            y += rand() % 5 - 2;
        }
    }

    //add water bodies with sandy banks, and occaisonal plants
    for (int i = 0; i <= 20; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 2000; j++){
            if (testRange(width, height, x, y)){
                terrain.at(x).at(y) = 4;
            }

            int temp_x, temp_y;
            double degrees;

            for (int size = 1; size <= 4; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        terrain.at(temp_x).at(temp_y) = 4;
                    }
                }
            }

            for (int size = 4; size <= 8; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        if (terrain.at(temp_x).at(temp_y) == 2){
                            terrain.at(temp_x).at(temp_y) = 3;
                        }
                        if (terrain.at(temp_x).at(temp_y) == 1){
                            if (rand() % 5 == 1){
                                terrain.at(temp_x).at(temp_y) = 3;
                            }
                        }
                    }
                }
            }

            if (rand() % 10 == 1){
                int degrees = rand() % 360, size = rand () % 30;
                temp_x = x + (size * (cos(degrees * (PI / 180))));
                temp_y = y + (size * (sin(degrees * (PI / 180))));
                if (testRange(width, height, temp_x, temp_y)){
                    if (terrain.at(temp_x).at(temp_y) == 2){
                        terrain.at(temp_x).at(temp_y) = 5;
                    }
                }

            }

            x += rand() % 3 - 1;
            y += rand() % 3 - 1;
        }
    }

    //add grass and plants
    for (int i = 0; i <= 100; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 500; j++){
            restrictToRange(width, height, x, y);
            int temp_x, temp_y;
            double degrees;

            for (int size = 1; size <= rand() % 5; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        if (terrain.at(temp_x).at(temp_y) == 2){
                            if (rand() % 50 > 0){
                                terrain.at(temp_x).at(temp_y) = 6;
                            } else {
                                terrain.at(temp_x).at(temp_y) = 5;
                            }
                        }
                    }
                }
            }

            x += rand() % 5 - 2;
            y += rand() % 5 - 2;
        }
    }

    //add patches of snow and rock
    for (int i = 0; i <= 25; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= 500; j++){
            if (testRange(width, height, x, y)){
                terrain.at(x).at(y) = 7;
            }

            int temp_x, temp_y;
            double degrees;

            int radius = rand() % 3 + 1;

            for (int size = 1; size <= radius; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        terrain.at(temp_x).at(temp_y) = 7;
                    }
                }
            }

            for (int size = radius; size <= (radius * 2); size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        if (terrain.at(temp_x).at(temp_y) != 7){
                            terrain.at(temp_x).at(temp_y) = 1;
                        }
                    }
                }
            }

            for (int size = (radius * 2); size <= (radius * 2) + 4; size++){
                for (int k = 0; k <= size * PI; k++){
                    degrees = k * 360 / size * PI;
                    temp_x = x + (size * (cos(degrees * (PI / 180))));
                    temp_y = y + (size * (sin(degrees * (PI / 180))));
                    if (testRange(width, height, temp_x, temp_y)){
                        if (terrain.at(temp_x).at(temp_y) == 4){
                            terrain.at(temp_x).at(temp_y) = 3;
                        }
                    }
                }
            }

            x += rand() % 3 - 1;
            y += rand() % 3 - 1;
        }
    }

    //add pieces of ore to rock
    for (int i = 0; i <= 100; i++){
        int x = rand() % width, y = rand() % height;
        for (int j = 0; j <= (rand() % 40) + 10; j++){
            restrictToRange(width, height, x, y);
            if (terrain.at(x).at(y) == 1){
                terrain.at(x).at(y) = 8;
            }
            x += rand() % 3 - 1;
            y += rand() % 3 - 1;
        }
    }
}

void drawMenu() {
    // TODO
}

int main(){
    clearScreen();
    mygotoxy(0, 0);
    textColor(Color::GREEN, Color::BRIGHT_WHITE);
    std::cout << "                                                                                \n";
    std::cout << "                                  Welcome to                                    \n";
    std::cout << "                                                                                \n";
    textColor(Color::BLUE, Color::BRIGHT_GREEN);
    std::cout << "                                                                                \n";
    std::cout << "              ### ### #   # ### ### #   ###    ### ###  #  ### ###              \n";
    std::cout << "              #   # # ##  # #   # # #   #      #   # # # # #    #               \n";
    std::cout << "              #   # # # # # ### # # #   ##  ## #   ### ### ##   #               \n";
    std::cout << "              #   # # #  ##   # # # #   #      #   ##  # # #    #               \n";
    std::cout << "              ### ### #   # ### ### ### ###    ### # # # # #    #               \n";
    std::cout << "                                                                                \n";
    textColor(Color::GREEN, Color::BRIGHT_WHITE);
    std::cout << "                              By Tim Straubinger                                \n";
    std::cout << "                                                                                \n";

    mygotoxy(20, 15);
    textColor(Color::BLACK, Color::BRIGHT_WHITE);
    std::cout << "Enter a world seed (0 for random one): ";

    unsigned int seed;

    std::cin >> seed;

    if (seed == 0){
        seed = (unsigned)time(0);
    }
    srand(seed);

    int difficulty = 0;

    mygotoxy(25, 18);
    std::cout << "1 - Piece of cake";
    mygotoxy(25, 19);
    std::cout << "2 - Moderate";
    mygotoxy(25, 20);
    std::cout << "3 - Tricky";
    mygotoxy(25, 21);
    std::cout << "4 - Extreme";
    mygotoxy(20, 17);
    std::cout << "Please select a difficulty: ";
    while (!(difficulty >= 1 && difficulty <= 4)){
        std::cin >> difficulty;
        if (difficulty < 1 || difficulty > 4){
            drawLine(0, 17, 79, 17, " ", Color::BRIGHT_WHITE);
            mygotoxy(20, 17);
            std::cout << "Please select a VALID difficulty: ";
        }
    }

    clearScreen();


    std::cout << "\n\n\t\tCreating materials...\n";

    //define materials
    const size_t num_materials = 12;
    const std::array<std::string, num_materials> material_names = {"air", "stone", "dirt", "sand", "water", "plant", "grass", "snow", "ore", "wal", "door", "bedrock"};
    const std::array<Color, num_materials> material_bgcolors = {
        Color::BLACK, Color::GREY,         Color::RED,     Color::BRIGHT_YELLOW,   Color::BLUE,        Color::GREEN,           Color::GREEN,   Color::BRIGHT_WHITE,    Color::WHITE,   Color::WHITE,   Color::RED,     Color::WHITE};
    const std::array<Color, num_materials> material_fgcolors = {
        Color::BLACK, Color::WHITE,  Color::GREEN,   Color::BRIGHT_WHITE,    Color::BRIGHT_BLUE, Color::BRIGHT_GREEN,    Color::GREEN,   Color::BLACK,           Color::YELLOW,  Color::BLACK,   Color::WHITE,   Color::BLACK};
    const std::array<std::string, num_materials> material_chars = {u8" ", u8"░", u8"▒", u8"▒", u8"≈", u8"♣", u8" ", u8" ", u8"♦", u8"╬", u8"■", u8"░"};

    std::array<int, num_materials> inventory = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::array<int, num_materials> underlying_material = {0, 11, 1, 2, 4, 6, 2, 1, 1, 1, 2, 1};

    std::cout << "\t\tGenerating terrain...\n";

    //generate terrain
    const int width = 500, height = 500;
    std::array<std::array<int, 500>, 500> terrain;

    generateWorld(terrain);

    std::cout << "\t\tPreparing game...";

    //fixed vital gameplay variables
    char key = 'r';
    int temp_material, x_offset = width / 2 - 39, y_offset = height / 2 - 12, player_x = width / 2, player_y = height / 2, old_x = player_x, old_y = player_y, selection = 1;
    double health = 100;
    bool redraw = false;
    bool redraw_player = false;
    bool redraw_help = false;
    bool redraw_inventory = false;
    bool redraw_info = false;
    bool quit = false;
    bool attack = false;

    int num_monsters;
    //create monsters
    if (difficulty == 1){
        num_monsters = 5;
    } else if (difficulty == 2){
        num_monsters = 15;
    } else if (difficulty == 3){
        num_monsters = 50;
    } else if (difficulty == 4){
        num_monsters = 150;
    }

    std::array<std::string, 5> monster_chars = {u8"S", u8"Ω", u8"@", u8"¥", u8"Ö"};
    std::array<int, 151> monster_x;
    std::array<int, 151> monster_y;
    std::array<int, 151> monster;
    std::array<int, 151> monster_oldx;
    std::array<int, 151> monster_oldy;
    std::array<int, 151> monster_health;

    for (int i = 0; i <= num_monsters; i++){
        monster_x.at(i) = rand() % width;
        monster_y.at(i) = rand() % height;
        monster.at(i) = rand() % 5;
        monster_health.at(i) = rand() % 40 + 10;
    }

    clearScreen();

    //pre-draw the screen
    for (int x_index = 0; x_index <= 79; x_index++){
        for (int y_index = 0; y_index <= 24; y_index++){
            mygotoxy(x_index, y_index);
            temp_material = terrain.at(x_index + x_offset).at(y_index + y_offset);
            textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
            std::cout << material_chars.at(temp_material);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    //draw the inventory
    drawBox(1, 26, 23, 35, u8"░", Color::WHITE, "INVENTORY");
    for (int i = 1; i <= 8; i++){
        mygotoxy(2, 26 + i);
        if (i == selection){
            textColor(Color::BLACK, Color::BRIGHT_YELLOW);
        } else {
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
        }
        std::cout << i << " " << material_names.at(i) << " \t";
        textColor(material_bgcolors.at(i), material_fgcolors.at(i));
        std::cout << material_chars.at(i);
        textColor(Color::BLACK, Color::BRIGHT_WHITE);
        std::cout << " " << inventory.at(i) << '\n';
    }

    //draw the help box
    drawBox(25, 26, 40, 35, u8"░", Color::WHITE, "QUICK HELP");
    textColor(Color::BLACK, Color::BRIGHT_WHITE);
    mygotoxy(26, 27);
    std::cout << "w - up";
    mygotoxy(26, 28);
    std::cout << "a - left";
    mygotoxy(26, 29);
    std::cout << "s - down";
    mygotoxy(26, 30);
    std::cout << "d - right";
    mygotoxy(26, 31);
    std::cout << "m - mine";
    mygotoxy(26, 32);
    std::cout << "p - place";
    mygotoxy(26, 33);
    std::cout << "q - quit";
    mygotoxy(26, 34);
    std::cout << "h - more help";

    //draw the info box
    drawBox(42, 26, 78, 35, u8"░", Color::WHITE, "INFO");
    mygotoxy(43, 27);
    textColor(Color::BLACK, Color::BRIGHT_WHITE);
    if (terrain.at(player_x).at(player_y) == 4){
        std::cout << "You are standing in water.";
    } else if (terrain.at(player_x).at(player_y) == 5){
        std::cout << "You are standing on a plant.";
    } else {
        std::cout << "You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << ".";
    }
    mygotoxy(43, 32);
    std::cout << "Difficulty: ";
    if (difficulty == 1){
        std::cout << "Piece of cake";
    } else if (difficulty == 2){
        std::cout << "Moderate";
    } else if (difficulty == 3){
        std::cout << "Tricky";
    } else if (difficulty == 4){
        std::cout << "Extreme";
    }
    mygotoxy(43, 33);
    std::cout << "Health: " << health;
    mygotoxy(43, 34);
    std::cout << "Seed: " << seed;

    //draw monsters
    for (int i = 0; i <= num_monsters; i++){
        if (testRange(79, 24, monster_x.at(i) - x_offset, monster_y.at(i) - y_offset)){
            monster_oldx.at(i) = monster_x.at(i);
            monster_oldy.at(i) = monster_y.at(i);

            mygotoxy(monster_x.at(i) - x_offset, monster_y.at(i) - y_offset);
            textColor(material_bgcolors.at(terrain.at(monster_x.at(i)).at(monster_y.at(i))), Color::BLACK);
            std::cout << monster_chars.at(monster.at(i));
        }
    }

    //draw the player
    mygotoxy(player_x - x_offset, player_y - y_offset);
    if (terrain.at(player_x).at(player_y) == 7){
        textColor(Color::BLACK, Color::BRIGHT_WHITE);
    } else {
        textColor(material_bgcolors.at(terrain.at(player_x).at(player_y)), material_bgcolors.at(terrain.at(player_x).at(player_y)));
    }
    std::cout << u8"Å";

    //draw dividers
    drawLine(0, 25, 79, 25, u8"░", Color::BLUE);
    drawLine(0, 36, 79, 36, u8"░", Color::BLUE);
    drawLine(0, 25, 0, 36, u8"░", Color::BLUE);
    drawLine(24, 25, 24, 36, u8"░", Color::BLUE);
    drawLine(41, 25, 41, 36, u8"░", Color::BLUE);
    drawLine(79, 25, 79, 36, u8"░", Color::BLUE);

    //draw the alert to maximise the window
    drawBox(19, 5, 59, 9, u8"░", Color::WHITE, "ALERT", true);
    mygotoxy(20, 6);
    textColor(Color::BLACK, Color::BRIGHT_WHITE);
    std::cout << "Please maximize this window, or resize";
    mygotoxy(20, 7);
    std::cout << "it to fit all components of the game.";
    mygotoxy(20, 8);
    std::cout << "Press any key to begin...";

    mygotoxy(0, 0);
    mygetch();

    //game loop
    while (!quit){

        //refresh variables
        redraw = false;
        redraw_player = false;
        redraw_help = false;
        redraw_inventory = false;
        redraw_info = true;
        attack = false;

        old_x = player_x;
        old_y = player_y;

        //key actions
        if (key == 'w'){
            if (player_y > 0 && terrain.at(player_x).at(player_y - 1) != 9){
                if (terrain.at(player_x).at(player_y - 1) == 10){
                    if (testRange(width, height, player_x, player_y - 2)){
                        if (terrain.at(player_x).at(player_y - 2) != 10){
                            player_y -= 2;
                            redraw_player = true;
                        }
                    }
                } else {
                    player_y -= 1;
                    redraw_player = true;
                }
            }
        }
        if (key == 'a'){
            if (player_x > 0 && terrain.at(player_x - 1).at(player_y) != 9){
                if (terrain.at(player_x - 1).at(player_y) == 10){
                    if (testRange(width, height, player_x - 2, player_y)){
                        if (terrain.at(player_x - 2).at(player_y) != 10){
                            player_x -= 2;
                            redraw_player = true;
                        }
                    }
                } else {
                    player_x -= 1;
                    redraw_player = true;
                }
            }
        }
        if (key == 's'){
            if (player_y < 499 && terrain.at(player_x).at(player_y + 1) != 9){
                if (terrain.at(player_x).at(player_y + 1) == 10){
                    if (testRange(width, height, player_x, player_y + 2)){
                        if (terrain.at(player_x).at(player_y + 2) != 10){
                            player_y += 2;
                            redraw_player = true;
                        }
                    }
                } else {
                    player_y += 1;
                    redraw_player = true;
                }
            }
        }
        if (key == 'd'){
            if (player_y < 499 && terrain.at(player_x + 1).at(player_y) != 9){
                if (terrain.at(player_x + 1).at(player_y) == 10){
                    if (testRange(width, height, player_x + 2, player_y)){
                        if (terrain.at(player_x + 2).at(player_y) != 10){
                            player_x += 2;
                            redraw_player = true;
                        }
                    }
                } else {
                    player_x += 1;
                    redraw_player = true;
                }
            }
        }
        if (key == 'r'){
            redraw = true;
            redraw_player = true;
            redraw_help = true;
            redraw_inventory = true;
        }
        if (key == 'm'){
            mygotoxy(43, 28);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            std::cout << "Select a direction to mine...";
            if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
                mygotoxy(player_x - x_offset - 1, player_y - y_offset);
                textColor(material_bgcolors.at(terrain.at(player_x - 1).at(player_y)), Color::BLACK);
                std::cout << u8"◄";
            }
            if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
                mygotoxy(player_x - x_offset, player_y - y_offset - 1);
                textColor(material_bgcolors.at(terrain.at(player_x).at(player_y - 1)), Color::BLACK);
                std::cout << u8"▲";
            }
            if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
                mygotoxy(player_x - x_offset + 1, player_y - y_offset);
                textColor(material_bgcolors.at(terrain.at(player_x + 1).at(player_y)), Color::BLACK);
                std::cout << u8"►";
            }
            if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
                mygotoxy(player_x - x_offset, player_y - y_offset + 1);
                textColor(material_bgcolors.at(terrain.at(player_x).at(player_y + 1)), Color::BLACK);
                std::cout << u8"▼";
            }
            char direction = mygetch();
            if (direction == 'w'){
                if (testRange(width, height, player_x, player_y - 1)){
                    if (terrain.at(player_x).at(player_y - 1) == 9){
                        inventory.at(1) += 1;
                    } else if (terrain.at(player_x).at(player_y - 1) == 10){
                        inventory.at(8) += 1;
                    } else if (terrain.at(player_x).at(player_y - 1) != 11){
                        inventory.at(terrain.at(player_x).at(player_y - 1)) += 1;
                    }
                    terrain.at(player_x).at(player_y - 1) = underlying_material.at(terrain.at(player_x).at(player_y - 1));
                    if (terrain.at(player_x).at(player_y - 1) == 11 && rand() % 1000 == 1){
                        terrain.at(player_x).at(player_y - 1) = 8;
                    }
                }
            }
            if (direction == 'a'){
                if (testRange(width, height, player_x - 1, player_y)){
                    if (terrain.at(player_x - 1).at(player_y) == 9){
                        inventory.at(1) += 1;
                    } else if (terrain.at(player_x - 1).at(player_y) == 10){
                        inventory.at(8) += 1;
                    } else if (terrain.at(player_x - 1).at(player_y) != 11){
                        inventory.at(terrain.at(player_x - 1).at(player_y)) += 1;
                    }
                    terrain.at(player_x - 1).at(player_y) = underlying_material.at(terrain.at(player_x - 1).at(player_y));
                    if (terrain.at(player_x - 1).at(player_y) == 11 && rand() % 1000 == 1){
                        terrain.at(player_x - 1).at(player_y) = 8;
                    }
                }
            }
            if (direction == 's'){
                if (testRange(width, height, player_x, player_y + 1)){
                    if (terrain.at(player_x).at(player_y + 1) == 9){
                        inventory.at(1) += 1;
                    } else if (terrain.at(player_x).at(player_y + 1) == 10){
                        inventory.at(8) += 1;
                    } else  if (terrain.at(player_x).at(player_y + 1) != 11){
                        inventory.at(terrain.at(player_x).at(player_y + 1)) += 1;
                    }
                    terrain.at(player_x).at(player_y + 1) = underlying_material.at(terrain.at(player_x).at(player_y + 1));
                    if (terrain.at(player_x).at(player_y + 1) == 11 && rand() % 1000 == 1){
                        terrain.at(player_x).at(player_y + 1) = 8;
                    }
                }
            }
            if (direction == 'd'){
                if (testRange(width, height, player_x + 1, player_y)){
                    if (terrain.at(player_x + 1).at(player_y) == 9){
                        inventory.at(1) += 1;
                    } else if (terrain.at(player_x + 1).at(player_y) == 10){
                        inventory.at(8) += 1;
                    } else  if (terrain.at(player_x + 1).at(player_y) != 11){
                        inventory.at(terrain.at(player_x + 1).at(player_y)) += 1;
                    }
                    terrain.at(player_x + 1).at(player_y) = underlying_material.at(terrain.at(player_x + 1).at(player_y));
                    if (terrain.at(player_x + 1).at(player_y) == 11 && rand() % 1000 == 1){
                        terrain.at(player_x + 1).at(player_y) = 8;
                    }
                }
            }

            if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
                mygotoxy(player_x - x_offset - 1, player_y - y_offset);
                temp_material = terrain.at(player_x - 1).at(player_y);
                textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                std::cout << material_chars.at(temp_material);
            }
            if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
                mygotoxy(player_x - x_offset, player_y - y_offset - 1);
                temp_material = terrain.at(player_x).at(player_y - 1);
                textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                std::cout << material_chars.at(temp_material);
            }
            if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
                mygotoxy(player_x - x_offset + 1, player_y - y_offset);
                temp_material = terrain.at(player_x + 1).at(player_y);
                textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                std::cout << material_chars.at(temp_material);
            }
            if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
                mygotoxy(player_x - x_offset, player_y - y_offset + 1);
                temp_material = terrain.at(player_x).at(player_y + 1);
                textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                std::cout << material_chars.at(temp_material);
            }

            redraw_help = true;
            redraw_inventory = true;

            mygotoxy(43, 28);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            std::cout << "                              ";
        }
        if (key == 'p'){
            mygotoxy(43, 28);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);

            if (inventory.at(selection) > 0){
                mygotoxy(43, 28);
                std::cout << "Select a direction to place a";
                mygotoxy(43, 29);
                std::cout << "block...";
                if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
                    mygotoxy(player_x - x_offset - 1, player_y - y_offset);
                    textColor(material_bgcolors.at(terrain.at(player_x - 1).at(player_y)), Color::BLACK);
                    std::cout << u8"◄";
                }
                if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
                    mygotoxy(player_x - x_offset, player_y - y_offset - 1);
                    textColor(material_bgcolors.at(terrain.at(player_x).at(player_y - 1)), Color::BLACK);
                    std::cout << u8"▲";
                }
                if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
                    mygotoxy(player_x - x_offset + 1, player_y - y_offset);
                    textColor(material_bgcolors.at(terrain.at(player_x + 1).at(player_y)), Color::BLACK);
                    std::cout << u8"►";
                }
                if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
                    mygotoxy(player_x - x_offset, player_y - y_offset + 1);
                    textColor(material_bgcolors.at(terrain.at(player_x).at(player_y + 1)), Color::BLACK);
                    std::cout << u8"▼";
                }

                char direction = mygetch();

                if (direction == 'w'){
                    if (testRange(width, height, player_x, player_y - 1)){
                        inventory.at(selection) -= 1;
                        if (terrain.at(player_x).at(player_y - 1) == 1 && selection == 1){
                            terrain.at(player_x).at(player_y - 1) = 9;
                        } else if (terrain.at(player_x).at(player_y - 1) == 2 && selection == 8){
                            terrain.at(player_x).at(player_y - 1) = 10;
                        } else {
                            terrain.at(player_x).at(player_y - 1) = selection;
                        }
                    }
                }
                if (direction == 'a'){
                    if (testRange(width, height, player_x - 1, player_y)){
                        inventory.at(selection) -= 1;
                        if (terrain.at(player_x - 1).at(player_y) == 1 && selection == 1){
                            terrain.at(player_x - 1).at(player_y) = 9;
                        } else if (terrain.at(player_x - 1).at(player_y) == 2 && selection == 8){
                            terrain.at(player_x - 1).at(player_y) = 10;
                        } else {
                            terrain.at(player_x - 1).at(player_y) = selection;
                        }
                    }
                }
                if (direction == 's'){
                    if (testRange(width, height, player_x, player_y + 1)){
                        inventory.at(selection) -= 1;
                        if (terrain.at(player_x).at(player_y + 1) == 1 && selection == 1){
                            terrain.at(player_x).at(player_y + 1) = 9;
                        } else if (terrain.at(player_x).at(player_y + 1) == 2 && selection == 8){
                            terrain.at(player_x).at(player_y + 1) = 10;
                        } else {
                            terrain.at(player_x).at(player_y + 1) = selection;
                        }
                    }
                }
                if (direction == 'd'){
                    if (testRange(width, height, player_x + 1, player_y)){
                        inventory.at(selection) -= 1;
                        if (terrain.at(player_x + 1).at(player_y) == 1 && selection == 1){
                            terrain.at(player_x + 1).at(player_y) = 9;
                        } else if (terrain.at(player_x + 1).at(player_y) == 2 && selection == 8){
                            terrain.at(player_x + 1).at(player_y) = 10;
                        } else {
                            terrain.at(player_x + 1).at(player_y) = selection;
                        }
                    }
                }

                if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
                    mygotoxy(player_x - x_offset - 1, player_y - y_offset);
                    temp_material = terrain.at(player_x - 1).at(player_y);
                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                    std::cout << material_chars.at(temp_material);
                }
                if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
                    mygotoxy(player_x - x_offset, player_y - y_offset - 1);
                    temp_material = terrain.at(player_x).at(player_y - 1);
                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                    std::cout << material_chars.at(temp_material);
                }
                if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
                    mygotoxy(player_x - x_offset + 1, player_y - y_offset);
                    temp_material = terrain.at(player_x + 1).at(player_y);
                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                    std::cout << material_chars.at(temp_material);
                }
                if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
                    mygotoxy(player_x - x_offset, player_y - y_offset + 1);
                    temp_material = terrain.at(player_x).at(player_y + 1);
                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                    std::cout << material_chars.at(temp_material);
                }

                redraw_help = true;
                redraw_inventory = true;

            }

            mygotoxy(43, 28);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            std::cout << "                              ";
            mygotoxy(43, 29);
            std::cout << "                              ";
            mygotoxy(43, 30);
            std::cout << "                              ";
        }
        if (key == 'h'){
            drawBox(1, 1, 78, 23, u8"░", Color::WHITE, "HELP", true);

            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            mygotoxy(3, 3);
            std::cout << "Moving Around - use the keys 'w' (up), 'a' (left), 's' (down) and 'd'";
            mygotoxy(3, 4);
            std::cout << "     (right) to move the player around.";
            mygotoxy(3, 5);
            std::cout << "Mining - press 'm' to mine, then select a direction using 'w', 'a , 's' or";
            mygotoxy(3, 6);
            std::cout << "     'd', or press 'm' again to cancel. The material you mine will be added";
            mygotoxy(3, 7);
            std::cout << "     to your inventory.";
            mygotoxy(3, 8);
            std::cout << "Placing Blocks - press 'p' to place a selected block from your inventory,"; 
            mygotoxy(3, 9);
            std::cout << "     using the direction keys for where you want to place the block.";
            mygotoxy(3, 10);
            std::cout << "Selecting Blocks - to choose a block in your inventory that you want to";
            mygotoxy(3, 11);
            std::cout << "      place, press the corresponding number key.";
            mygotoxy(3, 12);
            std::cout << "Refreshing the Screen - if at any point you're fed up with the all the";
            mygotoxy(3, 13);
            std::cout << "     visual flaws that are ruining your gaming experience, press 'r' to";
            mygotoxy(3, 14);
            std::cout << "     refresh the game display.";
            mygotoxy(3, 15);
            std::cout << "Exiting - plain and simple, press 'q' to close the game.";
            mygotoxy(3, 16);
            std::cout << "Time Passage - time passes each time you perform and action, such as moving";
            mygotoxy(3, 17);
            std::cout << "     or mining a block. To allow time to pass while not doing anything,";
            mygotoxy(3, 18);
            std::cout << "     simply hold down any non-functional key.";
            mygotoxy(3, 19);
            std::cout << "          Press any key to continue...";

            mygotoxy(3, 21);
            std::cout << "[Hint: Try placing stone on top of stone to create walls and ore on top of";
            mygotoxy(3, 22);
            std::cout << "     dirt to create doors]";

            std::cout.flush();
            mygetch();
            redraw = true;
            redraw_player = true;
        }
        if (key > '0' && key <= '8'){
            selection = key - '0';
        }
        if (key == 'q'){
            drawBox(19, 5, 59, 9, u8"░", Color::WHITE, "ALERT", true);
            mygotoxy(20, 6);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            std::cout << "Are you sure you want to quit? (y/n) ";
            char answer = '\0';
            std::cin >> answer;
            if (answer == 'y'){
                quit = true;
            } else {
                redraw = true;
                redraw_player = true;
            }
        }
        if (key == 'x'){
            attack = true;
        }

        //stay in bounds
        restrictToRange(width, height, player_x, player_y);

        //reposition the view
        if (player_x - 39 - x_offset > 31 && player_x <= width - 9){
            redraw = true;
            x_offset += 31;
        }

        if (player_x - 39 - x_offset < -31 && player_x >= 9){
            redraw = true;
            x_offset -= 31;
        }

        if (player_y - 12 - y_offset > 8 && player_y <= height - 4){
            redraw = true;
            y_offset += 8;
        }

        if (player_y - 12 - y_offset < -8 && player_y >= 4){
            redraw = true;
            y_offset -= 8;
        }

        restrictToRange(width - 79, height - 24, x_offset, y_offset);

        //make grass grow
        for (int ax = x_offset; ax <= x_offset + 79; ax++){
            for (int ay = y_offset; ay <= y_offset + 24; ay++){
                if (terrain.at(ax).at(ay) == 6){
                    if (rand() % 500 == 0){

                        int temp_x, temp_y;
                        double degrees;

                        int size = 2;

                        for (int k = 0; k <= PI; k++){
                            degrees = k * 360 / size * PI;
                            temp_x = ax + (size * (cos(degrees * (PI / 180))));
                            temp_y = ay + (size * (sin(degrees * (PI / 180))));
                            if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                                if (terrain.at(temp_x).at(temp_y) == 2){
                                    terrain.at(temp_x).at(temp_y) = 6;
                                    mygotoxy(temp_x - x_offset, temp_y - y_offset);
                                    temp_material = terrain.at(temp_x).at(temp_y);
                                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                                    std::cout << material_chars.at(temp_material);
                                    if (temp_x == player_x && temp_y == player_y){
                                        redraw_player = true;
                                        redraw_info = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //make grass turn rock into soil
        for (int ax = x_offset; ax <= x_offset + 79; ax++){
            for (int ay = y_offset; ay <= y_offset + 24; ay++){
                if (terrain.at(ax).at(ay) == 6){
                    if (rand() % 500 == 0){

                        int temp_x, temp_y;
                        double degrees;

                        int size = 2;

                        for (int k = 0; k <= PI; k++){
                            degrees = k * 360 / size * PI;
                            temp_x = ax + (size * (cos(degrees * (PI / 180))));
                            temp_y = ay + (size * (sin(degrees * (PI / 180))));
                            if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                                if (terrain.at(temp_x).at(temp_y) == 1){
                                    terrain.at(temp_x).at(temp_y) = 2;
                                    mygotoxy(temp_x - x_offset, temp_y - y_offset);
                                    temp_material = terrain.at(temp_x).at(temp_y);
                                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                                    std::cout << material_chars.at(temp_material);
                                    if (temp_x == player_x && temp_y == player_y){
                                        redraw_player = true;
                                        redraw_info = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //make water turn soil into sand
        for (int ax = x_offset; ax <= x_offset + 79; ax++){
            for (int ay = y_offset; ay <= y_offset + 24; ay++){
                if (terrain.at(ax).at(ay) == 4){
                    if (rand() % 100 == 0){

                        int temp_x, temp_y;
                        double degrees;

                        int size = 1;

                        for (int k = 0; k <= PI; k++){
                            degrees = k * 360 / size * PI;
                            temp_x = ax + (size * (cos(degrees * (PI / 180))));
                            temp_y = ay + (size * (sin(degrees * (PI / 180))));
                            if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                                if (terrain.at(temp_x).at(temp_y) == 2){
                                    terrain.at(temp_x).at(temp_y) = 3;
                                    mygotoxy(temp_x - x_offset, temp_y - y_offset);
                                    temp_material = terrain.at(temp_x).at(temp_y);
                                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                                    std::cout << material_chars.at(temp_material);
                                    if (temp_x == player_x && temp_y == player_y){
                                        redraw_player = true;
                                        redraw_info = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //make plants spread
        for (int ax = x_offset; ax <= x_offset + 79; ax++){
            for (int ay = y_offset; ay <= y_offset + 24; ay++){
                if (terrain.at(ax).at(ay) == 5){
                    if (rand() % 500 == 0){

                        int temp_x, temp_y;
                        double degrees;

                        int size = 6;

                        degrees = (rand() % 6) * 360 / size * PI;
                        temp_x = ax + (size * (cos(degrees * (PI / 180))));
                        temp_y = ay + (size * (sin(degrees * (PI / 180))));
                        if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                            if (terrain.at(temp_x).at(temp_y) == 2 || terrain.at(temp_x).at(temp_y) == 6){
                                terrain.at(temp_x).at(temp_y) = 5;
                                mygotoxy(temp_x - x_offset, temp_y - y_offset);
                                temp_material = terrain.at(temp_x).at(temp_y);
                                textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                                std::cout << material_chars.at(temp_material);
                                if (temp_x == player_x && temp_y == player_y){
                                    redraw_player = true;
                                    redraw_info = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        //make water flow
        for (int ax = x_offset; ax <= x_offset + 79; ax++){
            for (int ay = y_offset; ay <= y_offset + 24; ay++){
                if (terrain.at(ax).at(ay) == 4 && (rand() % 2) == 1){
                    int temp_x, temp_y;

                    temp_x = ax - 1;
                    temp_y = ay;

                    if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                        if (terrain.at(temp_x).at(temp_y) == 11){
                            terrain.at(temp_x).at(temp_y) = 4;
                            mygotoxy(temp_x - x_offset, temp_y - y_offset);
                            temp_material = terrain.at(temp_x).at(temp_y);
                            textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                            std::cout << material_chars.at(temp_material);
                            if (temp_x == player_x && temp_y == player_y){
                                redraw_player = true;
                                redraw_info = true;
                            }
                        }
                    }

                    temp_x = ax;
                    temp_y = ay - 1;

                    if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                        if (terrain.at(temp_x).at(temp_y) == 11){
                            terrain.at(temp_x).at(temp_y) = 4;
                            mygotoxy(temp_x - x_offset, temp_y - y_offset);
                            temp_material = terrain.at(temp_x).at(temp_y);
                            textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                            std::cout << material_chars.at(temp_material);
                            if (temp_x == player_x && temp_y == player_y){
                                redraw_player = true;
                                redraw_info = true;
                            }
                        }
                    }

                    temp_x = ax + 1;
                    temp_y = ay;

                    if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                        if (terrain.at(temp_x).at(temp_y) == 11){
                            terrain.at(temp_x).at(temp_y) = 4;
                            mygotoxy(temp_x - x_offset, temp_y - y_offset);
                            temp_material = terrain.at(temp_x).at(temp_y);
                            textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                            std::cout << material_chars.at(temp_material);
                            if (temp_x == player_x && temp_y == player_y){
                                redraw_player = true;
                                redraw_info = true;
                            }
                        }
                    }

                    temp_x = ax;
                    temp_y = ay + 1;

                    if (testRange(79, 24, temp_x - x_offset, temp_y - y_offset)){
                        if (terrain.at(temp_x).at(temp_y) == 11){
                            terrain.at(temp_x).at(temp_y) = 4;
                            mygotoxy(temp_x - x_offset, temp_y - y_offset);
                            temp_material = terrain.at(temp_x).at(temp_y);
                            textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                            std::cout << material_chars.at(temp_material);
                            if (temp_x == player_x && temp_y == player_y){
                                redraw_player = true;
                                redraw_info = true;
                            }
                        }
                    }
                }
            }
        }

        //redraw whole screen
        if (redraw){
            clearScreen();
            for (int x_index = 0; x_index <= 79; x_index++){
                for (int y_index = 0; y_index <= 24; y_index++){
                    mygotoxy(x_index, y_index);
                    temp_material = terrain.at(x_index + x_offset).at(y_index + y_offset);
                    textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
                    std::cout << material_chars.at(temp_material);
                }
            }
        }

        //draw attack
        if (attack){
            if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset - 1)){
                //mygotoxy(player_x -
            }
        }

        //monsters
        for (int i = 0; i <= num_monsters; i++){
            if (testRange(79, 24, monster_x.at(i) - x_offset, monster_y.at(i) - y_offset)){
                monster_oldx.at(i) = monster_x.at(i);
                monster_oldy.at(i) = monster_y.at(i);

                if (monster_x.at(i) < player_x - 1){
                    monster_x.at(i) += 1;
                }
                if (monster_x.at(i) > player_x + 1){
                    monster_x.at(i) -= 1;
                }

                if (monster_y.at(i) < player_y - 1){
                    monster_y.at(i) += 1;
                }
                if (monster_y.at(i) > player_y + 1){
                    monster_y.at(i) -= 1;
                }

                if (monster_x.at(i) == player_x - 1 || monster_x.at(i) == player_x + 1 || monster_y.at(i) == player_y - 1 || monster_y.at(i) == player_y+ 1){
                    health -= 0.1;
                }

                if (attack){
                    if (monster_x.at(i) >= player_x - 5 && monster_x.at(i) <= player_x + 5 && monster_y.at(i) >= player_y - 5 && monster_y.at(i) <= player_y + 5){
                        monster_health.at(i) -= 1;
                    }
                }

                if (monster_health.at(i) <= 0){
                    monster_x.at(i) = rand() % width;
                    monster_y.at(i) = rand() % height;
                    monster_health.at(i) = rand() % 20 + 10;
                }

                if (monster_x.at(i) != monster_oldx.at(i) || monster_y.at(i) != monster_oldy.at(i)){
                    if (testRange(79, 24, monster_oldx.at(i) - x_offset, monster_oldy.at(i) - y_offset)){
                        mygotoxy(monster_oldx.at(i) - x_offset, monster_oldy.at(i) - y_offset);
                        textColor(material_bgcolors.at(terrain.at(monster_oldx.at(i)).at(monster_oldy.at(i))), material_fgcolors.at(terrain.at(monster_oldx.at(i)).at(monster_oldy.at(i))));
                        std::cout << material_chars.at(terrain.at(monster_oldx.at(i)).at(monster_oldy.at(i)));
                    }
                }

                if (testRange(79, 24, monster_x.at(i) - x_offset, monster_y.at(i) - y_offset)){
                    mygotoxy(monster_x.at(i) - x_offset, monster_y.at(i) - y_offset);
                    textColor(material_bgcolors.at(terrain.at(monster_x.at(i)).at(monster_y.at(i))), Color::BLACK);
                    std::cout << monster_chars.at(monster.at(i));
                }
            }
        }

        //redraw player
        if (redraw_player){
            mygotoxy(player_x - x_offset, player_y - y_offset);
            if (terrain.at(player_x).at(player_y) == 7){
                textColor(Color::BLACK, Color::BRIGHT_WHITE);
            } else {
                textColor(material_bgcolors.at(terrain.at(player_x).at(player_y)), material_bgcolors.at(terrain.at(player_x).at(player_y)));
            }
            std::cout << u8"Å";
        }

        //redraw help
        if (redraw_help){
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            mygotoxy(26, 27);
            std::cout << "w - up";
            mygotoxy(26, 28);
            std::cout << "a - left";
            mygotoxy(26, 29);
            std::cout << "s - down";
            mygotoxy(26, 30);
            std::cout << "d - right";
            mygotoxy(26, 31);
            std::cout << "m - mine";
            mygotoxy(26, 32);
            std::cout << "p - place";
            mygotoxy(26, 33);
            std::cout << "q - quit";
            mygotoxy(26, 34);
            std::cout << "h - more help";
        }

        //redraw inventory
        for (int i = 1; i <= 8; i++){
            mygotoxy(2, 26 + i);
            if (i == selection){
                textColor(Color::BLACK, Color::BRIGHT_YELLOW);
            } else {
                textColor(Color::BLACK, Color::BRIGHT_WHITE);
            }
            std::cout << i << " " << material_names.at(i) << " \t";
            textColor(material_bgcolors.at(i), material_fgcolors.at(i));
            std::cout << material_chars.at(i);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            std::cout << " " << inventory.at(i) << " ";
        }

        //redraw info box
        if (redraw_info){
            mygotoxy(43, 27);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            if (terrain.at(player_x).at(player_y) == 4){
                std::cout << "You are standing in water.";
            } else if (terrain.at(player_x).at(player_y) == 5){
                std::cout << "You are standing on a plant.";
            } else {
                std::cout << "You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << ".";
            }
            mygotoxy(43, 28);
            mygotoxy(43, 32);
            std::cout << "Difficulty: ";
            if (difficulty == 1){
                std::cout << "Piece of cake";
            } else if (difficulty == 2){
                std::cout << "Moderate";
            } else if (difficulty == 3){
                std::cout << "Tricky";
            } else if (difficulty == 4){
                std::cout << "Extreme";
            }
            mygotoxy(43, 33);
            std::cout << "Health: " << health << "   ";
        }

        //cover up the player's tracks, add a delay if in water and refresh the info 
        if (player_x != old_x || player_y != old_y){
            mygotoxy(old_x - x_offset, old_y - y_offset);
            temp_material = terrain.at(old_x).at(old_y);
            textColor(material_bgcolors.at(temp_material), 
                material_fgcolors.at(temp_material));
            std::cout << material_chars.at(temp_material);

            if (terrain.at(player_x).at(player_y) == 4){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            mygotoxy(43, 27);
            textColor(Color::BLACK, Color::BRIGHT_WHITE);
            if (terrain.at(player_x).at(player_y) == 4){
                std::cout << "You are standing in water.     ";
            } else if (terrain.at(player_x).at(player_y) == 5){
                std::cout << "You are standing on a plant.     ";
            } else {
                std::cout << "You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << ".     ";
            }
        }

        mygotoxy(0, 0);
        textColor(Color::BLACK, Color::BRIGHT_WHITE);

        if (!quit){
            std::cout.flush();
            key = mygetch();
        }
    }

    return 0;
}