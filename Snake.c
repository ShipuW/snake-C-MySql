/********************************************************
* Project: Snake(̰ʳ��)                                *
* Email:   424065448@qq.com                             *
* Date:    2013-05-01 00:28:39                          *
********************************************************/
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "mysql.h" 
#include "pcc32.h"
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "libmysql")
#pragma comment(lib, "pcc32_vc6.lib")

// �����ͼ�ĳߴ缰����(��ʹ��˫�ַ�����)
#define MAP_WIDTH     40
#define MAP_HEIGHT    40
#define OFFSET_X      1
#define OFFSET_Y      1
#define TOTAL_WIDTH   (MAP_WIDTH+OFFSET_X*2)
#define TOTAL_HEIGHT  (MAP_HEIGHT+OFFSET_Y*2)
#define GotoMap(x,y)  gotoTextPos((uint8)(x)*2,(uint8)(y))

// �����ߵ���ز���
#define SNAKE_MIN_LEN   5

// �����ͼ���״̬,�ֱ�Ϊ[�ո�|��ͷ|����|��β|ʳ��]
#define BS_SPACE    0
#define BS_HEAD     1
#define BS_BODY     2
#define BS_TAIL     3
#define BS_FOOD     4
#define BS_TAILu    5
#define BS_TAILd    6
#define BS_TAILl    7
#define BS_TAILr    8
#define BS_UP       9
#define BS_DOWN     10
#define BS_LEFT     11
#define BS_RIGHT    12///��֮��˳���Ӧ�����ڹ���
									
// ������������ɫ,��״,˳��ͬ��.
PCCOLOR myColors[] = {CYAN, MAGENTA, RED,   GREEN, YELLOW, GREEN, GREEN, GREEN, GREEN, WHITE, WHITE, WHITE, WHITE};
char mySharps[][3] = {"  ",  "��",   "��",   "��",  "��",   "��",  "��",  "��", "��" , "��",  "�x",  "��",   "��"};

// �����ߵ��˶�����[��|��|��|��] case�ж�ʹ��
#define DIR_UP    1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define DIR_RIGHT 4

///////////////////////�ṹ��
typedef struct _POINT2D
{
	uint8 x, y;
}POINT2D, *PPOINT2D;
struct tm * timeinfo;

///////////////////////��������
POINT2D mySnake[MAP_WIDTH*MAP_HEIGHT] = {{0}};
POINT2D myFood = {0};
int snakeLength = SNAKE_MIN_LEN;
int snakeDir = DIR_RIGHT;
int isFood = 0;
int isOver = 0;
int isPause = 1;
int mul;
static int ms=200;
static int S;
char sql_insert[200]; 
time_t rawtime;
uint16 ch1,ch2,ch3,ch4,ch5;

///////////////////////��������
int isInSnake(int x, int y);
void drawMap(void);
void initSnake(void);
void drawSnake(void);
void moveSnake(void);
void drawBlock(int x, int y, int bs);
void drawFood(void);
void doGame(void);
void gameOver(void);
void replay(void);
void info(void);
void level(void);
void db_in(int score);
void db_out(void);
char* input_name(void);


///////////////////////������
int main()
{
	initSnake();
	info();
	drawSnake();
	level();
	doGame();
	gameOver();
	return 0;
}

///////////////////////���к���
void initSnake()
{
	int i;
	for (i = 0; i < SNAKE_MIN_LEN; i++)
	{
		mySnake[i].x = 10 - i;
		mySnake[i].y = 3;//////(10,3)���������ڷ�
	}
	fixConsoleSize((TOTAL_WIDTH) * 2, TOTAL_HEIGHT);
	setCursorVisible(0);
	setConsoleTitle("Snake");
}

