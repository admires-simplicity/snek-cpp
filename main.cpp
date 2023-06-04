#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <deque>

enum directions {UP, DOWN, LEFT, RIGHT};
enum status {LIVE, DED};
enum cause_of_death {NONE, AUTOSARCOPHAGY, WALL_BRUTALITY};

//GLOBALS
const int win_height = 20;
const int win_width = 41;

struct pos {
	int y, x;
	bool operator==(const pos& rhs) const {
		return y == rhs.y && x == rhs.x;
	}
};

class Snake {
public:
	//std::deque<pos> body { std::deque<pos>(0) };
	std::deque<pos> body;
	//directions dir { RIGHT };	


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


	void die() {
		stat = DED;
	}

	bool isDead() {
		return stat == DED;
	}

	cause_of_death deathBy() {
		return cod;
	}

	bool colliding(pos p) const {
		for (auto b : body)
			if (b == p)
				return true;
		return false;
	}

private:
	status stat { LIVE };
	cause_of_death cod { NONE };

	void moveSnake(pos next) {
		if (ateSelf(next)) {
			die();
			cod = AUTOSARCOPHAGY;
		} else 
		if (out_of_bounds(next)) {
			die();
			cod = WALL_BRUTALITY;
		}
		if (isDead()) return;
		body.push_front(next);
		body.pop_back();
	}

	bool ateSelf(pos next) {
		return colliding(next);
	}

	bool out_of_bounds(pos p) {
		return p.y < 1 || p.y >= win_height - 1 || p.x < 1 || p.x >= win_width - 1;
	}


};

pos randApple(const Snake &snek) {
	pos apple;
	do {
		apple.y = rand() % (win_height - 2) + 1;
		apple.x = rand() % (((win_width - 2) / 2)) * 2 + 2;
	} while (snek.colliding(apple));
	return apple;
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

	Snake snek;
	snek.body.push_back({1, 6});
	snek.body.push_back({1, 4});
	snek.body.push_back({1, 2});


	bool ded = false;

	directions dir { RIGHT };
	pos apple = randApple(snek);
	
	do {
		wclear(stdscr);

		attron(COLOR_PAIR(1));
		printw("snek game :)");
		attroff(COLOR_PAIR(1));

		wclear(win);
		box(win, 0, 0);


		//render snek
		wattron(win, COLOR_PAIR(1));
		for (auto p : snek.body) {
			wmove(win, p.y, p.x);
			waddch(win, 'o');
		}
		wattroff(win, COLOR_PAIR(1));

		// render apple
		wmove(win, apple.y, apple.x);
		waddch(win, '*');

		//if (out_of_bounds(snek.body.front())) {
		//	snek.die();
		//	wmove(win, snek.body.front().y, snek.body.front().x);
		//	attron(COLOR_PAIR(1));
		//	waddch(win, 'x');
		//	attroff(COLOR_PAIR(1));

		//}
		
		c = getch();

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

		if (snek.isDead()) {
			wmove(win, snek.body.front().y, snek.body.front().x);
			waddch(win, 'x');
		}

		
		//switch (c) {
		//	case KEY_UP:    snek.moveUp();    break;
		//	case KEY_DOWN:  snek.moveDown();  break;
		//	case KEY_LEFT:  snek.moveLeft();  break;
		//	case KEY_RIGHT: snek.moveRight(); break;
		//	default: break;
		//}
		//wprintw(win, "%d", x);

		if (snek.body.front() == apple) {
			snek.body.push_back(snek.body.back());
			apple = randApple(snek);
		}
		

		//wmove(win, y, x);
		//waddch(win, HEAD);
		//wmove(win, y, x);



		refresh();
		wrefresh(win);
	} while (//c = getch(), 
			//usleep(16666),
			usleep(100000),
			//(++f > 60) ? f = 0 : f,
			c != 'q' && c != 'Q' && !snek.isDead());


	if (snek.deathBy() == WALL_BRUTALITY)
		printw(" YOU DIED BY RUNNING INTO A WALL");
	else if (snek.deathBy() == AUTOSARCOPHAGY)
		printw(" CONGRATULATIONS, YOU ATE YOURSELF");
	else if (c == 'q' || c == 'Q') {
		printw(" GAME ABORTED");
	}

	timeout(-1);
	getch();
	endwin();			

	printf("goodbye!\n");

	return 0;
}
