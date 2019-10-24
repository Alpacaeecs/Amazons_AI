#include "pch.h"
#include <iostream>
#include <easyx.h>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h> 
#include <cstring>
#include <cmath>
#include <algorithm>
#include <fstream>

using namespace std;












//基本结构部分（作者：刘鹏）
int board[9][120][9][9];//障碍黑方的为1，amazon黑方的为2，障碍白方的为-1，amazon为-2；
int game = 0; //开局的游戏总数目
int hand[9] = { 0 };//每一局下子的手数
int last = 0;//判断每一盘下完没有
int gamebotcolor[9] = { 0 };//每局的ai
int ailose = 0;//ai的失败次数
int playerlose = 0;//玩家的失败次数
#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
double eps = 1e-6;//浮点数判断
//队列结构体（作者不会使用库<queue>）
struct que
{
	int i;
	int j;
	int no;
};
que q[3000];
int currBotColor = -1;//ai默认白方
int playerColor = 1;//玩家默认黑方
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int oo = 2000000000;//无穷大
int tempx, tempy, tempxx, tempyy, tempxxx, tempyyy;//暂时决策传递方案



//存读取部分（作者：刘鹏）
//存盘
void savemydata()
{
	ofstream outfile("data.txt");
	outfile << game << " ";
	for (int i = 1; i <= game; i++)
		outfile << hand[i] << " ";
	
		outfile << last << " ";
	for (int i = 1; i <= game; i++)
		outfile << gamebotcolor[i] << " ";

	for (int i = 1; i <= game; i++)
		for (int j = 1; j <= hand[i]; j++)
			for (int k = 1; k <= 8; k++)
				for (int l = 1; l <= 8; l++)
					outfile << board[i][j][k][l] << " ";



}
void loadmydata()
{
	ifstream infile("data.txt");
	infile >> game;
	for (int i = 1; i <= game; i++)
		infile >> hand[i];
	
		infile >> last;
	for (int i = 1; i <= game; i++)
		infile >> gamebotcolor[i];

	for (int i = 1; i <= game; i++)
		for (int j = 1; j <= hand[i]; j++)
			for (int k = 1; k <= 8; k++)
				for (int l = 1; l <= 8; l++)
					infile >> board[i][j][k][l];
}




