#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <stack>
#include <iostream>
#include <conio.h>
#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 17
#define HEIGHT 20
#define MY_BUFSIZE 1024
#define GAME_IMG 1
#define GAME_FINISH 0
#define LOCK 1
#define OPEN 0
#define INIT 1
#define GAME 2
using namespace std;

int score = 0;
int user_bubble = 8;//별의 위치
int user_bubblecolor = 3;//구슬색깔
void display();
int getkey(int);
int result();
void initDisplay(int direction);
void random_color();
int add_bubble();
void init_setting();
void game_over_display();
void displayPosition(int x, int y, int values);
void array_copy(int src[][WIDTH], int dst[][WIDTH]);

clock_t start;
struct buffer {
	int x;
	int y;
};
struct reminder {
	int x;
	int y;
	int color;
};
int background[HEIGHT][WIDTH] = { 0 };

struct Line {
	int top;
	int button;
};
struct Line line = { 1,4 }; // button은 구슬이 마지막에 있는 줄, top는 한줄에 구슬이 꽉차있는 줄
int gameFinishFlag = GAME_FINISH;
int initDisplayFlag = INIT;
int main() {
	int input;
	int menuSelect = 0;  // 게임시작: 0 , 게임 종료 : 1
	//→:224 77 |←:224 75 | spacebar: 32

	// 커서 안보이게 하기
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 10;
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	
	// 윈도우 창 셋팅
	system("mode con cols=37 lines=23 | title bubble game");

	// 게임 초기 화면
	initDisplay(menuSelect);
	while (1) {
		input = _getch();
		if (input == 32) {
			// 스페이스 바
			if (menuSelect == 1) {
				// 게임 종료
				return 0;
			}
			else {
				// 게임 시작
				start = clock();
				init_setting();
				initDisplayFlag = GAME;
				display();
				gameFinishFlag = GAME_IMG;
				while (1) {
					clock_t end = clock();
					if ((double)(end - start) / CLOCKS_PER_SEC > 5.0) {
						// 5초가 지나면
						add_bubble();
						start = end;
					}				
					if (gameFinishFlag == GAME_FINISH) break;
			   		input = _getch();
					if (input == 49) {
						gameFinishFlag = GAME_FINISH;
						break;
					}
					if (getkey(input)) {
						gameFinishFlag = GAME_FINISH;
						break;
					}
				}
				if (initDisplayFlag == GAME) {
					game_over_display();
					initDisplayFlag = INIT;
				}
			}
		}
		if (input == 224) {
			input = _getch();
			// 위 방향키
			if (input == 72 && menuSelect != 0) {
				menuSelect = 0;
				initDisplay(menuSelect);
			}
			else if (input == 80) {
				// 아래 방향키
				menuSelect = 1;
				initDisplay(menuSelect);
			}
		}
	}

	return 0;
}

void initDisplay(int direction) {
	// 하얀색으로 설정
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	system("cls");
	COORD Pos;
	Pos.X = 12;
	Pos.Y = 7;
	// 커서 옮기기
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	
	printf("Bubble Game");
	Pos.Y = 10;
	if (direction == 0) {
		Pos.X = 10;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		printf("▶  게임 시작");
	}
	else {
		Pos.X = 13;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		printf("게임 시작");
	}

	Pos.Y = 12;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	if (direction == 1) {
		Pos.X = 10;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		printf("▶  게임 종료");
	}
	else {
		Pos.X = 13;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		printf("게임 종료");
	}
	Pos.Y = 16;
	Pos.X = 10;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	printf("선택 : SpaceBar");

	Pos.X = 0;
	Pos.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
void displayPosition(int x, int y, int values) {
	
	COORD Pos;
	Pos.X = x * 2;
	Pos.Y = y;
	// 커서 옮기기
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	// 원하는 값 출력
	switch (values) {
	case 5:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		printf("■");
		break;

	case 0:
		printf("  ");
		break;

	case 6:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		printf("★");
		break;

	case 7:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), user_bubblecolor);
		printf("●");
		break;

	default:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), values);
		printf("●");
	}
	// 커서 다시 0,0 으로 설정
	Pos.X = 0;
	Pos.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
int getkey(int key) {
	//→:224 77 |←:224 75 | spacebar: 32
	if (gameFinishFlag == GAME_FINISH) {
		return 1;
	}
	
	if (key == 32) {
		if (result()) {
			return 1;
		}
	}
	if (key == 224) {
		key = _getch();
		if (key == 77) {
			if (user_bubble != 15) {
				background[18][user_bubble] = 0;
				background[17][user_bubble] = 0;
				background[18][user_bubble + 1] = 6;
				background[17][user_bubble + 1] = 7;
				displayPosition(user_bubble, 18 + 1, 0);
				displayPosition(user_bubble, 17 + 1, 0);
				displayPosition(user_bubble + 1, 18 + 1, 6);
				displayPosition(user_bubble + 1, 17 + 1, 7);
				user_bubble += 1;
			}
		}
		else if (key == 75) {
			if (user_bubble != 1) {
				background[18][user_bubble] = 0;
				background[17][user_bubble] = 0;
				background[18][user_bubble - 1] = 6;
				background[18][user_bubble - 1] = 6;
				background[17][user_bubble - 1] = 7;
				displayPosition(user_bubble, 18 + 1, 0);
				displayPosition(user_bubble, 17 + 1, 0);
				displayPosition(user_bubble - 1, 18 + 1, 6);
				displayPosition(user_bubble - 1, 17 + 1, 7);
				user_bubble -= 1;
			}
		}
	}
	return 0;
}

