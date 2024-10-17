#include<iostream>
#include<vector>
#include<ctime>
#include<SFML\Graphics.hpp>
#include<Windows.h>

using namespace std;
using namespace sf;

bool gamerunner = true;
enum SNAKE_DIRECTION {UP, DOWN, LEFT, RIGHT, IDLE};

class Snake {
	RectangleShape shape;
public:
	Snake() {
		shape.setPosition(100, 100);
		shape.setFillColor(Color::White);
		shape.setSize(Vector2f(20, 20));
	}

	void setPosition_X(int x) {
		shape.setPosition(int(shape.getPosition().x) + x, int(shape.getPosition().y));
	}

	void setPosition_Y(int y) {
		shape.setPosition(int(shape.getPosition().x), int(shape.getPosition().y) + y);
	}

	void setPosition(Vector2f pos) {
		shape.setPosition(pos);
	}

	int getPosition_X() {
		return shape.getPosition().x;
	}

	int getPosition_Y() {
		return shape.getPosition().y;
	}

	Vector2f getPosition() {
		return shape.getPosition();
	}

	void move_X(int x) {
		shape.setPosition(Vector2f((getPosition_X() + x), getPosition_Y()));
	}

	void move_Y(int y) {
		shape.setPosition(Vector2f(getPosition_X(), getPosition_Y() - y));
	}

	void render(RenderWindow& window) {
		window.draw(shape);
	}
};

class Apple {
private:
	RectangleShape shape;
public:
	Apple() {
		shape.setSize(Vector2f(20, 20));
		shape.setFillColor(Color::Red);
		shape.setPosition((rand() % 36 + 2) * 20, (rand() % 26 + 2) * 20);
	}

	void randomizePosition(vector<Snake*> snake) {
		shape.setPosition((rand() % 38 + 1) * 20, (rand() % 28 + 1) * 20);
		for (int i = 0; i < snake.size(); i++) {
			if (shape.getPosition() == snake[i]->getPosition()) {
				return randomizePosition(snake);
			}
		}
	}

	void Render(RenderWindow& window) {
		window.draw(shape);
	}

	Vector2f getPosition() {
		return shape.getPosition();
	}
};

class Borders {
private:
	RectangleShape Top;
	RectangleShape Bottom;
	RectangleShape Left;
	RectangleShape Right;
public:
	Borders() {
		Top.setSize(Vector2f(800, 20));
		Bottom.setSize(Vector2f(800, 20));
		Left.setSize(Vector2f(20, 600));
		Right.setSize(Vector2f(20, 600));

		Top.setFillColor(Color::White);
		Bottom.setFillColor(Color::White);
		Left.setFillColor(Color::White);
		Right.setFillColor(Color::White);

		Top.setPosition(Vector2f(0, 0));
		Bottom.setPosition(Vector2f(0, 580));
		Left.setPosition(Vector2f(0, 0));
		Right.setPosition(Vector2f(780, 0));
	}

	void Render(RenderWindow& window) {
		window.draw(Top);
		window.draw(Left);
		window.draw(Bottom);
		window.draw(Right);
	}
};

class Score {
private:
	Text text;
	Font font;
	string textString;
public:
	Score() {
		if (!font.loadFromFile("Font/calibri-bold.ttf"))
			cout << "Can't load font.\n";
		text.setFillColor(Color::White);
		text.setFont(font);
		text.setCharacterSize(40);
		text.setPosition(Vector2f(40, 605));
	}

	void Render(RenderWindow& window) {
		window.draw(text);
	}

	void setString(String textString) {
		text.setString(textString);
	}

	void setPosition_X(float pos_x) {
		text.setPosition(pos_x, 605);
	}
};

class GridBlock {
private:
	RectangleShape shape;

public:
	GridBlock() {
		shape.setFillColor(Color::Color(67, 67, 67));
	}

	void setPosition(float x, float y) {
		shape.setPosition(Vector2f(x, y));
	}

	void setSize(int block_size) {
		shape.setSize(Vector2f(block_size, block_size));
	}

	void render(RenderWindow* window) {
		window->draw(shape);
	}
};

void gameover(RenderWindow& window, vector<Snake*>& snake, vector<vector<GridBlock*>>& grid) {
	for (int i = 0; i < snake.size(); i++) {
		snake.pop_back();
	}	
	for (int i = 0; i < (grid.size()); i++) {
		for (int j = 0; j < (grid[i].size()); j++) {
			grid[i].pop_back();
		}
	}
	window.close();
}