//决策准备部分（作者：刘鹏）
//决策棋盘
double preva[8][9][9] = { 0 };
//判断
inline bool inMap(int x, int y)
{
	if (x <= 0 || x >= 9 || y <= 0 || y >= 9)
		return false;
	return true;
}
//浮点最大值寻找
inline double maxnum(double a, double b)
{
	if (a > b) return a;
	else return b;
}
//浮点最小值寻找
inline double minnum(double a, double b)
{
	if (a < b) return a;
	else return b;
}
//检查落步是否合法
bool lg(int i, int j, int gameno, int step )
{
	if (i > 0 && i <= 8 && j > 0 && j <= 8 && board[gameno][step][i][j] != -1 
		&& board[gameno][step][i][j] != 1 && board[gameno][step][i][j] != -2 
		&& board[gameno][step][i][j] != 2) return true;
	else return false;

}
//浮点判断相等
inline bool eq(double a, double b)
{
	if (a < b + eps && a > b - eps) return true;
	else return false;

}
//模拟步数（来自botzone样例，作者未改动）
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color2, bool check_only, int step)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (board[game][step][x0][y0] != color2 || board[game][step][x1][y1] != 0)
		return false;
	if ((board[game][step][x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		board[game][step][x0][y0] = 0;
		board[game][step][x1][y1] = color2;
		board[game][step][x2][y2] = -1;
	}
	return true;
}








//棋局外观部分（作者：刘鹏）
//开局制图
void opening()
{
	initgraph(640, 480);
	cleardevice();

	setcolor(WHITE);
	POINT pts[] = { {0, 0}, {640, 0}, {640,480},{0, 480} };
	solidpolygon(pts, 4);
	
	IMAGE img;
	loadimage(&img, L"D:\\Amazons\\ConsoleApplication2\\开头.jpg", 300, 300);//先把图片放在工程项目下，使用L+"绝对路径"
	if(last == 0)putimage(170, 80, &img);
	IMAGE img4;
	loadimage(&img4, L"D:\\Amazons\\ConsoleApplication2\\开头2.jpg", 300, 300);//先把图片放在工程项目下，使用L+"绝对路径"
	if(last != 0) putimage(170, 80, &img4);
	
	IMAGE img7;
	loadimage(&img7, L"D:\\Amazons\\ConsoleApplication2\\按钮.jpg", 70, 70);
	putimage(570, 410, &img7);
	IMAGE img8;
	loadimage(&img8, L"D:\\Amazons\\ConsoleApplication2\\logof.jpg", 40, 40);
	putimage(0, 0, &img8);
	IMAGE img10;
	loadimage(&img10, L"D:\\Amazons\\ConsoleApplication2\\开局.jpg", 190, 130);
	putimage(223, 0, &img10);
	

}
//全部清空函数
void clearini()
{
	cleardevice();

	setcolor(WHITE);
	POINT pts[] = { {0, 0}, {640, 0}, {640,480},{0, 480} };
	solidpolygon(pts, 4);
}
//读盘界面
void pick()
{
	clearini();
	IMAGE img15;
	loadimage(&img15, L"D:\\Amazons\\ConsoleApplication2\\taichi.jpg", 640, 480);//先把图片放在工程项目下，使用L+"绝对路径"
	putimage(0, 0, &img15);
}
void readMySavings()
{
	clearini();
	int i = 0;
	
	IMAGE img1;
	loadimage(&img1, L"D:\\Amazons\\ConsoleApplication2\\存档1.jpg", 360, 70);//先把图片放在工程项目下，使用L+"绝对路径"
	IMAGE img2;
	loadimage(&img2, L"D:\\Amazons\\ConsoleApplication2\\存档2.jpg", 360, 70);//先把图片放在工程项目下，使用L+"绝对路径"
	IMAGE img3;
	loadimage(&img3, L"D:\\Amazons\\ConsoleApplication2\\存档3.jpg", 360, 70);//先把图片放在工程项目下，使用L+"绝对路径"
	IMAGE img4;
	loadimage(&img4, L"D:\\Amazons\\ConsoleApplication2\\存档4.jpg", 360, 70);//先把图片放在工程项目下，使用L+"绝对路径"
	IMAGE img5;
	loadimage(&img5, L"D:\\Amazons\\ConsoleApplication2\\存档5.jpg", 360, 70);//先把图片放在工程项目下，使用L+"绝对路径"
	IMAGE img6;
	loadimage(&img6, L"D:\\Amazons\\ConsoleApplication2\\按钮.jpg", 70, 70);
	
	putimage(570, 410, &img6);
	while (game != 0)
	{
		

		
		putimage(280, 0, &img1);
		i++;
		if (i == game) break;
		
		
		putimage(280, 80, &img2);
		i++;
		if (i == game) break;
		
		
		putimage(280, 160, &img3);
		i++;
		if (i == game) break;
		
		putimage(280, 240, &img4);
		
	}
	
	setbkcolor(WHITE);
	settextcolor(RED);
	RECT r1 = { 380, 420, 580, 460 };

	drawtext(_T("删除全部存档"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	
	while (i < 4)
	{
		putimage(280, 80 * i, &img5);
		i++;
	}


	
	
}
//玩家胜利
void youWin()
{
	clearini();
	
	IMAGE img;
	loadimage(&img, L"D:\\Amazons\\ConsoleApplication2\\胜利.jpg", 300, 300);//先把图片放在工程项目下，使用L+"绝对路径"
	putimage(170, 80, &img);

}
//玩家失败
void youLose()
{
	clearini();

	IMAGE img;
	loadimage(&img, L"D:\\Amazons\\ConsoleApplication2\\失败.jpg", 300, 300);//先把图片放在工程项目下，使用L+"绝对路径"
	putimage(170, 80, &img);
}
//等待操作
void waitForClick()
{
	MOUSEMSG Click1;
	Click1 = GetMouseMsg();
	while (Click1.uMsg != WM_LBUTTONDOWN)
	{
		Click1 = GetMouseMsg();
	}
	return;

}
//初始化
void boardini()
{
	for(int k = 0; k <= 99; k++)
		for (int i = 0; i <= 8; i++)
			for (int j = 0; j <= 8; j++)
			{
			
				board[game][k][i][j] = 0;
			}
	
	hand[game] = 1;
	board[game][1][1][3] = 2;
	board[game][1][1][6] = 2;
	board[game][1][3][1] = 2;
	board[game][1][3][8] = 2;
	

	board[game][1][6][1] = -2;
	board[game][1][8][3] = -2;
	board[game][1][8][6] = -2;
	board[game][1][6][8] = -2;

	


}
//画下此时的棋盘
void drawboard(int step)
{
	clearini();
	
	setlinecolor(BLACK);
	line(20, 20, 420, 20);
	line(20, 70, 420, 70);
	line(20, 120, 420, 120);
	line(20, 170, 420, 170);
	line(20, 220, 420, 220);
	line(20, 270, 420, 270);
	line(20, 320, 420, 320);
	line(20, 370, 420, 370);
	line(20, 420, 420, 420);

	line(20, 20, 20, 420);
	line(70, 20, 70, 420);
	line(120, 20, 120, 420);
	line(170, 20, 170, 420);
	line(220, 20, 220, 420);
	line(270, 20, 270, 420);
	line(320, 20, 320, 420);
	line(370, 20, 370, 420);
	line(420, 20, 420, 420);

	for(int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			if (board[game][step][i][j] == 2)
			{
				setfillcolor(BLACK);
				fillcircle(50 * j - 5, 50 * i - 5, 20);
			}
			if (board[game][step][i][j] == -2)
			{
				setfillcolor(LIGHTGRAY);
				fillcircle(50 * j - 5, 50 * i - 5, 20);

			}
			if (board[game][step][i][j] == -1 || board[game][step][i][j] == 1)
			{
				setfillcolor(BLUE);
				fillcircle(50 * j - 5, 50 * i - 5, 20);
			}
		}
	}
	setfillcolor(WHITE);
	
	if (currBotColor == 1)//ai是黑方
	{
		setbkcolor(WHITE);
		settextcolor(BLACK);
		RECT r1 = { 440, 20, 640, 40 };
		
		drawtext(_T("执黑：电脑"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		RECT r2 = { 440, 50, 640, 70 };
	
		drawtext(_T("执白：玩家"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	
	}
	else//ai是白方
	{
		setbkcolor(WHITE);
		settextcolor(BLACK);
		RECT r1 = { 440, 20, 640, 40 };
		
		drawtext(_T("执黑：玩家"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		RECT r2 = { 440, 50, 640, 70 };
		
		drawtext(_T("执白：电脑"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}


	RECT r3 = { 440, 90, 640, 130 };

	drawtext(_T("当前回合"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	TCHAR s[5];
	_stprintf_s(s, _T("%d"), hand[game] - 1);        // 高版本 VC 推荐使用 _stprintf_s 函数
	outtextxy(540, 130, s);

	IMAGE img7;
	loadimage(&img7, L"D:\\Amazons\\ConsoleApplication2\\按钮.jpg", 70, 70);
	putimage(570, 410, &img7);
}
//读盘界面
void drawboardbackward(int gameno, int step)
{
	clearini();

	setlinecolor(BLACK);
	line(20, 20, 420, 20);
	line(20, 70, 420, 70);
	line(20, 120, 420, 120);
	line(20, 170, 420, 170);
	line(20, 220, 420, 220);
	line(20, 270, 420, 270);
	line(20, 320, 420, 320);
	line(20, 370, 420, 370);
	line(20, 420, 420, 420);

	line(20, 20, 20, 420);
	line(70, 20, 70, 420);
	line(120, 20, 120, 420);
	line(170, 20, 170, 420);
	line(220, 20, 220, 420);
	line(270, 20, 270, 420);
	line(320, 20, 320, 420);
	line(370, 20, 370, 420);
	line(420, 20, 420, 420);

	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			if (board[gameno][step][i][j] == 2)
			{
				setfillcolor(BLACK);
				fillcircle(50 * j - 5, 50 * i - 5, 20);
			}
			if (board[gameno][step][i][j] == -2)
			{
				setfillcolor(LIGHTGRAY);
				fillcircle(50 * j - 5, 50 * i - 5, 20);

			}
			if (board[gameno][step][i][j] == -1 || board[gameno][step][i][j] == 1)
			{
				setfillcolor(BLUE);
				fillcircle(50 * j - 5, 50 * i - 5, 20);
			}
		}
	}
	setfillcolor(WHITE);
	if (gamebotcolor[gameno] == 1)//ai是黑方
	{
		setbkcolor(WHITE);
		settextcolor(BLACK);
		RECT r1 = { 440, 20, 640, 40 };

		drawtext(_T("执黑：电脑"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		RECT r2 = { 440, 50, 640, 70 };

		drawtext(_T("执白：玩家"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


	}
	else//ai是白方
	{
		setbkcolor(WHITE);
		settextcolor(BLACK);
		RECT r1 = { 440, 20, 640, 40 };

		drawtext(_T("执黑：玩家"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		RECT r2 = { 440, 50, 640, 70 };

		drawtext(_T("执白：电脑"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	RECT r3 = { 440, 90, 640, 130 };

	drawtext(_T("当前回合"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	TCHAR s[5];
	_stprintf_s(s, _T("%d"), step - 1);        // 高版本 VC 推荐使用 _stprintf_s 函数
	outtextxy(540, 130, s);
}
//行走提示
void tips(int x, int y, int step)
{
	int goalx = x - 1, goaly = y;
	while (board[game][step][goalx][goaly] == 0 && goalx > 0)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goalx--;
	}
	
	goalx = x + 1, goaly = y;
	while (board[game][step][goalx][goaly] == 0 && goalx <= 8)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goalx++;
	}
	goalx = x, goaly = y + 1;
	while (board[game][step][goalx][goaly] == 0 && goaly <= 8)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly++;
	}
	goalx = x, goaly = y - 1;
	while (board[game][step][goalx][goaly] == 0 && goaly > 0)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly--;
	}
	setfillcolor(WHITE);
	goalx = x - 1, goaly = y - 1;
	while (board[game][step][goalx][goaly] == 0 && goaly > 0 && goalx > 0)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly--;
		goalx--;
	}
	goalx = x + 1, goaly = y + 1;
	while (board[game][step][goalx][goaly] == 0 && goaly <= 8 && goalx <= 8)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly++;
		goalx++;
	}
	goalx = x - 1, goaly = y + 1;
	while (board[game][step][goalx][goaly] == 0 && goaly <= 8 && goalx > 0)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly++;
		goalx--;
	}
	goalx = x + 1, goaly = y - 1;
	while (board[game][step][goalx][goaly] == 0 && goaly > 0 && goalx <= 8)
	{
		setfillcolor(YELLOW);
		fillcircle(50 * goaly - 5, 50 * goalx - 5, 5);
		goaly--;
		goalx++;
	}
	setfillcolor(WHITE);
}
//检查玩家步法是否合格
bool check(int i, int j, int inii, int inij, int step)
{
	

	if (j == inij)
	{
		if (i < inii)
		{
			while (i < inii)
			{
				if (board[game][step][j][i] != 0) return false;
				
				i++;
			}

			return true;
		}
		if (i > inii)
		{
			while (i > inii)
			{
				if (board[game][step][j][i] != 0) return false;
				
				i--;
			}

			return true;
		}
	}//
	//同一行
	if (i == inii)
	{
		if (j < inij)
		{
			while (j < inij)
			{
				if (board[game][step][j][i] != 0) { return false; }
				j++;
			}

			return true;
		}
		if (j > inij)
		{
			while (j > inij)
			{
				if (board[game][step][j][i] != 0) { return false; }
				j--;
			}

			return true;
		}
	}
	//同一列
	if (j + i == inij + inii)
	{
		if (i < inii)
		{
			while (i < inii)
			{
				if (board[game][step][j][i] != 0) return false;
				i++;
				j--;
			}

			return true;
		}
		if (i > inii)
		{
			while (i > inii)
			{
				if (board[game][step][j][i] != 0) return false;
				i--;
				j++;
			}

			return true;
		}
	}
	if (j - i == inij - inii)
	{
		if (i < inii)
		{
			while (i < inii)
			{
				if (board[game][step][j][i] != 0) return false;
				i++;
				j++;
			}

			return true;
		}
		if (i > inii)
		{
			while (i > inii)
			{
				if (board[game][step][j][i] != 0) return false;
				i--;
				j--;
			}

			return true;
		}
	}
	//同一斜线
	
	else return false;
}
//amazon的合法移动
int blackgoto(MOUSEMSG blackGoTo, int step, int inij, int inii)
{
	int legal = 0;
	if (blackGoTo.uMsg == WM_LBUTTONDOWN)
	{
		int mousex = 0, mousey = 0;
		

		mousex = blackGoTo.x;
		mousey = blackGoTo.y;

		int i = 0, j = 0;

		i = (mousex - 20) / 50 + 1;
		j = (mousey - 20) / 50 + 1;
		
		if (i >= 1 && i <= 8 && j >= 1 && j <= 8 && board[game][step][j][i] == 0 && check(i, j, inii, inij, step))
		{
			board[game][step][j][i] = 2 * playerColor;
			legal = 1;
		}
	}
	return legal;
}
//放箭提示
void tipsArrow(int j, int i, int step)
{
	int nj, ni;
	nj = j + 1; ni = i;
	while (board[game][step][ni][nj] == 0 && nj <= 8)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		nj++;
	}
	nj = j - 1; ni = i;
	while (board[game][step][ni][nj] == 0 && nj > 0)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		nj--;
	}
	nj = j; ni = i + 1;
	while (board[game][step][ni][nj] == 0 && ni <= 8)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni++;
	}
	nj = j; ni = i - 1;
	while (board[game][step][ni][nj] == 0 && ni > 0)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni--;
	}
	nj = j - 1; ni = i - 1;
	while (board[game][step][ni][nj] == 0 && ni > 0 && nj > 0)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni--;
		nj--;
	}
	nj = j + 1; ni = i + 1;
	while (board[game][step][ni][nj] == 0 && ni <= 8 && nj <= 8)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni++;
		nj++;
	}
	nj = j - 1; ni = i + 1;
	while (board[game][step][ni][nj] == 0 && ni <= 8 && nj > 0)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni++;
		nj--;
	}
	nj = j + 1; ni = i - 1;
	while (board[game][step][ni][nj] == 0 && ni > 0 && nj <= 8)
	{
		setfillcolor(RED);
		fillcircle(50 * nj - 5, 50 * ni - 5, 5);
		ni--;
		nj++;
	}

	setfillcolor(WHITE);
}
//放箭
int arrowgoto(MOUSEMSG arrowGoTo, int step, int movetoj, int movetoi)
{
	int legal = 0;
	if (arrowGoTo.uMsg == WM_LBUTTONDOWN)
	{
		int mousex = 0, mousey = 0;

		mousex = arrowGoTo.x;
		mousey = arrowGoTo.y;

		int i = 0, j = 0;

		i = (mousex - 20) / 50 + 1;
		j = (mousey - 20) / 50 + 1;

		if (i >= 1 && i <= 8 && j >= 1 && j <= 8 && board[game][step][j][i] == 0 )
		{
			board[game][step][j][i] = 1 * playerColor;
			legal = 1;
		}
	}
	return legal;
}











//估值与Ai算法部分（作者：刘鹏）
//估值函数1
double eva1(int step)
{
	memset(preva, 0, sizeof(preva));

	double oo = 2187;
	for (int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)
		{
			if (board[game][step][i][j] == 2)  preva[1][i][j] = preva[3][i][j] = 1, preva[2][i][j] = preva[4][i][j] = oo;
			if (board[game][step][i][j] == -2) preva[2][i][j] = preva[4][i][j] = 1, preva[1][i][j] = preva[3][i][j] = oo;
			if (board[game][step][i][j] == -1 || board[game][step][i][j] == 1)  preva[1][i][j] = preva[2][i][j] = preva[3][i][j] = preva[4][i][j] = oo;

		}




	int read = 1, write = 1;

	for (int i = 1; i <= 8; i++)//黑方的amazon
		for (int j = 1; j <= 8; j++)
			if (board[game][step][i][j] == 2) q[write].no = 1, q[write].i = i, q[write].j = j, write++;

	while (read <= write)//查queen存在preva[1][i][j]黑方
	{
		int begini, beginj, times;

		begini = q[read].i;
		beginj = q[read].j;
		times = q[read].no + 1;


		for (int k = 0; k < 8; k++)
		{
			for (int delta1 = 1; delta1 < 8; delta1++)
			{
				if (lg(begini + dx[k] * delta1, beginj + dy[k] * delta1, game, step))
				{



					if (eq(preva[1][begini + dx[k] * delta1][beginj + dy[k] * delta1], 0))
					{
						q[write].i = begini + dx[k] * delta1;
						q[write].j = beginj + dy[k] * delta1;
						q[write].no = times;
						preva[1][q[write].i][q[write].j] = times;
						write++;
					}
					else
					{
						double minaed = minnum(times, preva[1][begini + dx[k] * delta1][beginj + dy[k] * delta1]);

						preva[1][begini + dx[k] * delta1][beginj + dy[k] * delta1] = minaed;
					}



				}
				else break;
			}
		}

		read++;

	}


	read = 1, write = 5;
	while (read <= write)//查king存在preva[3][i][j]黑方
	{
		int begini, beginj, times;

		begini = q[read].i;
		beginj = q[read].j;
		times = q[read].no + 1;


		for (int k = 0; k < 8; k++)
		{


			if (lg(begini + dx[k], beginj + dy[k], game, step))
			{



				if (eq(preva[3][begini + dx[k]][beginj + dy[k]], 0))
				{
					q[write].i = begini + dx[k];
					q[write].j = beginj + dy[k];
					q[write].no = times;
					preva[3][q[write].i][q[write].j] = times;
					write++;
				}
				else
				{
					double minaed = minnum(times, preva[3][begini + dx[k]][beginj + dy[k]]);
					preva[3][begini + dx[k]][beginj + dy[k]] = minaed;
				}



			}
			else continue;
		}


		read++;

	}



	read = 1, write = 1;
	for (int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)
			if (board[game][step][i][j] == -2) q[write].no = 1, q[write].i = i, q[write].j = j, write++;

	while (read <= write)//查queen存在preva[2][i][j]白方
	{
		int begini, beginj, times;

		begini = q[read].i;
		beginj = q[read].j;
		times = q[read].no + 1;


		for (int k = 0; k < 8; k++)
		{
			for (int delta1 = 1; delta1 < 8; delta1++)
			{
				if (lg(begini + dx[k] * delta1, beginj + dy[k] * delta1, game, step))
				{



					if (eq(preva[2][begini + dx[k] * delta1][beginj + dy[k] * delta1], 0))
					{
						q[write].i = begini + dx[k] * delta1;
						q[write].j = beginj + dy[k] * delta1;
						q[write].no = times;
						preva[2][q[write].i][q[write].j] = times;
						write++;
					}
					else
					{
						double minaed = minnum(times, preva[2][begini + dx[k] * delta1][beginj + dy[k] * delta1]);
						preva[2][begini + dx[k] * delta1][beginj + dy[k] * delta1] = minaed;
					}



				}
				else break;
			}
		}

		read++;

	}
	read = 1, write = 5;
	while (read <= write)//查king存在preva[4][i][j]白方
	{
		int begini, beginj, times;

		begini = q[read].i;
		beginj = q[read].j;
		times = q[read].no + 1;


		for (int k = 0; k < 8; k++)
		{


			if (lg(begini + dx[k], beginj + dy[k], game, step))
			{



				if (eq(preva[4][begini + dx[k]][beginj + dy[k]], 0))
				{
					q[write].i = begini + dx[k];
					q[write].j = beginj + dy[k];
					q[write].no = times;
					preva[4][q[write].i][q[write].j] = times;
					write++;
				}
				else
				{
					double minaed = minnum(times, preva[4][begini + dx[k]][beginj + dy[k]]);
					preva[4][begini + dx[k]][beginj + dy[k]] = minaed;
				}



			}
			else continue;
		}


		read++;

	}


	for (int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)
		{
			if (eq(preva[1][i][j], 0)) preva[1][i][j] = oo;
			if (eq(preva[2][i][j], 0)) preva[2][i][j] = oo;
			if (eq(preva[3][i][j], 0)) preva[3][i][j] = oo;
			if (eq(preva[4][i][j], 0)) preva[4][i][j] = oo;
		}

	for (int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)//黑方的估值为正
		{
			if (eq(preva[1][i][j], preva[2][i][j]) && eq(preva[1][i][j], oo)) preva[5][i][j] = 0;
			if (eq(preva[1][i][j], preva[2][i][j]) && !eq(preva[1][i][j], oo)) { if (currBotColor == 1) preva[5][i][j] = 0.5; else preva[5][i][j] = -0.4; }
			if (preva[1][i][j] < preva[2][i][j]) preva[5][i][j] = 1;
			if (preva[1][i][j] > preva[2][i][j]) preva[5][i][j] = -1;

			if (eq(preva[3][i][j], preva[4][i][j]) && eq(preva[3][i][j], oo)) preva[6][i][j] = 0;
			if (eq(preva[3][i][j], preva[4][i][j]) && !eq(preva[3][i][j], oo)) { if (currBotColor == 1) preva[6][i][j] = 0.5; else preva[6][i][j] = -0.4; }
			if (preva[3][i][j] < preva[4][i][j]) preva[6][i][j] = 1;
			if (preva[3][i][j] > preva[4][i][j]) preva[6][i][j] = -1;



		}

	double t1 = 0, t2 = 0;
	double p1 = 0, p2 = 0;
	double mb = 0, mw = 0;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			t1 += preva[5][i][j]; t2 += preva[6][i][j];

			p1 += 2 * (pow(2, -preva[2][i][j]) - pow(2, -preva[1][i][j]));

			p2 += minnum(1.0, maxnum(-1.0, (preva[3][i][j] - preva[4][i][j]) / 6.0));




			if (step <= 30)
			{
				if (board[game][step][i][j] == 2)
				{


					for (int k = 0; k < 8; k++)
					{
						for (int delta1 = 1; delta1 < 8; delta1++)
						{
							if (lg(i + dx[k] * delta1, j + dy[k] * delta1, game, step))
							{
								int x = i + dx[k] * delta1;
								int y = j + dy[k] * delta1;

								mb += preva[3][x][y] / preva[1][x][y];


							}
							else break;
						}
					}



				}
				if (board[game][step][i][j] == -2)
				{


					for (int k = 0; k < 8; k++)
					{
						for (int delta1 = 1; delta1 < 8; delta1++)
						{
							if (lg(i + dx[k] * delta1, j + dy[k] * delta1, game, step))
							{
								int x = i + dx[k] * delta1;
								int y = j + dy[k] * delta1;

								mw += preva[4][x][y] / preva[2][x][y];


							}
							else break;
						}
					}


				}
			}

		}


	double mobil = (mb - mw);

	if (p1 < 1e-2) p1 = 0;
	if (p2 < 1e-2) p2 = 0;

	double T1 = t1;
	double T2 = t2;
	double evalu = 0;



	if (step <= 8) evalu = 0.14 * T1 + 0.37 * T2 + 0.13 * p1 + 0.13 * p2 + 0.20 * mobil;
	else if (step <= 30) evalu = 0.30 * T1 + 0.25 * T2 + 0.20 * p1 + 0.20 * p2 + 0.05 * mobil;
	else evalu = 0.80 * T1 + 0.10 * T2 + 0.05 * p1 + 0.05 * p2;

	double E = evalu * 100;



	if (currBotColor == 1) return E;

	else return -E;



}
//估值函数2
double eva2(double maxval, int step)
{
	int posCount = 0;
	for (int i = 1; i <= GRIDSIZE; ++i) 
	{
		for (int j = 1; j <= GRIDSIZE; ++j) 
		{
			if (board[game][step][i][j] != currBotColor * 2) continue;
			
			for (int k = 0; k < 8; ++k) 
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++) 
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;
					if (board[game][step][xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int l = 0; l < 8; ++l) 
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) 
						{
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[game][step][xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, currBotColor * 2, true, step))
							{
								posCount++;

							}
						}

					}
				}

			}
		}
	}
	if (posCount < maxval) return posCount;

	for (int i = 1; i <= GRIDSIZE; ++i) 
	{
		for (int j = 1; j <= GRIDSIZE; ++j) 
		{
			if (board[game][step][i][j] != -currBotColor * 2) continue;
			for (int k = 0; k < 8; ++k) 
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++) 
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;
					if (board[game][step][xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int l = 0; l < 8; ++l) 
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) 
						{
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[game][step][xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, -currBotColor * 2, true, step))
							{
								posCount--;
								if (posCount < maxval) return posCount;
							}
						}

					}
				}

			}
		}
	}
	return posCount;
}
//递归搜索
double dostep(int nowstep, int nowcolor, int goalstep, double alpha, double beta, int step)
{
	if (nowstep == goalstep)
	{
		double c;
		if (step <= 52)
			c = eva1(step);
		else
			c = eva2(alpha, step);

		return c;
	}


	int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];
	int posCount = 0;

	for (int i = 1; i <= GRIDSIZE; ++i)
	{
		for (int j = 1; j <= GRIDSIZE; ++j)
		{

			if (board[game][step][i][j] != 2 * nowcolor) continue;

			for (int k = 0; k < 8; ++k)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;

					if (board[game][step][xx][yy] != 0 || !inMap(xx, yy))break;

					for (int l = 0; l < 8; ++l) 
					{
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) 
						{
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (board[game][step][xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, 2 * nowcolor, true, step))
							{
								posCount++;
								beginPos[posCount][0] = i;
								beginPos[posCount][1] = j;
								possiblePos[posCount][0] = xx;
								possiblePos[posCount][1] = yy;
								obstaclePos[posCount][0] = xxx;
								obstaclePos[posCount][1] = yyy;
							}
						}

					}
				}

			}
		}
	}

	if (posCount == 0)
	{


		if (nowcolor == currBotColor) return -oo;
		if (nowcolor == -currBotColor) return oo;
	}

	else
	{
		double val[3000] = { 0 };
		int posc = posCount;


		int pick[3000] = { 0 };

		for (int i = 1; i <= posc; i++) pick[i] = i;

		random_shuffle(pick + 1, pick + posc + 1);

		if (nowcolor == currBotColor)//我行动
		{
			int maxplan;
			for (int i = 1; i <= posc; i++)
			{
				board[game][step][beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= 2 * nowcolor;
				board[game][step][possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += 2 * nowcolor;
				board[game][step][obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += -1;


				val[pick[i]] = dostep(nowstep + 1, -nowcolor, goalstep, alpha, beta, step);

				if (val[pick[i]] > alpha)
				{
					alpha = val[pick[i]];
					if (nowstep == 1) maxplan = pick[i];
				}

				board[game][step][beginPos[pick[i]][0]][beginPos[pick[i]][1]] += 2 * nowcolor;
				board[game][step][possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= 2 * nowcolor;
				board[game][step][obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= -1;

				if (beta < alpha) break;
			}



			if (nowstep == 1)
			{
				tempx = beginPos[maxplan][0];
				tempy = beginPos[maxplan][1];
				tempxx = possiblePos[maxplan][0];
				tempyy = possiblePos[maxplan][1];
				tempxxx = obstaclePos[maxplan][0];
				tempyyy = obstaclePos[maxplan][1];

			}

			return alpha;
		}

		else//他行动
		{


			for (int i = 1; i <= posc; i++)
			{
				board[game][step][beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= 2 * nowcolor;
				board[game][step][possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += 2 * nowcolor;
				board[game][step][obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += 1;


				val[pick[i]] = dostep(nowstep + 1, -nowcolor, goalstep, alpha, beta, step);
				if (val[pick[i]] < beta)
				{
					beta = val[pick[i]];

				}
				board[game][step][beginPos[pick[i]][0]][beginPos[pick[i]][1]] += 2 * nowcolor;
				board[game][step][possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= 2 * nowcolor;
				board[game][step][obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= 1;

				if (beta < alpha) break;
			}





			return beta;

		}




	}
}
//应对策略集合
//白色方应对策略
int putStrategy1(int step)
{
	double alpha = -oo - 1, beta = oo + 1;

	double goal = 0;
	
		goal = dostep(1, currBotColor, 3, alpha, beta, step);

	
	
	if (goal == -oo)
		return 0;
	else
	{
		board[game][step][tempx][tempy] = 0;
		board[game][step][tempxx][tempyy] = 2 * currBotColor;
		board[game][step][tempxxx][tempyyy] = 1 * currBotColor;
		return 1;
	}
	
}







//棋局局势判断传递部分（作者：刘鹏）
//判断白色是否失败
bool whiteGameOver(int step)
{
	int deadamazon = 0;
	int playeri, playerj;
	for (int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)
		{
			if (board[game][step][i][j] == -2)
			{
				playeri = i;
				playerj = j;
				int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				int dy[8] = { 1, 1, 0, -1, -1, 1, 0, -1 };
				int ways = 0;
				for (int l = 0; l < 8; l++)
				{
					if (playeri + dy[l] > 8 || playeri + dy[l] == 0 || playerj + dx[l] > 8 || playerj + dx[l] == 0) ways++;
					else if (board[game][step][playeri + dy[l]][playerj + dx[l]] != 0) ways++;
				}
				if (ways == 8) deadamazon++;
			}

		}
	if (deadamazon == 4) return true;
	else return false;
}
//判断黑方是否失败
bool blackGameOver(int step)
{
	int deadamazon = 0;
	int playeri, playerj;
	for(int i = 1; i <= 8; i++)
		for (int j = 1; j <= 8; j++)
		{
			if (board[game][step][i][j] == 2)
			{
				playeri = i;
				playerj = j;
				int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				int dy[8] = { 1, 1, 0, -1, -1, 1, 0, -1 };
				int ways = 0;
				for (int l = 0; l < 8; l++)
				{
					if (playeri + dy[l] > 8 || playeri + dy[l] == 0 || playerj + dx[l] > 8 || playerj + dx[l] == 0) ways++;
					else if (board[game][step][playeri + dy[l]][playerj + dx[l]] != 0) ways++;
				}
				if (ways == 8) deadamazon++;
			}

		}
	if (deadamazon == 4) return true;
	else return false;
}
//复制棋盘给ai并加步数
void copyForAi()
{
	hand[game]++;
	for (int i = 0; i <= 8; i++)
		for (int j = 0; j <= 8; j++)
			board[game][hand[game]][i][j] = board[game][hand[game] - 1][i][j];
}






//玩家操作部分（作者：刘鹏）
//玩家走子射箭
int playerdone(MOUSEMSG player)
{
	int truestepdone = 0;
    
	if(player.uMsg == WM_LBUTTONDOWN)
	{
		 
		
			int mousex = 0, mousey = 0;

			mousex = player.x;
			mousey = player.y;

			if (mousex > 570 && mousey > 410)
			{
				last = 1;//暂停
				return 1;
			}

			int i = 0, j = 0;

			i = (mousex - 20) / 50 + 1;
			j = (mousey - 20) / 50 + 1;

			if (i >= 1 && i <= 8 && j >= 1 && j <= 8 && board[game][hand[game]][j][i] == 2 * playerColor)
			{
				hand[game]++;
				for (int k = 0; k <= 8; k++)
					for (int l = 0; l <= 8; l++)
						board[game][hand[game]][k][l] = board[game][hand[game] - 1][k][l];

				

				
				board[game][hand[game]][j][i] = 0;
			    drawboard(hand[game]);
				tips(j, i, hand[game]);

				MOUSEMSG blackGoTo;
				blackGoTo = GetMouseMsg();
				while (blackgoto(blackGoTo, hand[game], j, i) == 0)
				{
					blackGoTo = GetMouseMsg();
				}
				drawboard(hand[game]);
				
				int movetox = (blackGoTo.x - 20) / 50 + 1;
				int movetoy = (blackGoTo.y - 20) / 50 + 1;
				tipsArrow(movetox, movetoy, hand[game]);
				
				MOUSEMSG arrowGoTo;
				arrowGoTo = GetMouseMsg();
				while (arrowgoto(arrowGoTo, hand[game], movetox, movetoy) == 0)
				{
					arrowGoTo = GetMouseMsg();
				}
				drawboard(hand[game]);
				
				
				
				
				
				truestepdone = 1;
			}

	}

	return truestepdone;
}




//结尾界面（作者：刘鹏）
void outGame()
{
	clearini();
	IMAGE img9;
	loadimage(&img9, L"D:\\Amazons\\ConsoleApplication2\\logof.jpg", 340, 340);
	putimage(150, 70, &img9);

}





//主函数
int main()
{
	loadmydata();
	while(true)
	{ 
		
		opening();
		//开始界面读取
		int flagopen = 0;
		setfillcolor(WHITE);
		MOUSEMSG m;
		while (true)
		{
			// 获取一条鼠标消息
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				if (m.x < 400 && m.x > 232 && m.y < 205 && m.y > 130)
				{
					clearini();
					flagopen = 1;
					break;
				}
				else if (m.x < 400 && m.x > 232 && m.y > 238 && m.y < 309)
				{
					clearini();
					flagopen = 2;
					break;
				}
				else if (m.x > 570 && m.y > 410)
				{
					flagopen = 3;
					break;
				}
				
			}
			
			if (flagopen) break;
		}
		//获取第一个鼠标信息得知是读取存档还是新游戏


		







		//新游戏
		if (flagopen == 1)
		{
			game++;//游戏加一
			if (last == 0)
			{
				boardini();//版面清空
				pick();
				MOUSEMSG borw;
				while (true)
				{
					// 获取一条鼠标消息
					borw = GetMouseMsg();
					if (borw.uMsg == WM_LBUTTONDOWN)
					{
						if (borw.x >= 320)
						{
							playerColor = 1, currBotColor = -1;
							gamebotcolor[game] = currBotColor;
							break;
						}
						else
						{
							playerColor = -1, currBotColor = 1;
							gamebotcolor[game] = currBotColor;
							break;
						}

					}
				}
			}
			else
			{
				memcpy(board[game][1], board[game - 1][hand[game - 1]], sizeof(board[game][1]));
				last = 0;
				playerColor = -gamebotcolor[game - 1];
				currBotColor = gamebotcolor[game - 1];
				hand[game] = 1;
			}
			
			
			
			drawboard(hand[game]);//画图
			

			if (playerColor == -1)
			{	
				waitForClick();
				copyForAi();
				int whiteok = 0;


				whiteok = putStrategy1(hand[game]);

				drawboard(hand[game]);
			}

			while (true)
			{
				//player操作部分
				MOUSEMSG player;
				player = GetMouseMsg();
				

				while(playerdone(player) == 0)
				{
					player = GetMouseMsg();

				}
				
				if (last == 1)
				{
					setfillcolor(WHITE);
					break;
				}

				//判断白子是否失败
				bool whitelost;
				whitelost = whiteGameOver(hand[game]);
				if (whitelost)
				{
					waitForClick();
					if(playerColor == 1) youWin();
					else youLose();
					waitForClick();
					break;

				}
				//复制棋盘
				copyForAi();
				
				int whiteok = 0;
				
				
				whiteok = putStrategy1(hand[game]);
				
				drawboard(hand[game]);

				
				
				//判断玩家是否失败
				bool blacklost;
				blacklost = blackGameOver(hand[game]);
				if (blacklost)
				{
					waitForClick();
					if(playerColor == 1) youLose();
					else youWin();
					waitForClick();
					break;
				}

			}
			
		}
		
		//读取存档
		if (flagopen == 2)
		{






			readMySavings();
			MOUSEMSG mx;
			int loadno = 0;
			while (loadno == 0)
			{
				
				mx = GetMouseMsg();
				while (mx.uMsg != WM_LBUTTONDOWN)
				{
					mx = GetMouseMsg();
				}

				if (mx.x > 250 && mx.y < 80) loadno = 1;
				else if (mx.x > 250 && mx.y < 160) loadno = 2;
				else if (mx.x > 250 && mx.y < 240) loadno = 3;
				else if (mx.x > 250 && mx.y < 320) loadno = 4;
				else if (mx.x > 570 && mx.y > 410) loadno = 5;
				else if (mx.x > 380 && mx.x < 560 && mx.y > 430)
				{
					game = 0;
					memset(hand, 0, sizeof(hand));
					last = 0;
					memset(gamebotcolor, 0, sizeof(gamebotcolor));
					memset(board, 0, sizeof(board));
					loadno = 5;
					readMySavings();
					waitForClick();
				}
			}
			
			
			if (loadno != 5 && loadno <= game)
			{
				drawboardbackward(loadno, 1);
				int loadnoStep = 1;
				while (loadnoStep < hand[loadno])
				{
					mx = GetMouseMsg();
					while (mx.uMsg != WM_LBUTTONDOWN)
					{
						mx = GetMouseMsg();
					}
					loadnoStep++;
					drawboardbackward(loadno, loadnoStep);
					setfillcolor(WHITE);
				}
				waitForClick();
				
			}
			
			
			
			
			setfillcolor(WHITE);


			
		}
	


		//存盘
		if (flagopen == 3)
		{
			
			savemydata();
			break;
		}
	}
	
	outGame();


	
	return 0;
}



























































// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