void info(void)
{	
	drawMap();
	setTextColor(WHITE);
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2-1);
	printf("Click F1 to read info !\n");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2+1);
	printf("Click F2 to see the ranking!\n");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2+3);
	printf("Click ENTER to Start directly!\n");
	ch2 = jkGetKey();
	if(ch2 == JK_F1){
		drawMap();
		setTextColor(WHITE);
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4);
		printf("����һ��̰������Ϸ��\n\n");
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4 +2);
		printf("��������Ʒ���\n\n");	
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4 +4);
		printf("�Ե�Խ�࣬�ܵ�Խ�죡\n\n");
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4 +6);
		printf("��ʱ�ɰ�ESC�˳���\n\n");
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4 +8);
		printf("���ո����ʼ/��ͣ��\n\n");
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4 +10);
		printf("development log ���� F12\n\n");
		ch3=jkGetKey();
		if (ch3 == JK_F12){
		
			drawMap();
			setTextColor(WHITE);
			GotoMap(3, 3);
			printf("�������漰c���Գ�����ƻ���֪ʶ��");
			GotoMap(3, 5);
			printf("����VC��֧��graphic.h���ʲ���ascII���ͼ�μ�¼��");
			GotoMap(3, 7);
			printf("��������Ϊ����ֺ���ִ�У�");
			GotoMap(3, 9);
			printf("initSnake()info()drawSnake()level()doGame()gameOver()");
			GotoMap(3, 11);
			printf("�����������͡������������궨�塢���顢�ṹ���͵�֪ʶ��");
			GotoMap(3, 13);
			printf("�����������ⲿpcc32.hͷ�ļ��Ż��������ܡ�");
			GotoMap(3, 15);
			printf("�Ż����ݰ���jkGetKey������getch��jkHasKey��kbhit���Ż���װ����ѯ�㷨���á�");
			GotoMap(3, 17);
			printf("���ݲ��ü򵥶�ά�����¼���߼���Ϊ���ܡ�");
			GotoMap(3, 19);
			printf("���ְ�������ݿ��¼���ݣ����ڿ�ʵ������������");
			GotoMap(3, 35);
			printf("��繤��ѧԺ0201202�� ѧ�ţ�2012210754 ��������ʿ��");
			jkGetKey();
			info();

		}else if(ch3 == JK_ESC ){
			info();
		}
	}else if(ch2 == JK_F2)
	{
		
		drawMap();
		setTextColor(WHITE);	
		db_out();
		
	}else if(ch2 == JK_ENTER){
		return;
	}else{
		info();
	}
		drawMap();
}

void drawSnake(void)
{
	int i,j,k;

	drawBlock(mySnake[0].x, mySnake[0].y, BS_HEAD);
	for (i = 1; i < snakeLength - 1; i++)
    drawBlock(mySnake[i].x, mySnake[i].y, BS_BODY);
	j=mySnake[snakeLength - 1].x-mySnake[snakeLength - 2].x;
	k=mySnake[snakeLength - 1].y-mySnake[snakeLength - 2].y;
	if (j== 1&&k== 0){drawBlock(mySnake[snakeLength-1].x, mySnake[snakeLength-1].y, BS_TAILl);}
	else if (j==-1&&k== 0){drawBlock(mySnake[snakeLength-1].x, mySnake[snakeLength-1].y, BS_TAILr);}
	else if (j== 0&&k== 1){drawBlock(mySnake[snakeLength-1].x, mySnake[snakeLength-1].y, BS_TAILu);}
	else if (j== 0&&k==-1){drawBlock(mySnake[snakeLength-1].x, mySnake[snakeLength-1].y, BS_TAILd);}
}

void level(void)
{

	drawMap();
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4-2);
	printf("  ��ѡ�񼶱�");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4-1);
	printf("  1. ���ţ�����X1��");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4);
	printf("  2. ��ͨ������X2��");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4+1);
	printf("  3. ��ʦ������X4��");
	ch4 = jkGetKey();
	switch(ch4)
	{
		case 0x0031:ms=200;break;
		case 0x0032:ms=100;break;
		case 0x0033:ms=50;break;
		default:break;
	}
	clearText();
	drawMap();
}




