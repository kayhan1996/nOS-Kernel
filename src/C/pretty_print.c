#include "pretty_print.h"
#include "printx.h"
#include "stdint.h"

typedef struct {
    int x;
    int y;
} Point;

typedef enum key {
    Up = 66,
    Down = 65,
    Left = 68,
    Right = 67,
    Space = 32,
    Tab = 9
} Key;

void format(){
	int columns = 0;
    uint8_t color = 184;
    int key = 0;

    HideCursor();
    while(1){  
        SetCursorPosition(95, 8);
        if(columns == 0) printf("\033[38;5;%dm", color);
        if(columns == 1) printf("\033[48;5;%dm", color);
        printf("%3d", color);

        key = receive();
        if(key == Up){
            color++;
        }
        if(key == Down){
            color--;
        }
        if(key == Tab){
            columns = (columns + 1) % 2;
        }
        if(key == 'f'){
            break;
        }
    }
    ShowCursor();
}


void SetCursorPosition(int x, int y){
        printf("\033[%d;%dH", (signed int)y, (signed int)x);
}

void HideCursor(){
    printf("\e[?25l");
}

void ShowCursor(){
    printf("\e[?25h");
}

void clear(){
	printf("%c[1;1f%c[J%c[3J", 27, 27, 27);
}

void drawUI(){
    SetCursorPosition(80, 4);
	printf("Select Character (T): ");
	SetCursorPosition(80, 6);
	printf("Pen Down (S): ");
	SetCursorPosition(80, 8);
	printf("Format (F): ");
}

void debugState(char key){
    HideCursor();
    SetCursorPosition(80, 10);
    printf("Debug key: %d ", key);
    ShowCursor();
}

void setCharacter(char *character){
    SetCursorPosition(110, 4);
	*character = receive();
	send(*character);
}

void penState(int *penDown){
    SetCursorPosition(110, 6);
    *penDown = !(*penDown);
    if(*penDown != 1){
        printf("0");
        *penDown = 0;
    }else{
        printf("1");
        *penDown = 1;
    }
}

void checkCursorBounds(Point *p){
    if(p->x < 0){
		p->x = 0;
	}
    if(p->x > 70){
        p->x = 70;
    }

    if(p->y < 0){
        p->y = 0;
    }
    if(p->y > 15){
        p->y = 15;
    }
}

void paint(){
    Point p;
	int penDown = 0;
	int debug = 0;

	char key;
	char character = '*';

    drawUI();

	while(1){
		SetCursorPosition(p.x, p.y);
		if(penDown) send(character);
		SetCursorPosition(p.x, p.y);

		key = receive();

		if(key == Left){
			p.x--;
		}else if(key == Right){
			p.x++;
		}else if(key == Down){
			p.y--;
		}else if(key == Up){
			p.y++;
		}else if(key == 'c'){
			clear();
			drawUI();
		}else if(key == 't'){
			setCharacter(&character);
		}else if(key == Space){
            penState(&penDown);
		}else if(key == 'd'){
			debug = !debug;
		}else if(key == 'f'){
			format();
		}else if(key == 'h'){
            HideCursor();
        }else if(key == 's'){
            ShowCursor();
        }

        if(debug == 1){
            debugState(key);
        }

		checkCursorBounds(&p);

	}
}