void random_color() {
	srand(time(NULL));
	user_bubblecolor = rand() % 4 + 1;
}

int add_bubble() {

	int sum = 0;
	//한줄씩 미루기
	for (int i = 15; i > 0; i--) {
		for (int j = 1; j < 16; j++) {
			background[i + 1][j] = background[i][j];
			displayPosition(j, i + 2, background[i][j]);
		}
	}
	//처음 줄에 구슬 생성
	for (int i = 1; i < 16; i++) {
		background[1][i] = rand() % 4 + 1;
		displayPosition(i, 2, background[1][i]);
	}
	for (int i = 1; i < 16; i++) {
		sum += background[16][i];
	}
	if (sum != 0) return 1;
	line.button++;
	line.top++;
	
	return 0;
}

int result() {
	int i, count = 0, real = -1, check = -1;
	int x, y, color = user_bubblecolor;
	struct buffer sameColor[100] = { 0 };
	struct reminder remin[100] = { 0 };

	for (i = 16; i > 0; i--) {
		if (background[i][user_bubble] != 0) break;
	}
	if (i == 16) {
		return 1;
	}
	i++;
	background[i][user_bubble] = color;
	displayPosition(user_bubble, i + 1, color);
	
	// 공을 발사했으니 새로운 공으로 대체한다.
	random_color();
	displayPosition(user_bubble, 17 + 1, 7);

	(sameColor[0]).y = user_bubble;
	(sameColor[0]).x = i;
	background[i][user_bubble] = 0;
	// 확인하기
	int temp_background[20][17] = { 0 };
	array_copy(background, temp_background);
	do {
		real++;
		x = (sameColor[real]).x;
		y = (sameColor[real]).y;
		if (temp_background[x + 1][y] == color) {//하탐색
			count++;
			(sameColor[count]).x = x + 1;
			(sameColor[count]).y = y;
			temp_background[x + 1][y] = 0;
		}
		else if (temp_background[x + 1][y] > 0 && temp_background[x + 1][y] < 5) { //하탐색 나머지 처리
			check++;
			(remin[check]).x = x + 1;
			(remin[check]).y = y;
			(remin[check]).color = temp_background[x + 1][y];
			temp_background[x + 1][y] = 0;
		}
		if (temp_background[x - 1][y] == color) {//상 탐색
			count++;
			(sameColor[count]).x = x - 1;
			(sameColor[count]).y = y;
			temp_background[x - 1][y] = 0;
		}
		if (temp_background[x][y + 1] == color) {//우탐색
			count++;
			(sameColor[count]).x = x;
			(sameColor[count]).y = y + 1;
			temp_background[x][y + 1] = 0;
		}
		else if (temp_background[x][y + 1] > 0 && temp_background[x][y + 1] < 5) { //우탐색 나머지 처리
			check++;
			(remin[check]).x = x;
			(remin[check]).y = y + 1;
			(remin[check]).color = temp_background[x][y + 1];
			temp_background[x][y + 1] = 0;
		}
		if (temp_background[x][y - 1] == color) {//좌탐색
			count++;
			(sameColor[count]).x = x;
			(sameColor[count]).y = y - 1;
			temp_background[x][y - 1] = 0;
		}
		else if (temp_background[x][y - 1] > 0 && temp_background[x][y - 1] < 5) { //좌탐색 나머지 처리
			check++;
			(remin[check]).x = x;
			(remin[check]).y = y - 1;
			(remin[check]).color = temp_background[x][y - 1];
			temp_background[x][y - 1] = 0;
		}
	} while (count > real);
	
	// 같은 색의 버블이 3개 이하일 때, 버블들을 지울 수 없다.
	if (count < 2) {
		background[i][user_bubble] = color;
	}
	else {
		// 버블을 지울 수 있다.
		// 추가 점수
		int addScore = 0;
		for (i = 0; i <= count; i++) {
			background[(sameColor[i]).x][(sameColor[i]).y] = 0;
			displayPosition((sameColor[i]).y, (sameColor[i]).x + 1, 0);
			addScore++;
		}
		// 버블들을 지울 수 있을 경우 연관되어 있는 버블들은 다 지워야 한다.
		// 버블들을 지워야 한다.
		for (i = 0; i <= check; i++) {
			struct reminder temp[500] = { 0 };
			int k, tmp_count = 0, flag = 1;
			temp[tmp_count].x = remin[i].x; temp[tmp_count].y = remin[i].y; temp[tmp_count].color = remin[i].color;
			background[temp[tmp_count].x][temp[tmp_count].y] = 0;
			displayPosition(temp[tmp_count].y, temp[tmp_count].x + 1, 0);
			addScore++;
			for (k = 0; k <= tmp_count; k++) {
				x = temp[k].x;
				y = temp[k].y;
				if (background[x + 1][y] > 0 && background[x + 1][y] < 5) {//하탐색
					tmp_count++;
					(temp[tmp_count]).x = x + 1;
					(temp[tmp_count]).y = y;
					(temp[tmp_count]).color = background[x + 1][y];
					background[x + 1][y] = 0;
					addScore++;
				}
				if (background[x - 1][y] > 0 && background[x - 1][y] < 5) {//상 탐색
					tmp_count++;
					(temp[tmp_count]).x = x - 1;
					(temp[tmp_count]).y = y;
					(temp[tmp_count]).color = background[x - 1][y];
					background[x - 1][y] = 0;
					addScore++;
				}
				else if (background[x - 1][y] == 5) {
					// 맨 위의 벽일 경우
					for (int h = 0; h <= tmp_count; h++) {
						background[temp[h].x][temp[h].y] = temp[h].color;
						displayPosition(temp[h].y, temp[h].x + 1, temp[h].color);
						addScore--;
					}
					flag = 0;
					break;
				}
				if (background[x][y + 1] > 0 && background[x][y + 1] < 5) {//우탐색
					tmp_count++;
					(temp[tmp_count]).x = x;
					(temp[tmp_count]).y = y + 1;
					(temp[tmp_count]).color = background[x][y + 1];
					background[x][y + 1] = 0;
					addScore++;
				}
				if (background[x][y - 1] > 0 && background[x][y - 1] < 5) {//좌탐색
					tmp_count++;
					(temp[tmp_count]).x = x;
					(temp[tmp_count]).y = y - 1;
					(temp[tmp_count]).color = background[x][y - 1];
					background[x][y - 1] = 0;
					addScore++;
				}
			}
			if (flag) {
				for (int g = 0; g <= tmp_count; g++) {
					displayPosition(temp[g].y, temp[g].x + 1, background[temp[g].x][temp[g].y]);
				}
			}
		}
		// 점수 추가
		score += addScore;
		COORD Pos;
		Pos.X = 0;
		Pos.Y = 0;
		// 커서 옮기기, 색상 하얀색으로 고정
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		printf("score : %d\n", score);
	}
	return 0;
}