void doGame(void)
{
	isPause = 0;
	while (!isOver)
	{
		if (!isPause)
    {
		moveSnake();
		if (!isFood)            
			drawFood();
    }
    delayMS(ms - snakeLength*2);
    if (jkHasKey()) ///�첽��ⰴ��
    {
		switch (jkGetKey())
		{
			case JK_UP:
				if (snakeDir != DIR_DOWN)
				snakeDir = DIR_UP;
			break;
			case JK_DOWN:
				if (snakeDir != DIR_UP)
				snakeDir = DIR_DOWN;
			break;
			case JK_LEFT:
				if (snakeDir != DIR_RIGHT)
				snakeDir = DIR_LEFT;
			break;
			case JK_RIGHT:
				if (snakeDir != DIR_LEFT)
				snakeDir = DIR_RIGHT;
			break;
			case JK_ESC: //��Ϸ����
				isOver = !isOver;
			break;
			case JK_SPACE: //��ͣ
				isPause = !isPause;
				break;
			default:
			break;
      }
    }
  }
}

void gameOver(void)
{
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2 -1);
	setTextColor(YELLOW);
	S= snakeLength - SNAKE_MIN_LEN;
	printf("Game Over! Score: %d.\n", (250-ms)/50*S);
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2);
	printf("��F1��������");
	GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 2+1);
	printf("Click Enter to retry!");
	ch1 = jkGetKey();
		if(ch1 == JK_ENTER){replay();}else if(ch1 == JK_F1){db_in(S);}else if(ch1 == JK_ESC){}else{gameOver();}
}

void replay(void)
{
	int i;
	for(i=0;i<=snakeLength;i++)
	{mySnake[i].x=0;mySnake[i].x=0;}
	snakeLength = SNAKE_MIN_LEN;
	snakeDir = DIR_RIGHT;
	isOver=0;
	isFood=0;
	main();
}

///////////////////////�����ǹ��ܺ���
void drawMap(void)
{

	int i, j;
	for (i=-1,        j=-1;        j<MAP_WIDTH ;j++) if (i!=j){drawBlock(j, i, BS_DOWN);}else{drawBlock(j, i, BS_SPACE);}
	for (i=-1,        j=-1;        i<MAP_HEIGHT;i++) if (i!=j){drawBlock(j, i, BS_RIGHT);}
	for (i=MAP_HEIGHT,j= 0;        j<MAP_WIDTH ;j++) drawBlock(j, i, BS_UP);
	for (i=0,         j=MAP_WIDTH; i<MAP_HEIGHT;i++) drawBlock(j, i, BS_LEFT);
	for (i = 0; i < MAP_HEIGHT; i++)
	{	
		for (j = 0; j < MAP_WIDTH; j++)
		{
			drawBlock(j, i, BS_SPACE);
		}
	}
}
void moveSnake(void)
{
	int i;
	int hx = mySnake[0].x;
	int hy = mySnake[0].y;
	switch (snakeDir)
	{
		case DIR_UP:
			hy--;
		break;
		case DIR_DOWN:
			hy++;
		break;
		case DIR_LEFT:
			hx--;
		break;
		case DIR_RIGHT:
			hx++;
		break;
		default:
		break;
	}
	if (hx < 0 || hx >= MAP_WIDTH || hy < 0 || hy >= MAP_HEIGHT || isInSnake(hx, hy))
	{
		isOver = 1;
		return;
	}
	if (hx == myFood.x && hy == myFood.y)
	{
		snakeLength++;
		isFood = 0;
	}
	else
    drawBlock(mySnake[snakeLength-1].x, mySnake[snakeLength-1].y, BS_SPACE);
	for (i = snakeLength - 1; i >= 0; i--)
	{
		mySnake[i+1].x = mySnake[i].x;
		mySnake[i+1].y = mySnake[i].y;
	}
	mySnake[0].x = hx;
	mySnake[0].y = hy;
	drawSnake();
	
	return;
}

void drawBlock(int x, int y, int bs)
{
	GotoMap((uint8)(OFFSET_X + x), (uint8)(OFFSET_Y + y));
	setTextColor(myColors[bs]);
	printf("%2s", mySharps[bs]);
	return;
}

int isInSnake(int x, int y)
{
	int i;
	for (i = 0; i < snakeLength; i++)
		if (x == mySnake[i].x && y == mySnake[i].y)
		return 1;
	return 0;
}

void drawFood(void)
{
	//srand((uint32)time(NULL));
	do
	{
		myFood.x = rand() % MAP_WIDTH;
		myFood.y = rand() % MAP_HEIGHT;
	}
	while (isInSnake(myFood.x, myFood.y));
	drawBlock(myFood.x, myFood.y, BS_FOOD);
	isFood = 1;
}

