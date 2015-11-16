#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
//引用请注明出处：http://hi.baidu.com/neptune_moon/item/56ac4b0fe783acc891571823
#define MAX__X 23
#define MAX__Y 50
#define MAX_FOOD 50
#define ESC 27
#define SPACE 32
typedef struct Node{
 int x;
 int y;
 struct Node *next;
}Node,*LinkList;
typedef struct{
 LinkList front;
 LinkList rear;
 int length;
}Queue;
Queue snake;
void gotoxy(int x,int y)//坐标定位
{
 COORD loc;
 loc.X  =  y;
 loc.Y  =  x;
 SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), loc);
}
void help_info()//显示帮助信息
{
 gotoxy(8,55);
 puts("按ESC键退出,空格键暂停:");
 gotoxy(10,55);
 puts("上,下,左,右:w,s,a,d");
 gotoxy(12,55);
 puts("蛇越长,速度也会越快:");
 gotoxy(14,55);
 puts("如无法控制蛇,请关闭");
 gotoxy(15,56);
 puts("大写键盘锁定状态:");
}
void game_info()//显示游戏信息
{
 gotoxy(3,60);
 printf("当前蛇长度: %d",snake.length);
 gotoxy(5,60);
 printf("               ");
 gotoxy(5,60);
 printf("离胜利还差: %d",MAX_FOOD-snake.length);
}
void enqueue(int x,int y)//入队
{
 LinkList p;
 p = (LinkList)malloc(sizeof(Node));
 p->x = x;
 p->y = y;
 snake.rear->next = p;
 snake.rear = p;
 p->next = NULL;
 snake.length++;
}
void dequeue()//出队
{
 LinkList p = snake.front;
 //保存食物位置
 p->next->x = p->x;
 p->next->y = p->y;
 snake.front = snake.front->next;
 snake.length--;
 free(p);
}
void destroy_queue()//销毁队列
{
 LinkList p;
 while(p = snake.front){
  snake.front = snake.front->next;
  free(p);
 }
}
void snake_init()//初始化蛇,rear指向蛇头,front指向食物,front->next指向蛇尾
{
 int i;
 snake.rear = snake.front = (LinkList)malloc(sizeof(Node));
 snake.rear->next = NULL;
 snake.length = 0;
 for(i = 0;i < 3;i++){
  enqueue(1,i + 1);
 }
}
void show_food()//随机化食物位置
{
 LinkList p;
 srand(time(NULL));//初始化随机种子
 while(1){
  snake.front->x = rand() % (MAX__X-1) + 1;
  snake.front->y = rand() % (MAX__Y-1) + 1;
  p = snake.front->next;
  //防止食物出现在蛇内部
  while(p){
   if(snake.front->x == p->x && snake.front->y == p->y){ break; }
   else{ p = p->next; }
  }
  if( !p){ break; }
 }
 gotoxy(snake.front->x,snake.front->y);
 putchar('@');//食物
}
int judge()//判断失败与否
{
 LinkList p;
 if(snake.length >= MAX_FOOD){
  system("cls");
  gotoxy(4,20);
  puts("恭喜你,赢了!!!");
  exit(0);
 }
 //碰到自身
 p = snake.front->next;
 while(snake.rear !=  p){
  if(snake.rear->x == p->x && snake.rear->y == p->y){ break; }
  else{ p = p->next; }
 }
 if(snake.rear == p){
  //碰到边界
  if(snake.rear->x >= 1 && snake.rear->y >= 1 && snake.rear->x < MAX__X && snake.rear->y < MAX__Y){
   return 1;
  }
 }
 system("cls");
 gotoxy(4,20);
 puts("GAME OVER!!!");
 destroy_queue();
 getch();
 exit(0);
}
void drow_wall()//画墙
{
 int i;
 for(i = 0; i <= MAX__Y; i++){
  gotoxy(0,i);
  putchar('W');
  gotoxy(MAX__X,i);
  putchar('M');
  if(i <= MAX__X){
   gotoxy(i,0);
   putchar('I');
   gotoxy(i,MAX__Y);
   putchar('I');
  }
 }
}
void drow_snake(int i)//0表示画蛇头,1表示画蛇身
{
 LinkList p;
 p = snake.rear;
 gotoxy(p->x,p->y);
 if(i){ putchar('*'); }//蛇身
 else{ putchar('%'); }//蛇头
}
void clear_snake_tail()
{
 LinkList p = snake.front->next;
 gotoxy(p->x,p->y);
 putchar(' ');//清除旧蛇尾
 p = p->next;
 gotoxy(p->x,p->y);
 putchar('~');//添加新蛇尾
}
void snake_auto_move(char temp)//实现蛇的自动移动
{
 int x,y,speed;
 do{
  clear_snake_tail();
  drow_snake(1);//画蛇身
  x = snake.rear->x;
  y = snake.rear->y;
  switch(temp){
  case 'w': x--;break;
  case 's': x++;break;
  case 'a': y--;break;
  case 'd': y++;
  }
  enqueue(x,y);//新蛇头入队
  if(snake.front->x == x && snake.front->y == y){//蛇捕捉到食物后,食物重新随机化
   game_info();
   show_food();
  }
  else{ dequeue(); }//旧蛇尾出队
  drow_snake(0);//画蛇头
  speed = -2.3 * snake.length + 157;
  Sleep(speed);//程序暂停speed个单位毫秒
 }while(!_kbhit() && judge());//_kbhit为检测键盘输入
}
void snake_move()//实现玩家控制蛇的移动
{
 char c,pause;
 static char temp = 0;//静态局部变量
 pause = 0;
 if( !temp){//一开始的时候让蛇先自动移动
  temp = 'd';
  snake_auto_move(temp);
 }
 while(1){
  c = getch();
  if(c == ESC){//退出
   system("cls");
   puts("游戏退出成功!");
   system("pause");
   destroy_queue();
   exit(0);
  }
  if(pause == SPACE){
   if(c == SPACE){//再按下空格键继续游戏
    pause = 0;
    gotoxy(6,20);
    printf("           ");//消除~~Pause~~:
    snake_auto_move(temp);
   }
   continue;
  }
  if(c == SPACE){//按空格键暂停游戏
   pause = SPACE;
   gotoxy(6,20);
   printf("~~Pause~~:");
   continue;
  }
  if(c == 'w' || c == 'a' || c == 's' || c == 'd'){
   if((temp == 'w' && c == 's') || (temp == 's' && c == 'w') || (temp == 'd' && c == 'a') || (temp == 'a' && c == 'd')){//不允许连续的两次内按相反的键
    snake_auto_move(temp);
    continue;
   }
   break;
  }
  snake_auto_move(temp);
 }
 temp = c; 
 snake_auto_move(temp);
}
int main()
{
 help_info();
 snake_init();
 show_food();
 drow_wall();
 game_info();
 while(judge()){
  snake_move();
 }
 return 0;
}