void array_copy(int src[][WIDTH], int dst[][WIDTH]) {
	int* p = &src[0][0];

	int* end = &src[HEIGHT - 1][WIDTH - 1];
	int* q = &dst[0][0];
	while (p <= end) {
		*q++ = *p++;
	}
}

void init_setting() {
	//벽:5
	//빈칸:0
	//버블:1,2,3,4
	//유저:6,7
	// 구슬 색깔 새롭게 설정
	random_color();
	for (int i = 0; i < HEIGHT; i++) {
		for (int k = 0; k < WIDTH; k++) {
			if (i == 0 || i == HEIGHT - 1) background[i][k] = 5;
			else if (i == 1) {
				if (k == 0 || k == WIDTH - 1) background[i][k] = 5;
				else background[i][k] = rand() % 4 + 1;
			}
			else if (i == 2) {
				if (k == 0 || k == WIDTH - 1) background[i][k] = 5;
				else background[i][k] = rand() % 5;
			}
			else {
				if (k == 0 || k == WIDTH - 1) background[i][k] = 5;
				else background[i][k] = 0;
			}
		}
	}
	// 게임 유저 별 표시
	user_bubble = 8;
	background[18][8] = 6;
	background[17][8] = 7;
	score = 0;
}

void game_over_display() {
	system("cls");
	COORD Pos;
	Pos.X = 13;
	Pos.Y = 8;
	// 커서 옮기기
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf(" Game Over!!\n");
	Pos.X = 10;
	Pos.Y = 10;
	// 커서 옮기기
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	printf("당신의 점수 : %d\n", score);

	_getch();
	// 초기화면 셋팅
	system("cls");
	initDisplay(0);
}

void display() {
	
	printf("score : %d\n", score);
	//벽:5
	//빈칸:0
	//버블:1,2,3,4
	//유저:6,7
	for (int i = 0; i < 20; i++) {
		for (int k = 0; k < 17; k++) {
			switch (background[i][k]) {
			case 5:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				printf("■");
				break;

			case 0:
				printf("  ");
				break;

			case 6:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				printf("★");
				break;

			case 7:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), user_bubblecolor);
				printf("●");
				break;

			default:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), background[i][k]);
				printf("●");

			}
		}
		printf("\n");
	}
}