void db_in(int score)
{
	int res;
	char* name;
    MYSQL mysql;
  
    mysql_init(&mysql);	// ��ʼ��mysql�ṹ  
						//mysql_real_connect()�����Ĺ���������һ��MYSql���ݿ������,
						//MYSQL�ṹ��ַ��&mysql��host���������ַ��localhost��
						//�û�����root,������root,���ݿ���mydb,�˿���3306
    if (!mysql_real_connect(&mysql, "localhost", "root", "", "snake", 3306, NULL, 0))
    {
        printf("\n���ݿ����ӷ�������!");
    }
    else
    {
        printf("\n���ݿ����ӳɹ�!\n"); 			
		//����һ�����ݵ����ݿ�
	switch(ms)
	{
	case 200:mul=1;break;
	case 100:mul=2;break;
	case 50 :mul=4;break;
	default:break;
	}

			///////////ȡʱ��
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			name=input_name();

		sprintf(sql_insert, "INSERT INTO score(name,score,time) values('%s','%d','%s')",name,mul*score,asctime (timeinfo));
		res = mysql_query(&mysql, sql_insert);
        if(res)
        {		
			 printf("��������ʧ��!\n");
        }else 
		{
			printf("�ɼ�����ɹ�\n");
		}
    }  
    mysql_close(&mysql); // �ͷ����ݿ����� 
	printf("��������ԣ�");
	fgetchar();
	replay();

}

void db_out(void)
{

	int j;
	int sel;
	MYSQL mysql;
    MYSQL_RES *resultset;
    MYSQL_ROW row;  
    mysql_init(&mysql);// ��ʼ��mysql�ṹ  

    if (!mysql_real_connect(&mysql, "localhost", "root", "", "snake", 3306, NULL, 0))
    {
        printf("\n���ݿ����ӷ�������!");
    }
    else
    {
		GotoMap(MAP_WIDTH / 4, MAP_HEIGHT / 4);
        printf("\n�� ���а�(ǰʮ��)\n"); 		
		sel=mysql_query(&mysql,"select * from score order by `score` desc limit 0,10");
        if(sel)
			{
				printf("���ݿ��ѯ��������");
			}
		else
		{                    
           resultset = mysql_store_result(&mysql);// ��ý����         
           if (mysql_num_rows(resultset) != NULL)
           {
				int numRows = mysql_num_rows(resultset); // ��ý�����еļ�¼��
				int numFields = mysql_num_fields(resultset);// ��ñ����ֶ���
				printf("��   ����       ����                ����                ʱ��");
				//printf("�� %d �м�¼��ÿ�� %d ���ֶΡ�", numRows, numFields);
				j = 1;
				//mysql_fetch_row()�����ǲ�ѯ�ɹ��󣬰Ѳ�ѯ�����һ��ȡ��һ�������У��Ա�ʹ��.
				//ÿִ��һ��mysql_fetch_row()�����Զ�ȡ������е���һ�м�¼��
                while (row = mysql_fetch_row(resultset))
                {
                    int i = 0;
                    printf("\n\n");
					printf("�� �� %d ����", j);
					for (i = 1; i < numFields; i++)
						{
							GotoMap(MAP_WIDTH/4-2+(i-1)*9,MAP_HEIGHT/4+3+j);
							fprintf(stdout, "%s", row[i]); // ��ӡ�ֶ�ֵ
							GotoMap(MAP_WIDTH/4-2+(i-1)*9,MAP_HEIGHT/4+2+j);
						}
                    j++;
                 }
           }
           else
           {
               printf("\n�޲�ѯ���!");
           } 		
           mysql_free_result(resultset);  // �ͷŽ����
        }
    }  
    mysql_close(&mysql); // �ͷ����ݿ����� 
	printf("\n\n\n\n�� �����������");
	jkGetKey();
	info();
}

char* input_name(void)
{
	static char name[256];
	clearText();
	printf("����������������Ӣ�ģ����س�������\n");
	gets(name);
	return name;
}