void initializeGrid(int block_size, int spacing_factor, vector<vector<GridBlock*>>& grid, int map_Width, int map_Height,
					int border_thickness) {

	for (int i = 0; i < (map_Height / (block_size + spacing_factor)); i++) {
		for (int j = 0; j < (map_Width / (block_size + spacing_factor)); j++) {
			grid[i].push_back(new GridBlock);
			grid[i][j]->setPosition((block_size + spacing_factor) * j + border_thickness,
									(block_size + spacing_factor) * i + border_thickness);
			grid[i][j]->setSize(block_size);
		}
	}

}

int main() {
	// initializing and declaring
	srand(time(NULL));
	int window_Width = 800;
	int window_Height = 660;
	int map_Width = 760;
	int map_Height = 560;
	int border_Thickness = 20;

	RenderWindow window(VideoMode(window_Width, window_Height), "Snake");
	Event event;

	vector<Snake*> snake;
	snake.push_back(new Snake);
	snake.push_back(new Snake);

	int block_size = 18;
	int spacing_factor = 2;
	vector<vector<GridBlock*>> grid((map_Height / (block_size + spacing_factor)));

	initializeGrid(block_size, spacing_factor, grid, map_Width, map_Height, border_Thickness);

	Apple apple;
	Borders borders;

	int total_score = 0;
	Score scoreText;
	Score score;
	string textString = "Score: ";
	score.setPosition_X(150);
	scoreText.setString(textString);
	score.setString(to_string(total_score));
	
	int Framerate = 60; 
	float moveDelay = 0.06f; 
	float accumulatedTime = 0.0f;

	Clock clock;

	bool isPositionChanged = false;
	SNAKE_DIRECTION SNAKE_DIR = IDLE;

	// Window 
	window.setFramerateLimit(Framerate);
	
	// Main game loop
	while (window.isOpen()) {
		float dt = clock.restart().asSeconds();
		accumulatedTime += dt;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				gameover(window, snake, grid);
			}
		}

		// Input		

		if (!isPositionChanged) {
			if (Keyboard::isKeyPressed(Keyboard::W) && SNAKE_DIR != DOWN && Event::KeyReleased) {
				SNAKE_DIR = UP;
				isPositionChanged = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::A) && SNAKE_DIR != RIGHT && Event::KeyReleased) {
				SNAKE_DIR = LEFT;
				isPositionChanged = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::S) && SNAKE_DIR != UP && Event::KeyReleased) {
				SNAKE_DIR = DOWN;
				isPositionChanged = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::D) && SNAKE_DIR != LEFT && Event::KeyReleased) {
				SNAKE_DIR = RIGHT;
				isPositionChanged = true;
			}
		}
		
		// Update
		if (accumulatedTime >= moveDelay) {
			accumulatedTime -= moveDelay;

			for (int i = snake.size() - 1; i > 0; i--) {
				snake[i]->setPosition(snake[i - 1]->getPosition());
			}

			if (SNAKE_DIR == UP) {
				snake[0]->move_Y(20);
			}
			else if (SNAKE_DIR == LEFT) {
				snake[0]->move_X(-20);
			}
			else if (SNAKE_DIR == DOWN) {
				snake[0]->move_Y(-20);
			}
			else if (SNAKE_DIR == RIGHT) {
				snake[0]->move_X(20);
			}
			isPositionChanged = false;
		}

		if (snake[0]->getPosition() == apple.getPosition()) {
			total_score++;
			score.setString(to_string(total_score));
			apple.randomizePosition(snake);
			snake.push_back(new Snake);
			snake[snake.size() - 1]->setPosition(snake[snake.size() - 2]->getPosition());
		}

		for (int i = 2; i < snake.size(); i++) {
			if (snake[0]->getPosition() == snake[i]->getPosition()) {
				gamerunner = false;
				break;
			}
		}

		if (snake[0]->getPosition_X() == 0 || snake[0]->getPosition_X() == 780 || snake[0]->getPosition_Y() == 0 || snake[0]->getPosition_Y() == 580) {
			gamerunner = false;
		}

		if (!gamerunner)
			gameover(window, snake, grid);

		// Draw image
		window.clear();
		for (int i = 0; i < grid.size(); i++) {
			for (int j = 0; j < grid[i].size(); j++) {
				grid[i][j]->render(&window);
			}
		}
		for (int i = 0; i < snake.size(); i++) {
			snake[i]->render(window);
		}
		apple.Render(window);
		borders.Render(window);
		scoreText.Render(window);
		score.Render(window);
		window.display();
	}
}
