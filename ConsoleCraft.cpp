#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <Windows.h>
#include <conio.h>
#include <random>
#include <ctime>
#include <math.h>
#include <io.h>
#include <fcntl.h>

#define PI 3.14159265358979323846264338

void restrictToRange(int width, int height, int &x, int &y){
	if (x < 0){
		x = 0;
	}
	if (x > width){
		x = width;
	}
	if (y < 0){
		y = 0;
	}
	if (y > height){
		y = height;
	}
}

bool testRange(int width, int height, int x, int y){
	return (x >= 0) && (x <= width) && (y >= 0) && (y <= height);
}

void mygotoxy(short x, short y){
	if (!SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{x, y})){
		throw std::runtime_error("Why????");
	}
}

void textColor(int bg_color, int text_color){
	if (!SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bg_color * 15) + text_color)){
		throw std::runtime_error("Why????");
	}
}

void clearScreen(){
	COORD topLeft  = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

void drawLine(int start_x, int start_y, int end_x, int end_y, wchar_t symbol, int symbol_color){
	textColor(0, symbol_color);
	int distance = ((end_x - start_x) + (end_y - start_y));
	for (int i = 0; i <= distance; i++){
		int symbol_x = start_x + ((i * (end_x - start_x)) / distance);
		int symbol_y = start_y + ((i * (end_y - start_y)) / distance);
		mygotoxy(symbol_x, symbol_y);
		std::wcout << symbol;
	}
}

void drawBox(int sx, int sy, int ex, int ey, wchar_t symbol, int symbol_color, const std::wstring& title, bool empty = false){
	drawLine(sx, sy, sx, ey, symbol, symbol_color);
	drawLine(sx, sy, ex, sy, symbol, symbol_color);
	drawLine(ex, sy, ex, ey, symbol, symbol_color);
	drawLine(sx, ey, ex, ey, symbol, symbol_color);
	if (empty){
		for (int box_x = 1; box_x < ex - sx; box_x++){
			for (int box_y = 1; box_y < ey - sy; box_y++){
				mygotoxy(sx + box_x, sy + box_y);
				std::wcout << L" ";
			}
		}
	}
	mygotoxy(sx + 1, sy);
	std::wcout << title;
}

int main(){
	_setmode(_fileno(stdout), _O_U16TEXT);

	textColor(2, 16);
	std::wcout << L"                                                                                ";
	std::wcout << L"                                  Welcome to                                    ";
	std::wcout << L"                                                                                ";
	textColor(1, 11);
	std::wcout << L"                                                                                ";
	std::wcout << L"              ### ### #   # ### ### #   ###    ### ###  #  ### ###              ";
	std::wcout << L"              #   # # ##  # #   # # #   #      #   # # # # #    #               ";
	std::wcout << L"              #   # # # # # ### # # #   ##  ## #   ### ### ##   #               ";
	std::wcout << L"              #   # # #  ##   # # # #   #      #   ##  # # #    #               ";
	std::wcout << L"              ### ### #   # ### ### ### ###    ### # # # # #    #               ";
	std::wcout << L"                                                                                ";
	textColor(1, 3);
	std::wcout << L"                              By Tim Straubinger                                ";
	std::wcout << L"                                                                                ";

	mygotoxy(20, 15);
	textColor(0, 15);
	std::wcout << L"Enter a world seed (0 for random one): ";

	unsigned int seed;

	std::wcin >> seed;

	if (seed == 0){
		seed = (unsigned)time(0);
	}
	srand(seed);

	int difficulty = 0;

	mygotoxy(25, 18);
	std::wcout << L"1 - Piece of cake";
	mygotoxy(25, 19);
	std::wcout << L"2 - Moderate";
	mygotoxy(25, 20);
	std::wcout << L"3 - Tricky";
	mygotoxy(25, 21);
	std::wcout << L"4 - Extreme";
	mygotoxy(20, 17);
	std::wcout << L"Please select a difficulty: ";
	while (!(difficulty >= 1 && difficulty <= 4)){
		std::wcin >> difficulty;
		if (difficulty < 1 || difficulty > 4){
			drawLine(0, 17, 79, 17, L' ', 15);
			mygotoxy(20, 17);
			std::wcout << L"Please select a VALID difficulty: ";
		}
	}

	clearScreen();


	std::wcout << L"\n\n\t\tCreating materials...\n";

	//define materials
	const size_t num_materials = 12;
	const std::array<std::wstring, num_materials> material_names = {L"air", L"stone", L"dirt", L"sand", L"water", L"plant", L"grass", L"snow", L"ore", L"wall", L"door", L"bedrock"};
	const std::array<int, num_materials> material_bgcolors = {0, 7, 4, 15, 1, 2, 2, 16, 8, 8, 4, 8};
	const std::array<int, num_materials> material_fgcolors = {0, 8, 6, 14, 9, 12, 2, 0, 6, 8, 11, 8};
	const std::array<wchar_t, num_materials> material_chars = {L' ', L'░', L'▒', L'▒', L'≈', L'♣', L' ', L' ', L'♦', L'╬', L'■', L'░'};

	std::array<int, num_materials> inventory = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::array<int, num_materials> underlying_material = {0, 11, 1, 2, 4, 6, 2, 1, 1, 1, 2, 1};

	std::wcout << L"\t\tGenerating terrain...\n";

	//generate terrain
	const int width = 500, height = 500;
	std::array<std::array<int, 501>, 501> terrain;

	//turn everything into grass
	for (int x = 0; x <= width; x++){
		for (int y = 0; y <= height; y++){
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

	std::wcout << L"\t\tPreparing game...";

	//fixed vital gameplay variables
	wchar_t key = L'r';
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

	std::array<wchar_t, 5> monster_chars = {L'S', L'Ω', L'@', L'¥', L'Ö'};
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
			std::wcout << material_chars.at(temp_material);
			Sleep(1);
		}
	}

	//draw the inventory
	drawBox(1, 26, 23, 35, L'░', 8, L"INVENTORY");
	for (int i = 1; i <= 8; i++){
		mygotoxy(2, 26 + i);
		if (i == selection){
			textColor(0, 14);
		} else {
			textColor(0, 15);
		}
		std::wcout << i << L" " << material_names.at(i) << L" \t";
		textColor(material_bgcolors.at(i), material_fgcolors.at(i));
		std::wcout << material_chars.at(i);
		textColor(0, 15);
		std::wcout << L" " << inventory.at(i) << L'\n';
	}

	//draw the help box
	drawBox(25, 26, 40, 35, L'░', 8, L"QUICK HELP");
	textColor(0, 15);
	mygotoxy(26, 27);
	std::wcout << L"w - up";
	mygotoxy(26, 28);
	std::wcout << L"a - left";
	mygotoxy(26, 29);
	std::wcout << L"s - down";
	mygotoxy(26, 30);
	std::wcout << L"d - right";
	mygotoxy(26, 31);
	std::wcout << L"m - mine";
	mygotoxy(26, 32);
	std::wcout << L"p - place";
	mygotoxy(26, 33);
	std::wcout << L"q - quit";
	mygotoxy(26, 34);
	std::wcout << L"h - more help";

	//draw the info box
	drawBox(42, 26, 78, 35, L'░', 8, L"INFO");
	mygotoxy(43, 27);
	textColor(0, 15);
	if (terrain.at(player_x).at(player_y) == 4){
		std::wcout << L"You are standing in water.";
	} else if (terrain.at(player_x).at(player_y) == 5){
		std::wcout << L"You are standing on a plant.";
	} else {
		std::wcout << L"You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << L".";
	}
	mygotoxy(43, 32);
	std::wcout << L"Difficulty: L";
	if (difficulty == 1){
		std::wcout << L"Piece of cake";
	} else if (difficulty == 2){
		std::wcout << L"Moderate";
	} else if (difficulty == 3){
		std::wcout << L"Tricky";
	} else if (difficulty == 4){
		std::wcout << L"Extreme";
	}
	mygotoxy(43, 33);
	std::wcout << L"Health: " << health;
	mygotoxy(43, 34);
	std::wcout << L"Seed: " << seed;

	//draw monsters
	for (int i = 0; i <= num_monsters; i++){
		if (testRange(79, 24, monster_x.at(i) - x_offset, monster_y.at(i) - y_offset)){
			monster_oldx.at(i) = monster_x.at(i);
			monster_oldy.at(i) = monster_y.at(i);

			mygotoxy(monster_x.at(i) - x_offset, monster_y.at(i) - y_offset);
			textColor(material_bgcolors.at(terrain.at(monster_x.at(i)).at(monster_y.at(i))), 0);
			std::wcout << monster_chars.at(monster.at(i));
		}
	}

	//draw the player
	mygotoxy(player_x - x_offset, player_y - y_offset);
	if (terrain.at(player_x).at(player_y) == 7){
		textColor(0, 15);
	} else {
		textColor(material_bgcolors.at(terrain.at(player_x).at(player_y)), material_bgcolors.at(terrain.at(player_x).at(player_y)));
	}
	std::wcout << L'Å';

	//draw dividers
	drawLine(0, 25, 79, 25, L'░', 4);
	drawLine(0, 36, 79, 36, L'░', 4);
	drawLine(0, 25, 0, 36, L'░', 4);
	drawLine(24, 25, 24, 36, L'░', 4);
	drawLine(41, 25, 41, 36, L'░', 4);
	drawLine(79, 25, 79, 36, L'░', 4);

	//draw the alert to maximise the window
	drawBox(19, 5, 59, 9, L'░', 8, L"ALERT", true);
	mygotoxy(20, 6);
	textColor(0, 15);
	std::wcout << L"Please maximize this window, or resize";
	mygotoxy(20, 7);
	std::wcout << L"it to fit all components of the game.";
	mygotoxy(20, 8);
	std::wcout << L"Press any key to begin...";

	mygotoxy(0, 0);
	_getwch();

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
		if (key == L'w'){
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
		if (key == L'a'){
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
		if (key == L's'){
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
		if (key == L'd'){
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
		if (key == L'r'){
			redraw = true;
			redraw_player = true;
			redraw_help = true;
			redraw_inventory = true;
		}
		if (key == L'm'){
			mygotoxy(43, 28);
			textColor(0, 15);
			std::wcout << L"Select a direction to mine...";
			if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
				mygotoxy(player_x - x_offset - 1, player_y - y_offset);
				textColor(material_bgcolors.at(terrain.at(player_x - 1).at(player_y)), 0);
				std::wcout << L'◄';
			}
			if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
				mygotoxy(player_x - x_offset, player_y - y_offset - 1);
				textColor(material_bgcolors.at(terrain.at(player_x).at(player_y - 1)), 0);
				std::wcout << L'▲';
			}
			if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
				mygotoxy(player_x - x_offset + 1, player_y - y_offset);
				textColor(material_bgcolors.at(terrain.at(player_x + 1).at(player_y)), 0);
				std::wcout << L'►';
			}
			if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
				mygotoxy(player_x - x_offset, player_y - y_offset + 1);
				textColor(material_bgcolors.at(terrain.at(player_x).at(player_y + 1)), 0);
				std::wcout << L'▼';
			}
			wchar_t direction = _getwch();
			if (direction == L'w'){
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
			if (direction == L'a'){
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
			if (direction == L's'){
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
			if (direction == L'd'){
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
				std::wcout << material_chars.at(temp_material);
			}
			if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
				mygotoxy(player_x - x_offset, player_y - y_offset - 1);
				temp_material = terrain.at(player_x).at(player_y - 1);
				textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
				std::wcout << material_chars.at(temp_material);
			}
			if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
				mygotoxy(player_x - x_offset + 1, player_y - y_offset);
				temp_material = terrain.at(player_x + 1).at(player_y);
				textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
				std::wcout << material_chars.at(temp_material);
			}
			if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
				mygotoxy(player_x - x_offset, player_y - y_offset + 1);
				temp_material = terrain.at(player_x).at(player_y + 1);
				textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
				std::wcout << material_chars.at(temp_material);
			}

			redraw_help = true;
			redraw_inventory = true;

			mygotoxy(43, 28);
			textColor(0, 15);
			std::wcout << L"                              ";
		}
		if (key == L'p'){
			mygotoxy(43, 28);
			textColor(0, 15);

			if (inventory.at(selection) > 0){
				mygotoxy(43, 28);
				std::wcout << L"Select a direction to place a";
				mygotoxy(43, 29);
				std::wcout << L"block...";
				if (testRange(79, 24, player_x - x_offset - 1, player_y - y_offset)){
					mygotoxy(player_x - x_offset - 1, player_y - y_offset);
					textColor(material_bgcolors.at(terrain.at(player_x - 1).at(player_y)), 0);
					std::wcout << L'◄';
				}
				if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
					mygotoxy(player_x - x_offset, player_y - y_offset - 1);
					textColor(material_bgcolors.at(terrain.at(player_x).at(player_y - 1)), 0);
					std::wcout << L'▲';
				}
				if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
					mygotoxy(player_x - x_offset + 1, player_y - y_offset);
					textColor(material_bgcolors.at(terrain.at(player_x + 1).at(player_y)), 0);
					std::wcout << L'►';
				}
				if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
					mygotoxy(player_x - x_offset, player_y - y_offset + 1);
					textColor(material_bgcolors.at(terrain.at(player_x).at(player_y + 1)), 0);
					std::wcout << L'▼';
				}

				wchar_t direction = _getwch();

				if (direction == L'w'){
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
				if (direction == L'a'){
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
				if (direction == L's'){
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
				if (direction == L'd'){
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
					std::wcout << material_chars.at(temp_material);
				}
				if (testRange(79, 24, player_x - x_offset, player_y - y_offset - 1)){
					mygotoxy(player_x - x_offset, player_y - y_offset - 1);
					temp_material = terrain.at(player_x).at(player_y - 1);
					textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
					std::wcout << material_chars.at(temp_material);
				}
				if (testRange(79, 24, player_x - x_offset + 1, player_y - y_offset)){
					mygotoxy(player_x - x_offset + 1, player_y - y_offset);
					temp_material = terrain.at(player_x + 1).at(player_y);
					textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
					std::wcout << material_chars.at(temp_material);
				}
				if (testRange(79, 24, player_x - x_offset, player_y - y_offset + 1)){
					mygotoxy(player_x - x_offset, player_y - y_offset + 1);
					temp_material = terrain.at(player_x).at(player_y + 1);
					textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
					std::wcout << material_chars.at(temp_material);
				}

				redraw_help = true;
				redraw_inventory = true;

			}

			mygotoxy(43, 28);
			textColor(0, 15);
			std::wcout << L"                              ";
			mygotoxy(43, 29);
			std::wcout << L"                              ";
			mygotoxy(43, 30);
			std::wcout << L"                              ";
		}
		if (key == L'h'){
			drawBox(1, 1, 78, 23, L'░', 8, L"HELP", true);

			textColor(0, 15);
			mygotoxy(3, 3);
			std::wcout << L"Moving Around - use the keys 'w' (up), 'a' (left), 's' (down) and 'd'";
			mygotoxy(3, 4);
			std::wcout << L"     (right) to move the player around.";
			mygotoxy(3, 5);
			std::wcout << L"Mining - press 'm' to mine, then select a direction using 'w', 'a , 's' or";
			mygotoxy(3, 6);
			std::wcout << L"     'd', or press 'm' again to cancel. The material you mine will be added";
			mygotoxy(3, 7);
			std::wcout << L"     to your inventory.";
			mygotoxy(3, 8);
			std::wcout << L"Placing Blocks - press 'p' to place a selected block from your inventory,"; 
			mygotoxy(3, 9);
			std::wcout << L"     using the direction keys for where you want to place the block.";
			mygotoxy(3, 10);
			std::wcout << L"Selecting Blocks - to choose a block in your inventory that you want to";
			mygotoxy(3, 11);
			std::wcout << L"      place, press the corresponding number key.";
			mygotoxy(3, 12);
			std::wcout << L"Refreshing the Screen - if at any point you're fed up with the all the";
			mygotoxy(3, 13);
			std::wcout << L"     visual flaws that are ruining your gaming experience, press 'r' to";
			mygotoxy(3, 14);
			std::wcout << L"     refresh the game display.";
			mygotoxy(3, 15);
			std::wcout << L"Exiting - plain and simple, press 'q' to close the game.";
			mygotoxy(3, 16);
			std::wcout << L"Time Passage - time passes each time you perform and action, such as moving";
			mygotoxy(3, 17);
			std::wcout << L"     or mining a block. To allow time to pass while not doing anything,";
			mygotoxy(3, 18);
			std::wcout << L"     simply hold down any non-functional key.";
			mygotoxy(3, 19);
			std::wcout << L"          Press any key to continue...";

			mygotoxy(3, 21);
			std::wcout << L".at(Hint: Try placing stone on top of stone to create walls and ore on top of";
			mygotoxy(3, 22);
			std::wcout << L"     dirt to create doors)";

			_getwch();
			redraw = true;
			redraw_player = true;
		}
		if (key >= 49 && key <= 56){
			selection = key - 48;
		}
		if (key == L'q'){
			drawBox(19, 5, 59, 9, L'░', 8, L"ALERT", true);
			mygotoxy(20, 6);
			textColor(0, 15);
			std::wcout << L"Are you sure you want to quit? (y/n) ";
			wchar_t answer = L'\0';
			std::wcin >> answer;
			if (answer == L'y'){
				quit = true;
			} else {
				redraw = true;
				redraw_player = true;
			}
		}
		if (key == L'x'){
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
									std::wcout << material_chars.at(temp_material);
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
									std::wcout << material_chars.at(temp_material);
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
									std::wcout << material_chars.at(temp_material);
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
								std::wcout << material_chars.at(temp_material);
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
							std::wcout << material_chars.at(temp_material);
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
							std::wcout << material_chars.at(temp_material);
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
							std::wcout << material_chars.at(temp_material);
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
							std::wcout << material_chars.at(temp_material);
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
			for (int x_index = 0; x_index <= 79; x_index++){
				for (int y_index = 0; y_index <= 24; y_index++){
					mygotoxy(x_index, y_index);
					temp_material = terrain.at(x_index + x_offset).at(y_index + y_offset);
					textColor(material_bgcolors.at(temp_material), material_fgcolors.at(temp_material));
					std::wcout << material_chars.at(temp_material);
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
						std::wcout << material_chars.at(terrain.at(monster_oldx.at(i)).at(monster_oldy.at(i)));
					}
				}

				if (testRange(79, 24, monster_x.at(i) - x_offset, monster_y.at(i) - y_offset)){
					mygotoxy(monster_x.at(i) - x_offset, monster_y.at(i) - y_offset);
					textColor(material_bgcolors.at(terrain.at(monster_x.at(i)).at(monster_y.at(i))), 0);
					std::wcout << monster_chars.at(monster.at(i));
				}
			}
		}

		//redraw player
		if (redraw_player){
			mygotoxy(player_x - x_offset, player_y - y_offset);
			if (terrain.at(player_x).at(player_y) == 7){
				textColor(0, 15);
			} else {
				textColor(material_bgcolors.at(terrain.at(player_x).at(player_y)), material_bgcolors.at(terrain.at(player_x).at(player_y)));
			}
			std::wcout << L'Å';
		}

		//redraw help
		if (redraw_help){
			textColor(0, 15);
			mygotoxy(26, 27);
			std::wcout << L"w - up";
			mygotoxy(26, 28);
			std::wcout << L"a - left";
			mygotoxy(26, 29);
			std::wcout << L"s - down";
			mygotoxy(26, 30);
			std::wcout << L"d - right";
			mygotoxy(26, 31);
			std::wcout << L"m - mine";
			mygotoxy(26, 32);
			std::wcout << L"p - place";
			mygotoxy(26, 33);
			std::wcout << L"q - quit";
			mygotoxy(26, 34);
			std::wcout << L"h - more help";
		}

		//redraw inventory
		for (int i = 1; i <= 8; i++){
			mygotoxy(2, 26 + i);
			if (i == selection){
				textColor(0, 14);
			} else {
				textColor(0, 15);
			}
			std::wcout << i << L" " << material_names.at(i) << L" \t";
			textColor(material_bgcolors.at(i), material_fgcolors.at(i));
			std::wcout << material_chars.at(i);
			textColor(0, 15);
			std::wcout << L" " << inventory.at(i) << L" ";
		}

		//redraw info box
		if (redraw_info){
			mygotoxy(43, 27);
			textColor(0, 15);
			if (terrain.at(player_x).at(player_y) == 4){
				std::wcout << L"You are standing in water.";
			} else if (terrain.at(player_x).at(player_y) == 5){
				std::wcout << L"You are standing on a plant.";
			} else {
				std::wcout << L"You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << L".";
			}
			mygotoxy(43, 28);
			mygotoxy(43, 32);
			std::wcout << L"Difficulty: ";
			if (difficulty == 1){
				std::wcout << L"Piece of cake";
			} else if (difficulty == 2){
				std::wcout << L"Moderate";
			} else if (difficulty == 3){
				std::wcout << L"Tricky";
			} else if (difficulty == 4){
				std::wcout << L"Extreme";
			}
			mygotoxy(43, 33);
			std::wcout << L"Health: " << health << L"   ";
		}

		//cover up the player's tracks, add a delay if in water and refresh the info 
		if (player_x != old_x || player_y != old_y){
			mygotoxy(old_x - x_offset, old_y - y_offset);
			temp_material = terrain.at(old_x).at(old_y);
			textColor(material_bgcolors.at(temp_material), 
				material_fgcolors.at(temp_material));
			std::wcout << material_chars.at(temp_material);

			if (terrain.at(player_x).at(player_y) == 4){
				Sleep(100);
			}

			mygotoxy(43, 27);
			textColor(0, 15);
			if (terrain.at(player_x).at(player_y) == 4){
				std::wcout << L"You are standing in water.     ";
			} else if (terrain.at(player_x).at(player_y) == 5){
				std::wcout << L"You are standing on a plant.     ";
			} else {
				std::wcout << L"You are standing on " << material_names.at(terrain.at(player_x).at(player_y)) << L".     ";
			}
		}

		mygotoxy(0, 0);
		textColor(0, 15);

		if (!quit){
			key = _getwch();
		}
	}

	return 0;
}