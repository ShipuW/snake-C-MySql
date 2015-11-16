#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcc32.h"
#include "jkey32.h"
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"libpcc32.a")
#pragma comment(lib,"libjkey32.a")
// 定义地图的尺寸及坐标(均使用双字符长度)
#define MAP_WIDTH    24
#define MAP_HEIGHT   16
#define MAP_BASE_X   8
#define MAP_BASE_Y   3
// 定义蛇的相关参数
#define SNAKE_MIN_LEN   2      //蛇的最小长度
#define BARS         4       //每一个障碍物的方块最大数量

// 定义地图块的状态,分别为[空格|蛇头|蛇身|食物|奖励|边框|障碍]
#define BS_SPACE     0
#define BS_SHEAD     1
#define BS_SBODY     2
#define BS_STAIL     3
#define BS_FOOD      4
#define BS_REWARD    5
#define BS_BLOCK     6
#define BS_BAR       7
// 定义各对象的颜色,颜色定义详见[pcc32.h],顺序同上.
PCCOLOR myColors[] = {ORANGE, RED_ORANGE, RED, LIGHT_GREEN, YELLOW, MEDIUM_BLUE, LIGHT_BLUE ,WHITE, TEAL,DARKER_GREY,LIGHT_PURPLE};
// 定义各对象的形状,顺序同上.
char mySharps[][3] = {"□", "◆", "■", "＋", "★", "★", "¤", "¤"};
// 定义蛇的运动方向[上|下|左|右]
#define DIR_UP    1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4
typedef struct _SnakeBody //定义表示蛇身（其中一节）坐标（x，y）的结构体
{
   uint8 x, y;
}POINT2D, *PPOINT2D;
//初始化变量
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
int p=0;                       //判断标志奖励食物出现与否的变量
int q=1;                       //判断障碍物出现与否的变量
int z;                         
char ch2;
char ch;
//函数申明
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
//游戏菜单
menu()
{
   char ch;
   setTextColor(myColors[RED]);
   gotoTextPos(MAP_WIDTH/2, MAP_HEIGHT/2);
   PlaySound("d:\\片头.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   printf("\t\tPlease choose the gamespeed:\n");
   printf("\t\t\t1-Fast   2-Normal    3-Slow\n");
   printf("\n\t\t\tPlease Press The numbers..\n");
do                                           
   {ch=getch();}
while(ch!='1'&&ch!='2'&&ch!='3');                //确保输入的为规定情况之一
   printf("\t\t\tYou have choosed the speed %c!",ch);
   printf("\n\n\t\t\tPress Anykey to continue!");
   if(jkGetKey())                     
   {
   system("cls");                     //清空屏幕
      return(ch); //返回一个选择的速度
   }      
}
menuDouble()           //单双人模式选择菜单
{
   char ch2;
   setTextColor(myColors[RED]);
   gotoTextPos(MAP_WIDTH/2, MAP_HEIGHT/2);
   PlaySound("d:\\片头.wav",NULL, SND_ASYNC|SND_NODEFAULT );
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
DELAY(char ch)//调节游戏速度
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

//主函数
int main()
{ clock_t start,finish;
   int isPause = 1;
   int chance=0;
   float RewardTime=0;
   int z;
   
   setCursorVisible(0);
   setConsoleTitle("贪吃蛇--D_Y。 2010.1.11");        //标题
   ch=menu();
   ch2=menuDouble();               
   if(ch2=='1')
    z=0;
   if(ch2=='2')
    z=1;
   initSnake(SNAKE_MIN_LEN,z);
   drawMap();
   drawSnake(z);
   //PlaySound("d:\\泡泡堂轻松欢快音乐音效WAV.wav",NULL, SND_ASYNC|SND_NODEFAULT|SND_LOOP );   
   switch(ch)                 //不同的难度对应不同的奖励食物消失时间
   {
   case '1':
       RewardTime=2500;
   case '2':
    RewardTime=3000;
   case '3':
    RewardTime=3500;
   }
   while (!isOver[0]&&!isOver[1])             //当两条蛇任意一条死亡则游戏结束
   {   
    srand((uint32)time(NULL));
      if (!isPause)                           
      {
         moveSnake(z);
         if (!isFood)
            drawFood();
    while(((point[1]+point[0])%4==0)&&((point[1]+point[0])>3)&&p==1)        
                                                          //规定在得分大于三分时每吃掉四个常规食物产生一个奖励食物
    { 
     if (!isReward)
    drawReward();
       start=clock();                                        //画出食物，记录此刻时间
      break;  
    }

    finish=start+RewardTime ;                                //结束时间为开始时间加规定的奖励食物出现时间
            if((isReward==1)&&(finish<=clock()))
    {
             drawBlock(myReward.x, myReward.y, BS_SPACE); //超出规定时间则将奖励食物用背景方框覆盖
    myReward.x=-1;
    myReward.y=-1;
    isReward=0;                                           
    }
    if(((point[1]+point[0])%3==0)&&((point[1]+point[0])>2)&&q==0)                         
    {
    
      clearBar();
    
                    drawBar();
    }
           
    setTextColor(myColors[4]);                                    //规定出输出分数等信息
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
      DELAY(ch); //赋予游戏速度
   delayMS(80- snakeLength[0]*2); //游戏速度随着蛇身增长而增加
      if (jkHasKey()) //键盘控制
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
   PlaySound("d:\\超级马里奥兄弟 死掉音.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   gotoTextPos(MAP_BASE_X + MAP_WIDTH - 7, MAP_BASE_Y + MAP_HEIGHT + 1);
   if(isOver[0]==1&&z==0)
printf("Game Over!!");
   if(isOver[0]==1&&z==1)
printf("Player 2 win!!");   
   if(isOver[1]==1)
   printf("Player 1 win!!");//游戏结束，在规定处显示分数
   getch();
   return 0;
}

void drawMap(void)
{
   int i, j;
   setTextColor(myColors[BS_SPACE]);            //画出移动范围
   for (i = 0; i < MAP_HEIGHT; i++)
   {
      gotoTextPos(MAP_BASE_X*2, MAP_BASE_Y + i);
      for (j = 0; j < MAP_WIDTH; j++)
      {
         printf("%2s", mySharps[BS_SPACE]);
      }
   }
   setTextColor(myColors[BS_BLOCK]);            //画出移动范围的边框
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
void initSnake(int len,int z)           //初始化蛇的长度和坐标
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
void drawSnake(int z)           //画出蛇身及头尾
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
void moveSnake(int z)             //蛇的移动函数
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
   switch (snakeDir[j])           //键盘控制：蛇的移动方向
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
   //判断，若蛇超出边界或咬到自己或撞到其他蛇则游戏结束
                                     
   for (i=0;i<BARS;i++)
   {
     if((hx[j]==myBar[i].x)&&(hy[j]==myBar[i].y))
{
        isOver[j] = 1;
      return;
}
   }
   
   //判断，若蛇碰到障碍物，游戏结束
   if (hx[j] == myFood.x && hy[j] == myFood.y) //判断是否吃到食物，吃到则长度增加，分数增加
   {
      snakeLength[j]++;
   PlaySound("d:\\食物.wav",NULL, SND_ASYNC|SND_NODEFAULT );
   point[j]++;
      isFood = 0;
   }
    if (hx[j] == myReward.x && hy[j] == myReward.y)      //判断是否吃到奖励食物，吃到则长度减少
   {
      snakeLength[j]--;
   PlaySound("d:\\奖励食物.wav",NULL, SND_ASYNC|SND_NODEFAULT);
      isReward = 0;
   drawBlock(mySnake[j][snakeLength[j]].x, mySnake[j][snakeLength[j]].y, BS_SPACE);
   drawBlock(mySnake[j][snakeLength[j]-1].x, mySnake[j][snakeLength[j]-1].y, BS_SPACE);
   }
else
      drawBlock(mySnake[j][snakeLength[j]-1].x, mySnake[j][snakeLength[j]-1].y, BS_SPACE);
  
   for (i = snakeLength[j] - 1; i >= 0; i--)                //实现蛇的移动，即将前一节的坐标赋给后一节
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
void drawBlock(int x, int y, int bs)             //画物点函数
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
int isInSnake(int x, int y,int z)                     //判断某坐标位置是否在蛇身体内
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
void drawFood(void)                               //画食物函数
{
int i=0,k=0;
   srand((uint32)time(NULL));
   do
   { 
      myFood.x = rand() % MAP_WIDTH;                      //食物随机产生在规定范围内
      myFood.y = rand() % MAP_HEIGHT;
   for(i=0;i<BARS;i++)
   {
   if(((myReward.x ==myFood.x)&&(myReward.y ==myFood.y))||
    ((myBar[i].x ==myFood.x)&&(myBar[i].y ==myFood.y)))
       k=1;
   break;
   }
   if(k==1)
   return;                                          //若产生的食物在障碍物内或与奖励食物重合则重新随机产生

   } while (isInSnake(myFood.x, myFood.y,z));                  //在蛇吃掉食物后
   drawBlock(myFood.x, myFood.y, BS_FOOD);
   isFood = 1;
   p=1;
   q=0;
}
void drawReward(void)                      //画奖励食物
{
int i=0, k=0;
srand((uint32)time(NULL));                 
   do
   {
      myReward.x = rand() % MAP_WIDTH;                       //随机产生奖励食物
      myReward.y = rand() % MAP_HEIGHT;
   for(i=0;i<BARS;i++)
   {
   if(((myReward.x ==myFood.x)&&(myReward.y ==myFood.y))||
    ((myBar[i].x ==myReward.x)&&(myBar[i].y ==myReward.y)))
       k=1;
   break;
   }
   if(k==1)
    return;                                                   //若产生的食物在障碍物内或与常规食物重合则重新随机产生
   } while (isInSnake(myReward.x, myReward.y,z));                      //在蛇吃掉食物后
   drawBlock(myReward.x, myReward.y, BS_REWARD);
   isReward = 1;
   p=0;
}
void drawBar(void)                                                   //画障碍物函数
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
   for(i=1;i<BARS;i++)                                              //分BARS次画出障碍物的方格
   {
    a=rand()%4;                                            
    switch(a)                                                    //在以下四种情况中随机产生一种
    {
       
    case 0: myBar[i].x=myBar[i-1].x+1;                           //在原方格的上侧产生新方格
                  myBar[i].y=myBar[i-1].y;
      break;
    
    case 1:    myBar[i].x=myBar[i-1].x;                             //在原方格的上侧产生新方格
                 myBar[i].y=myBar[i-1].y+1;
      break;
    
    case 2:    myBar[i].x=myBar[i-1].x-1;                           //在原方格的上侧产生新方格
                 myBar[i].y=myBar[i-1].y;
      break;
    
    case 3:    myBar[i].x=myBar[i-1].x;                            //在原方格的上侧产生新方格
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

for(j=0;j<BARS;j++)                    //擦去障碍物并清零
{
   drawBlock(myBar[j].x, myBar[j].y, BS_SPACE);
   myBar[j].x=-1; myBar[j].y=-1;
}

}