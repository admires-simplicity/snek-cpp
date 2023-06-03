#include <ncurses.h>
#include <unistd.h>
#include <deque>

enum directions {UP, DOWN, LEFT, RIGHT};

struct pos {
	int y, x;
};

class Snake {
public:
	//std::deque<pos> body { std::deque<pos>(0) };
	std::deque<pos> body;
	//Snake() : body{std::deque<pos>(0)};
	//Snake(std::deque<pos> b) : body(b) {}
	Snake() {};

	void moveUp() {
		pos next = {body.front().y - 1, body.front().x};
		moveSnake(next);
	}
	void moveDown() {
		pos next = {body.front().y + 1, body.front().x};
		moveSnake(next);
	}
	void moveLeft() {
		pos next = {body.front().y, body.front().x - 2};
		moveSnake(next);
	}
	void moveRight() {
		pos next = {body.front().y, body.front().x + 2};
		moveSnake(next);
	}



private:
	void moveSnake(pos next) {
		body.push_front(next);
		body.pop_back();
	}

};

const int win_height = 20;
const int win_width = 41;

bool out_of_bounds(pos p) {
	return p.y < 1 || p.y >= win_height - 1 || p.x < 1 || p.x >= win_width - 1;
}

int main(int argc, char *argv[])
{
	initscr();			// Start curses mode
	cbreak();			// Line buffering disabled
	noecho();			// Don't echo() while we do getch
	keypad(stdscr, TRUE);		// Enable function keys
	curs_set(0);			// Hide cursor
	timeout(0);			// Non-blocking getch()

	//start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED);


	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	//for (int c; c = getch(); c != 'q' && c != 'Q') {
	int c;
	WINDOW *win = newwin(win_height, win_width, 1, 0);
	box(win, 0, 0);

	int y = 1, x = 1;

	//int f = 0;

	directions dir = RIGHT;
	Snake snek;
	snek.body.push_back({1, 6});
	snek.body.push_back({1, 4});
	snek.body.push_back({1, 2});


	bool ded = false;

	do {
		c = getch();
		wclear(stdscr);

		attron(COLOR_PAIR(1));
		printw("snek game :)");
		attroff(COLOR_PAIR(1));

		wclear(win);
		box(win, 0, 0);
		//wmove(win, y, x);
		//wprintw(win, "frame %2d", f);

		//render snek
		wattron(win, COLOR_PAIR(1));
		for (auto p : snek.body) {
			wmove(win, p.y, p.x);
			waddch(win, 'o');
		}
		wattroff(win, COLOR_PAIR(1));

		if (out_of_bounds(snek.body.front())) {
			ded = true;
			wmove(win, snek.body.front().y, snek.body.front().x);
			attron(COLOR_PAIR(1));
			waddch(win, 'o');
			attroff(COLOR_PAIR(1));

		}

		switch (c) {
			case KEY_UP:   if (dir != DOWN)  dir = UP;    break; 
			case KEY_DOWN: if (dir != UP)    dir = DOWN;  break;
			case KEY_LEFT: if (dir != RIGHT) dir = LEFT;  break;
			case KEY_RIGHT:if (dir != LEFT)  dir = RIGHT; break;
		}
		switch (dir) {
			case UP:    snek.moveUp();    break;
			case DOWN:  snek.moveDown();  break;
			case LEFT:  snek.moveLeft();  break;
			case RIGHT: snek.moveRight(); break;
		}

		

		//wmove(win, y, x);
		//waddch(win, HEAD);
		//wmove(win, y, x);



		refresh();
		wrefresh(win);
	} while (c = getch(), 
			//usleep(16666),
			usleep(100000),
			//(++f > 60) ? f = 0 : f,
			c != 'q' && c != 'Q' && !ded);

	printw(" YOU DIED BY RUNNING INTO A WALL");

	timeout(-1);
	getch();
	endwin();			

	return 0;
}
