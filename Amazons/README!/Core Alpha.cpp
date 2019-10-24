
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstring>
#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1
double eps = 1e-6;
using namespace std;
struct que
{
	int i;
	int j;
	int no;
};
que q[3000];
int currBotColor = 1; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int oo = 2000000000;


//最终决策
int tempx, tempy, tempxx, tempyy, tempxxx, tempyyy;

// 判断是否在地图内
int turnID;
double preva[8][8][8] = { 0 };//第一维黑方queen， 第二维白方queen， 第三维黑方king， 第四维白方king, 第五queen估值


inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}
inline double maxnum(double a, double b)
{
	if (a > b) return a;
	else return b;
}
inline double minnum(double a, double b)
{
	if (a < b) return a;
	else return b;
}

bool lg(int i, int j)
{
	if (i >= 0 && i < 8 && j >= 0 && j < 8 && gridInfo[i][j] != 2 && gridInfo[i][j] != 1 && gridInfo[i][j] != -1) return true;
	else return false;

}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}

inline bool eq(double a, double b)
{
	if (a < b + eps && a > b - eps) return true;
	else return false;

}
double eva1()
{
	memset(preva, 0, sizeof(preva));

	double oo = 2187;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (gridInfo[i][j] == 1)  preva[1][i][j] = preva[3][i][j] = 1, preva[2][i][j] = preva[4][i][j] = oo;
			if (gridInfo[i][j] == -1) preva[2][i][j] = preva[4][i][j] = 1, preva[1][i][j] = preva[3][i][j] = oo;
			if (gridInfo[i][j] == 2)  preva[1][i][j] = preva[2][i][j] = preva[3][i][j] = preva[4][i][j] = oo;

		}

	


	int read = 1, write = 1;

	for (int i = 0; i < 8; i++)//黑方的amazon
		for (int j = 0; j < 8; j++)
			if (gridInfo[i][j] == 1) q[write].no = 1, q[write].i = i, q[write].j = j, write++;

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
				if (lg(begini + dx[k] * delta1, beginj + dy[k] * delta1))
				{
					


					if (eq(preva[1][begini + dx[k] * delta1][beginj + dy[k] * delta1],0))
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


			if (lg(begini + dx[k], beginj + dy[k]))
			{
				


				if (eq(preva[3][begini + dx[k]][beginj + dy[k]],0))
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
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (gridInfo[i][j] == -1) q[write].no = 1, q[write].i = i, q[write].j = j, write++;

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
				if (lg(begini + dx[k] * delta1, beginj + dy[k] * delta1))
				{
					


					if (eq(preva[2][begini + dx[k] * delta1][beginj + dy[k] * delta1],0))
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


			if (lg(begini + dx[k], beginj + dy[k]))
			{
				


				if (eq(preva[4][begini + dx[k]][beginj + dy[k]],0))
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
	
	
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (eq(preva[1][i][j],0)) preva[1][i][j] = oo;
			if (eq(preva[2][i][j],0)) preva[2][i][j] = oo;
			if (eq(preva[3][i][j],0)) preva[3][i][j] = oo;
			if (eq(preva[4][i][j],0)) preva[4][i][j] = oo;
		}

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)//黑方的估值为正
		{
			if (eq(preva[1][i][j],preva[2][i][j]) && eq(preva[1][i][j],oo)) preva[5][i][j] = 0;
			if (eq(preva[1][i][j], preva[2][i][j]) && !eq(preva[1][i][j], oo)) { if (currBotColor == 1) preva[5][i][j] = 0.2; else preva[5][i][j] = -0.2; }
			if (preva[1][i][j] < preva[2][i][j]) preva[5][i][j] = 1;
			if (preva[1][i][j] > preva[2][i][j]) preva[5][i][j] = -1;

			if (eq(preva[3][i][j], preva[4][i][j]) && eq(preva[3][i][j], oo)) preva[6][i][j] = 0;
			if (eq(preva[3][i][j], preva[4][i][j]) && !eq(preva[3][i][j], oo)) { if (currBotColor == 1) preva[6][i][j] = 0.2; else preva[6][i][j] = -0.2; }
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




			if(turnID <= 15)
			{
			    if (gridInfo[i][j] == 1)
			{


				for (int k = 0; k < 8; k++)
				{
					for (int delta1 = 1; delta1 < 8; delta1++)
					{
						if (lg(i + dx[k] * delta1, j + dy[k] * delta1))
						{
							int x = i + dx[k] * delta1;
							int y = j + dy[k] * delta1;

							mb += preva[3][x][y] / preva[1][x][y];


						}
						else break;
					}
				}



			}
			if (gridInfo[i][j] == -1)
			{


				for (int k = 0; k < 8; k++)
				{
					for (int delta1 = 1; delta1 < 8; delta1++)
					{
						if (lg(i + dx[k] * delta1, j + dy[k] * delta1))
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


	
	if (turnID <= 20) evalu = 0.14 * T1 + 0.37 * T2 + 0.001 * p1 + 0.001 * p2 + 0.001 * mobil;
	else evalu = 0.30 * T1 + 0.25 * T2;


	double E = evalu * 100;



	if (currBotColor == 1) return E;

	else return -E;



}

double eva2(double maxval)
{
	int posCount = 0;
	for (int i = 0; i < GRIDSIZE; ++i) {
		for (int j = 0; j < GRIDSIZE; ++j) {
			if (gridInfo[i][j] != currBotColor) continue;
			for (int k = 0; k < 8; ++k) {
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++) {
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int l = 0; l < 8; ++l) {
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) {
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, currBotColor, true))
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

	for (int i = 0; i < GRIDSIZE; ++i) {
		for (int j = 0; j < GRIDSIZE; ++j) {
			if (gridInfo[i][j] != -currBotColor) continue;
			for (int k = 0; k < 8; ++k) {
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++) {
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;
					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))
						break;
					for (int l = 0; l < 8; ++l) {
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) {
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, -currBotColor, true))
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





double dostep(int nowstep, int nowcolor, int goalstep, double alpha, double beta)
{
	if (nowstep == goalstep)
	{
		double c;
	
		c = eva1();
		if(turnID > 23) c += eva2();
		
		
		return c;
	}


	int beginPos[3000][2], possiblePos[3000][2], obstaclePos[3000][2];
	int posCount = 0;

	for (int i = 0; i < GRIDSIZE; ++i)
	{
		for (int j = 0; j < GRIDSIZE; ++j)
		{

			if (gridInfo[i][j] != nowcolor) continue;

			for (int k = 0; k < 8; ++k)
			{
				for (int delta1 = 1; delta1 < GRIDSIZE; delta1++)
				{
					int xx = i + dx[k] * delta1;
					int yy = j + dy[k] * delta1;

					if (gridInfo[xx][yy] != 0 || !inMap(xx, yy))break;

					for (int l = 0; l < 8; ++l) {
						for (int delta2 = 1; delta2 < GRIDSIZE; delta2++) {
							int xxx = xx + dx[l] * delta2;
							int yyy = yy + dy[l] * delta2;
							if (!inMap(xxx, yyy))
								break;
							if (gridInfo[xxx][yyy] != 0 && !(i == xxx && j == yyy))
								break;
							if (ProcStep(i, j, xx, yy, xxx, yyy, nowcolor, true))
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
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += OBSTACLE;


				val[pick[i]] = dostep(nowstep + 1, -nowcolor, goalstep, alpha, beta);
				if (val[pick[i]] > alpha)
				{
					alpha = val[pick[i]];
					if (nowstep == 1) maxplan = pick[i];
				}
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] += nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= OBSTACLE;

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
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] -= nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] += nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] += OBSTACLE;


				val[pick[i]] = dostep(nowstep + 1, -nowcolor, goalstep, alpha, beta);
				if (val[pick[i]] < beta)
				{
					beta = val[pick[i]];

				}
				gridInfo[beginPos[pick[i]][0]][beginPos[pick[i]][1]] += nowcolor;
				gridInfo[possiblePos[pick[i]][0]][possiblePos[pick[i]][1]] -= nowcolor;
				gridInfo[obstaclePos[pick[i]][0]][obstaclePos[pick[i]][1]] -= OBSTACLE;

				if (beta < alpha) break;
			}





			return beta;

		}




	}
}























int main()
{

	srand(time(0));
	int x0, y0, x1, y1, x2, y2;

	// 初始化棋盘
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;

	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;



	cin >> turnID;

	// 读入到当前回合为止，自己和对手的所有行动，从而把局面恢复到当前回合
	currBotColor = grid_white; // 先假设自己是白方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合

		// 首先是对手行动
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			currBotColor = grid_black; // 第一回合收到坐标是-1, -1，说明我是黑方
		else
			ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // 模拟对方落子

																	// 然后是自己当时的行动
																	// 对手行动总比自己行动多一个
		if (i < turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // 模拟己方落子
		}
	}

	// 做出决策（你只需修改以下部分）



	
	double alpha = -oo - 1, beta = oo + 1;

	double goal = 0;
	
   	goal = dostep(1, currBotColor, 3, alpha, beta);

    
	int startX, startY, resultX, resultY, obstacleX, obstacleY;





	if (goal == -oo)
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
		obstacleX = -1;
		obstacleY = -1;
	}
	else
	{

		startX = tempx;
		startY = tempy;
		resultX = tempxx;
		resultY = tempyy;
		obstacleX = tempxxx;
		obstacleY = tempyyy;
	}







	// 决策结束，输出结果（你只需修改以上部分）
	cout << startX << ' ' << startY << ' ' << resultX << ' ' << resultY << ' ' << obstacleX << ' ' << obstacleY << endl;
	return 0;
}
