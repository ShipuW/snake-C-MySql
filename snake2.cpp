#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcc32.h"
#include "jkey32.h"
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"libpcc32.a")
#pragma comment(lib,"libjkey32.a")
// �����ͼ�ĳߴ缰����(��ʹ��˫�ַ�����)
#define MAP_WIDTH    24
#define MAP_HEIGHT   16
#define MAP_BASE_X   8
#define MAP_BASE_Y   3
// �����ߵ���ز���
#define SNAKE_MIN_LEN   2      //�ߵ���С����
#define BARS         4       //ÿһ���ϰ���ķ����������

// �����ͼ���״̬,�ֱ�Ϊ[�ո�|��ͷ|����|ʳ��|����|�߿�|�ϰ�]
#define BS_SPACE     0
#define BS_SHEAD     1
#define BS_SBODY     2
#define BS_STAIL     3
#define BS_FOOD      4
#define BS_REWARD    5
#define BS_BLOCK     6
#define BS_BAR       7
// ������������ɫ,��ɫ�������[pcc32.h],˳��ͬ��.
PCCOLOR myColors[] = {ORANGE, RED_ORANGE, RED, LIGHT_GREEN, YELLOW, MEDIUM_BLUE, LIGHT_BLUE ,WHITE, TEAL,DARKER_GREY,LIGHT_PURPLE};
// ������������״,˳��ͬ��.
char mySharps[][3] = {"��", "��", "��", "��", "��", "��", "��", "��"};
// �����ߵ��˶�����[��|��|��|��]
#define DIR_UP    1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4
typedef struct _SnakeBody //�����ʾ��������һ�ڣ����꣨x��y���Ľṹ��
{
   uint8 x, y;
}POINT2D, *PPOINT2D;
//��ʼ������
POINT2D mySnake[2][MAP_WIDTH*MAP_HEIGHT] = {{{0}}};         
POINT2D myFood = {0};
POINT2D myReward={0};
POINT2D myBar[BARS] = {{0}};
int snakeLength[2] = {0,0};
int snakeDir[2] = {DIR_RIGHT,DIR_RIGHT};
int isFood = 0;
int isReward=0;
int isOver[2] = {0,0};
int gamespeed=50;
static int point[2]={0};
int p=0;                       //�жϱ�־����ʳ��������ı���
int q=1;                       //�ж��ϰ���������ı���
int z;                         
char ch2;
char ch;
//��������
void drawMap(void);
void initSnake(int len,int z);
void drawSnake(int z);
void moveSnake(int z);
void drawBlock(int x, int y, int bs);
int isInSnake(int x, int y,int z);
void drawFood(void);
void drawReward(void);
void drawBar(void);
void clearBar(void);
//��Ϸ�˵�
menu()
{
   char ch;
   setTextColor(myColors[RED]);
   gotoTextPos(MAP_WIDTH/2, MAP_HEIGHT/2);
   PlaySound("d:\\Ƭͷ.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   printf("\t\tPlease choose the gamespeed:\n");
   printf("\t\t\t1-Fast   2-Normal    3-Slow\n");
   printf("\n\t\t\tPlease Press The numbers..\n");
do                                           
   {ch=getch();}
while(ch!='1'&&ch!='2'&&ch!='3');                //ȷ�������Ϊ�涨���֮һ
   printf("\t\t\tYou have choosed the speed %c!",ch);
   printf("\n\n\t\t\tPress Anykey to continue!");
   if(jkGetKey())                     
   {
   system("cls");                     //�����Ļ
      return(ch); //����һ��ѡ����ٶ�
   }      
}
menuDouble()           //��˫��ģʽѡ��˵�
{
   char ch2;
   setTextColor(myColors[RED]);
   gotoTextPos(MAP_WIDTH/2, MAP_HEIGHT/2);
   PlaySound("d:\\Ƭͷ.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   printf("\t\tPlease choose the game mode:\n");
   printf("\t\t\t1-1 player\n");
   printf("\t\t\t2-2 players\n");
   printf("\n\t\t\tPlease Press The numbers..\n");
do
   {ch2=getch();}
while(ch2!='1'&&ch2!='2');
   printf("\t\t\tYou have choosed the mode %c!",ch2);
   printf("\n\n\t\t\tPress Anykey to start!");
   if(jkGetKey())
   {
   system("cls");
   return(ch2);
     
   }      
}
DELAY(char ch)//������Ϸ�ٶ�
{
if(ch=='3')
{
delayMS(gamespeed);            
delayMS(gamespeed);
}
else if(ch=='2')
{
delayMS(gamespeed);
}
}

//������
int main()
{ clock_t start,finish;
   int isPause = 1;
   int chance=0;
   float RewardTime=0;
   int z;
   
   setCursorVisible(0);
   setConsoleTitle("̰����--D_Y�� 2010.1.11");        //����
   ch=menu();
   ch2=menuDouble();               
   if(ch2=='1')
    z=0;
   if(ch2=='2')
    z=1;
   initSnake(SNAKE_MIN_LEN,z);
   drawMap();
   drawSnake(z);
   //PlaySound("d:\\���������ɻ���������ЧWAV.wav",NULL, SND_ASYNC|SND_NODEFAULT|SND_LOOP );   
   switch(ch)                 //��ͬ���Ѷȶ�Ӧ��ͬ�Ľ���ʳ����ʧʱ��
   {
   case '1':
       RewardTime=2500;
   case '2':
    RewardTime=3000;
   case '3':
    RewardTime=3500;
   }
   while (!isOver[0]&&!isOver[1])             //������������һ����������Ϸ����
   {   
    srand((uint32)time(NULL));
      if (!isPause)                           
      {
         moveSnake(z);
         if (!isFood)
            drawFood();
    while(((point[1]+point[0])%4==0)&&((point[1]+point[0])>3)&&p==1)        
                                                          //�涨�ڵ÷ִ�������ʱÿ�Ե��ĸ�����ʳ�����һ������ʳ��
    { 
     if (!isReward)
    drawReward();
       start=clock();                                        //����ʳ���¼�˿�ʱ��
      break;  
    }

    finish=start+RewardTime ;                                //����ʱ��Ϊ��ʼʱ��ӹ涨�Ľ���ʳ�����ʱ��
            if((isReward==1)&&(finish<=clock()))
    {
             drawBlock(myReward.x, myReward.y, BS_SPACE); //�����涨ʱ���򽫽���ʳ���ñ������򸲸�
    myReward.x=-1;
    myReward.y=-1;
    isReward=0;                                           
    }
    if(((point[1]+point[0])%3==0)&&((point[1]+point[0])>2)&&q==0)                         
    {
    
      clearBar();
    
                    drawBar();
    }
           
    setTextColor(myColors[4]);                                    //�涨�������������Ϣ
    gotoTextPos(MAP_BASE_X*2 -14 , MAP_BASE_Y + 2);
    printf("Player 1");
   gotoTextPos(MAP_BASE_X*2 -14 , MAP_BASE_Y + 4);
   
            printf("Score: %d.", point[0]);
    if(z==1)
    {
   gotoTextPos(MAP_BASE_X*2 + MAP_WIDTH*2+4 , MAP_BASE_Y + 2);
            printf("Player 2");
   gotoTextPos(MAP_BASE_X*2 + MAP_WIDTH*2+4 , MAP_BASE_Y + 4);
            printf("Score: %d.", point[1]);
    }                                                          
   
    
      }
      DELAY(ch); //������Ϸ�ٶ�
   delayMS(80- snakeLength[0]*2); //��Ϸ�ٶ�������������������
      if (jkHasKey()) //���̿���
      {
         switch (jkGetKey())
         {
      
            case JK_UP:
               if (snakeDir[0] != DIR_DOWN)
                  snakeDir[0] = DIR_UP;
      break;
    case 'w': 
               if (snakeDir[1] != DIR_DOWN)
                  snakeDir[1] = DIR_UP;
               
               break;
  
            case JK_DOWN:
               if (snakeDir[0] != DIR_UP)
                  snakeDir[0] = DIR_DOWN;
      break;
      case 's': 
      if (snakeDir[1] != DIR_UP)
                  snakeDir[1] = DIR_DOWN;
               break;
  
            case JK_LEFT:
               if (snakeDir[0] != DIR_RIGHT)
                  snakeDir[0] = DIR_LEFT;
      break;
      case 'a': 
       if (snakeDir[1] != DIR_RIGHT)
                  snakeDir[1] = DIR_LEFT;
               break;
  
            case JK_RIGHT:
               if (snakeDir[0] != DIR_LEFT)
                  snakeDir[0] = DIR_RIGHT;
      case 'd': 
        if (snakeDir[1] != DIR_LEFT)
                  snakeDir[1] = DIR_RIGHT;
               break;
            case JK_ENTER:
            case JK_SPACE:
               isPause = !isPause;
               break;
      case JK_ESC:
       isOver[0]=1;
       break;
            default:
               break;
         }

      }
   }
   PlaySound("d:\\����������ֵ� ������.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   gotoTextPos(MAP_BASE_X + MAP_WIDTH - 7, MAP_BASE_Y + MAP_HEIGHT + 1);
   if(isOver[0]==1&&z==0)
printf("Game Over!!");
   if(isOver[0]==1&&z==1)
printf("Player 2 win!!");   
   if(isOver[1]==1)
   printf("Player 1 win!!");//��Ϸ�������ڹ涨����ʾ����
   getch();
   return 0;
}

void drawMap(void)
{
   int i, j;
   setTextColor(myColors[BS_SPACE]);            //�����ƶ���Χ
   for (i = 0; i < MAP_HEIGHT; i++)
   {
      gotoTextPos(MAP_BASE_X*2, MAP_BASE_Y + i);
      for (j = 0; j < MAP_WIDTH; j++)
      {
         printf("%2s", mySharps[BS_SPACE]);
      }
   }
   setTextColor(myColors[BS_BLOCK]);            //�����ƶ���Χ�ı߿�
for (i=0; i<MAP_HEIGHT; i++)
{
      gotoTextPos(MAP_BASE_X*2-2, MAP_BASE_Y -1);
   for(j=-1; j<MAP_WIDTH+1; j++)
   {
        printf("%2s", mySharps[BS_BLOCK ]);
   }
   gotoTextPos(MAP_BASE_X*2-2, MAP_BASE_Y +i);
    printf("%2s", mySharps[BS_BLOCK ]);
  
   gotoTextPos(MAP_BASE_X*2+MAP_WIDTH*2, MAP_BASE_Y +i);
       printf("%2s", mySharps[BS_BLOCK ]);
   
   gotoTextPos(MAP_BASE_X*2-2, MAP_BASE_Y+MAP_HEIGHT);
   for(j=-1; j<MAP_WIDTH+1; j++)
   {
        printf("%2s", mySharps[BS_BLOCK ]);
   }
}
}
void initSnake(int len,int z)           //��ʼ���ߵĳ��Ⱥ�����
{
   int i=0,j=0;
   for(j=0;j<z+1;j++)
   {
    for (i = 0; i < len; i++)
    {
      mySnake[j][i].x = 10 - i;
      mySnake[j][i].y = 3+9*j;
    }
   
   snakeLength[j] = len;
   
   isOver[j] = 0;
   }

}
void drawSnake(int z)           //��������ͷβ
{
   int i=0,j=0;
   if (snakeLength[0] < SNAKE_MIN_LEN)
      return;
   if(z==1)
   if (snakeLength[1] < SNAKE_MIN_LEN)
      return;
for(j=0;j<z+1;j++)
{
   setTextColor(myColors[BS_SHEAD+7*j]);
   gotoTextPos((MAP_BASE_X + mySnake[j][0].x)*2, MAP_BASE_Y + mySnake[j][0].y);
   printf(mySharps[BS_SHEAD]);
   setTextColor(myColors[BS_SBODY+7*j]);
   for (i = 1; i < snakeLength[j] - 1; i++)
   {
      gotoTextPos((MAP_BASE_X + mySnake[j][i].x)*2, MAP_BASE_Y + mySnake[j][i].y);
      printf(mySharps[BS_SBODY]);
   }
   setTextColor(myColors[BS_STAIL+7*j]);
   gotoTextPos((MAP_BASE_X + mySnake[j][snakeLength[j]-1].x)*2, MAP_BASE_Y + mySnake[j][snakeLength[j]-1].y);
   printf(mySharps[BS_STAIL]);
}
   return;
}
void moveSnake(int z)             //�ߵ��ƶ�����
{
   int i,j;
   int hx[2] = {mySnake[0][0].x,mySnake[1][0].x};
   int hy[2] = {mySnake[0][0].y,mySnake[1][0].y};
   if (snakeLength[0] < SNAKE_MIN_LEN)
      return;
   if(z==1)
   if (snakeLength[1] < SNAKE_MIN_LEN)
      return;
   for(j=0;j<z+1;j++)
   {
   switch (snakeDir[j])           //���̿��ƣ��ߵ��ƶ�����
   {
      case DIR_UP:
         hy[j]--;
         break;
      case DIR_DOWN:
         hy[j]++;
         break;
      case DIR_LEFT:
         hx[j]--;
         break;
      case DIR_RIGHT:
         hx[j]++;
         break;
      default:
         break;
   }
   if (hx[j] < 0 || hx[j] >= MAP_WIDTH || hy[j] < 0 || hy[j] >= MAP_HEIGHT || isInSnake(hx[j], hy[j],z))
   {
      isOver[j] = 1;
      return;
   }
   //�жϣ����߳����߽��ҧ���Լ���ײ������������Ϸ����
                                     
   for (i=0;i<BARS;i++)
   {
     if((hx[j]==myBar[i].x)&&(hy[j]==myBar[i].y))
{
        isOver[j] = 1;
      return;
}
   }
   
   //�жϣ����������ϰ����Ϸ����
   if (hx[j] == myFood.x && hy[j] == myFood.y) //�ж��Ƿ�Ե�ʳ��Ե��򳤶����ӣ���������
   {
      snakeLength[j]++;
   PlaySound("d:\\ʳ��.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   point[j]++;
      isFood = 0;
   }
    if (hx[j] == myReward.x && hy[j] == myReward.y)      //�ж��Ƿ�Ե�����ʳ��Ե��򳤶ȼ���
   {
      snakeLength[j]--;
   PlaySound("d:\\����ʳ��.wav",NULL, SND_ASYNC|SND_NODEFAULT);
      isReward = 0;
   drawBlock(mySnake[j][snakeLength[j]].x, mySnake[j][snakeLength[j]].y, BS_SPACE);
   drawBlock(mySnake[j][snakeLength[j]-1].x, mySnake[j][snakeLength[j]-1].y, BS_SPACE);
   }
else
      drawBlock(mySnake[j][snakeLength[j]-1].x, mySnake[j][snakeLength[j]-1].y, BS_SPACE);
  
   for (i = snakeLength[j] - 1; i >= 0; i--)                //ʵ���ߵ��ƶ�������ǰһ�ڵ����긳����һ��
   {
      mySnake[j][i+1].x = mySnake[j][i].x;
      mySnake[j][i+1].y = mySnake[j][i].y;
   }
   mySnake[j][0].x = hx[j];
   mySnake[j][0].y = hy[j];
   }
   
   drawSnake(z);
   return;
}
void drawBlock(int x, int y, int bs)             //����㺯��
{
   if (x < 0 || x >= MAP_WIDTH)
      return;
   if (y < 0 || y >= MAP_HEIGHT)
      return;
   gotoTextPos((MAP_BASE_X + x)*2, MAP_BASE_Y + y);
   setTextColor(myColors[bs]);
   printf("%2s", mySharps[bs]);
   return;
}
int isInSnake(int x, int y,int z)                     //�ж�ĳ����λ���Ƿ�����������
{
   int i,j;
   for(j=0;j<z+1;j++)
   {
   for (i = 0; i < snakeLength[j]; i++)
      if (x == mySnake[j][i].x && y == mySnake[j][i].y)
         return 1;
   return 0;
   }
}
void drawFood(void)                               //��ʳ�ﺯ��
{
int i=0,k=0;
   srand((uint32)time(NULL));
   do
   { 
      myFood.x = rand() % MAP_WIDTH;                      //ʳ����������ڹ涨��Χ��
      myFood.y = rand() % MAP_HEIGHT;
   for(i=0;i<BARS;i++)
   {
   if(((myReward.x ==myFood.x)&&(myReward.y ==myFood.y))||
    ((myBar[i].x ==myFood.x)&&(myBar[i].y ==myFood.y)))
       k=1;
   break;
   }
   if(k==1)
   return;                                          //��������ʳ�����ϰ����ڻ��뽱��ʳ���غ��������������

   } while (isInSnake(myFood.x, myFood.y,z));                  //���߳Ե�ʳ���
   drawBlock(myFood.x, myFood.y, BS_FOOD);
   isFood = 1;
   p=1;
   q=0;
}
void drawReward(void)                      //������ʳ��
{
int i=0, k=0;
srand((uint32)time(NULL));                 
   do
   {
      myReward.x = rand() % MAP_WIDTH;                       //�����������ʳ��
      myReward.y = rand() % MAP_HEIGHT;
   for(i=0;i<BARS;i++)
   {
   if(((myReward.x ==myFood.x)&&(myReward.y ==myFood.y))||
    ((myBar[i].x ==myReward.x)&&(myBar[i].y ==myReward.y)))
       k=1;
   break;
   }
   if(k==1)
    return;                                                   //��������ʳ�����ϰ����ڻ��볣��ʳ���غ��������������
   } while (isInSnake(myReward.x, myReward.y,z));                      //���߳Ե�ʳ���
   drawBlock(myReward.x, myReward.y, BS_REWARD);
   isReward = 1;
   p=0;
}
void drawBar(void)                                                   //���ϰ��ﺯ��
{int i=0, j=0, a=0 ,k=0;

do
{
      myBar[0].x = rand() % MAP_WIDTH;
      myBar[0].y = rand() % MAP_HEIGHT;
   if(((myBar[0].x ==myFood.x)&&(myBar[0].y ==myFood.y))
    ||((myBar[0].x ==myReward.x)&&(myBar[0].y ==myReward.y))
    ||(isInSnake(myBar[0].x, myBar[0].y,z)))
    return;
    drawBlock(myBar[0].x, myBar[0].y, BS_BAR);
   for(i=1;i<BARS;i++)                                              //��BARS�λ����ϰ���ķ���
   {
    a=rand()%4;                                            
    switch(a)                                                    //����������������������һ��
    {
       
    case 0: myBar[i].x=myBar[i-1].x+1;                           //��ԭ������ϲ�����·���
                  myBar[i].y=myBar[i-1].y;
      break;
    
    case 1:    myBar[i].x=myBar[i-1].x;                             //��ԭ������ϲ�����·���
                 myBar[i].y=myBar[i-1].y+1;
      break;
    
    case 2:    myBar[i].x=myBar[i-1].x-1;                           //��ԭ������ϲ�����·���
                 myBar[i].y=myBar[i-1].y;
      break;
    
    case 3:    myBar[i].x=myBar[i-1].x;                            //��ԭ������ϲ�����·���
                 myBar[i].y=myBar[i-1].y-1;
         break;
    default:
                     break;
    }
    
    for(j=0;j<i;j++)
    {
           if(((myBar[j].x ==myBar[i].x )&&(myBar[j].y ==myBar[i].y )) 
      ||((myBar[i].x ==myFood.x)&&(myBar[i].y ==myFood.y))
      ||((myBar[i].x ==myReward.x)&&(myBar[i].y ==myReward.y))
      ||(isInSnake(myBar[i].x, myBar[i].y,z))
      ||(myBar[i].x<0)||myBar[i].x>=MAP_WIDTH || myBar[i].y<0||myBar[i].y>=MAP_HEIGHT)
     {i--;
     
     break;}
    }
    
   
    drawBlock(myBar[i].x, myBar[i].y, BS_BAR);
   }
   
   break;
} while (1);
   
   q=1;
}
void clearBar(void)
{ 
int j=0;

for(j=0;j<BARS;j++)                    //��ȥ�ϰ��ﲢ����
{
   drawBlock(myBar[j].x, myBar[j].y, BS_SPACE);
   myBar[j].x=-1; myBar[j].y=-1;
}

}