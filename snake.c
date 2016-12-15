#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#define vertical 20
#define horizontal 50
#define down 115
#define up 119
#define left 97
#define right 100

typedef struct Snake
{
    char symbol;
    int size;
    char direction;
    char prev_direction;
    int tail_X;
    int tail_Y;
    int head_X;
    int head_Y;

}snake;


typedef struct snake_pos
{
    int Y[vertical*horizontal];
    int X[vertical*horizontal];

}snake_pos;


typedef struct food
{
    int X;
    int Y;
    char symbol;

}food;

// --------------- 추가 된곳
static struct result
{
	char name[30];
	long point;
	int year;
	int month;
	int day;
	int hour;
	int min;
	int rank;
}temp_result;
// --------------- 추가 된곳

void main_screen();
void playing_screen();
void gameover_screen();

void snake_init(snake *snake1);
void pos_init(snake_pos *pos1);
void food_init(food *food1);
void gotoxy(int,int);
void snake_place(snake *snake1, snake_pos *pos1);
void snake_move(snake *snake1, snake_pos *pos1, food *food1, int*);
void move_tail(snake *snake1, snake_pos *pos1);
void move_head(snake *snake1, snake_pos *pos1);
void food_print(food *food1);
int game_over(snake *snake1, snake_pos *pos1);
void set_borders();
void print_score(int*);
int print_result(void);/* 추가 : 메뉴에서 기록출력시 호출되어 기록을 출력하는 함수*/
int search_result(void); /*추가 : 메뉴에서 기록검색시 호출되어 기러고을 검색하는 함수*/

int snake_speed = 100000;
int score = 0;
int best_point = 0; /* 추가 : 게임 최고 점수를 알려주는 변수*/

int main()
{

	main_screen();

	playing_screen();

	gameover_screen();


	return 0;

}



void main_screen()
{
      system("clear");
      system("stty -echo");
      
      char button;

      set_borders();
      
      gotoxy(12,3);
      printf("                       ");
      gotoxy(12,4);
      printf("                       ");
      gotoxy(12,5);
      printf("                       ");
      gotoxy(12,6);
      printf("  S N A K E   G A M E  ");
      gotoxy(12,7);
      printf("                       ");
      gotoxy(12,8);
      printf("                       ");
      gotoxy(12,9);
      printf("                       ");

      gotoxy(10,14);
      printf("Button : a(←) w(↑) d(→) s(↓)");
      gotoxy(10,15);
      printf("Pause  : p");
      gotoxy(10,16);
      printf("Quit   : ESC");
	  // --------------- 추가 된곳
	  gotoxy(10,17);
	  printf("점수판   : 1");
	  gotoxy(10,18);
      printf("점수검색   : 2");


      gotoxy(10,19);
      printf("Press Enter button to start game!");
	  scanf("%d", &button);
	  if(button = '1'){
			print_result();
		}
		else if(button = '2'){
			search_result();
		}

      while(!kbhit()){
      	button=getchar();
	    if(button = 's'){
      		system("clear");
      		system("stty echo");
		break;
	    }
		
		// --------------- 추가 된곳
      }
     
}



void playing_screen()
{
      /* struct init */

      snake snake1;
      snake_pos pos1;
      food food1;
      snake_init(&snake1);
      pos_init(&pos1);
      food_init(&food1);


      /* set initial field */

      system("clear");
      system("stty -echo");
      //curs_set(0);                    // doesn't work for some reason
      snake_place(&snake1,&pos1);
      set_borders();
      food_print(&food1);


      /* use system call to make terminal send all keystrokes directly to stdin */

      system ("/bin/stty raw");


      /* while snake not got collided into itself */

      while(!(game_over(&snake1,&pos1)))
      {

          /* while key not pressed */

          while (!kbhit())
          {
                 usleep(snake_speed);
                 snake_move(&snake1,&pos1,&food1,&score);
		 if (game_over(&snake1,&pos1))
                 {
                     sleep(1);
		     break;
                 }

          }
          /* store previous direction and fetch a new one */

          snake1.prev_direction=snake1.direction;
          snake1.direction=getchar();
	  
	  /* Stage & player's score */
	  //gotoxy(6, vertical+2);
	  //printf("Stage : %d", stage);
	  gotoxy(16, vertical+2);
	  printf("Your socre : %d", score);
      }

}


void gameover_screen()
{

// --------------- 추가 된곳
	time_t ptime;
	struct tm *t;
	FILE *fp = NULL;
// --------------- 추가 된곳

      /* use system call to set terminal behaviour to more normal behaviour */
      system ("/bin/stty cooked");
      system("stty echo");
      system("clear");

      gotoxy(30,5);
      printf("                       ");
      gotoxy(30,6);
      printf("                       ");
      gotoxy(30,7);
      printf("                       ");
      gotoxy(30,8);
      printf("   G A M E   O V E R   ");
      gotoxy(30,9);

      // --------------- 추가 된곳
				printf(" 최종 득점 : %d ", score);
				gotoxy(30,10);
				printf(" 이름을 입력 하세요 : ");
				scanf("%s%*c", temp_result.name);
				temp_result.point = score;

				if(temp_result.point >= best_point)
					best_point = temp_result.point;


				ptime = time(NULL); 
				t = localtime(&ptime);

				temp_result.year = t->tm_year + 1900;
				temp_result.month = t->tm_mon + 1;
				temp_result.day = t->tm_mday;
				temp_result.hour = t->tm_hour;
				temp_result.min = t->tm_min;

				fp = fopen("result", "ab");
				fseek(fp, 1, SEEK_END);
				fwrite(&temp_result, sizeof(struct result), 1, fp);
				fclose(fp);

				score = 0;


      gotoxy(30,15);
      printf("Your score : %d , Best score : %d \n\n", score,best_point);
      score = 0;
	  // --------------- 추가 된곳

}

void snake_init(snake *snake1)
{
    snake1->symbol='o';
    snake1->size=10;
    snake1->direction=right;
    snake1->prev_direction=down;
    snake1->tail_X=5;
    snake1->tail_Y=5;
    snake1->head_X=snake1->tail_X+snake1->size-1;
    snake1->head_Y=5;
}


void snake_place(snake *snake1, snake_pos *pos1)
{
    int i;
    for (i=0; i<snake1->size; ++i)
    {
        gotoxy(snake1->tail_X,snake1->tail_Y);
        printf("%c",snake1->symbol);
        pos1->X[i]=snake1->tail_X;
        pos1->Y[i]=snake1->tail_Y;
        snake1->tail_X+=1;
    }

}

void set_borders()
{
    int i;
    for (i=0; i<vertical; ++i)
    {
	gotoxy(0,i);
    	printf(" ");
        gotoxy(horizontal, i);
	printf(" ");
    }
    for (i=0; i<horizontal; ++i)
    {
	gotoxy(i,0);
    	printf(" ");
	gotoxy(i,vertical);
    	printf(" ");
    }
    gotoxy(horizontal, vertical);
    printf(" ");
}




void snake_move(snake *snake1, snake_pos *pos1, food *food1, int *score)
{
    move_head(snake1,pos1);

    if (!((snake1->head_X==food1->X) && (snake1->head_Y==food1->Y)))
    {
        move_tail(snake1,pos1);
    }
    else
    {
        snake1->size++;
        *score=*score+10;
        food1->X=rand()%(horizontal-3)+2;
        food1->Y=rand()%(vertical-3)+2;
        food_print(food1);
    }
}



void move_tail(snake *snake1, snake_pos *pos1)
{
    int i;

    // remove last cell of tail
    gotoxy(pos1->X[0],pos1->Y[0]);
    printf(" ");


    // update new tail position
    for (i=0; i<snake1->size; ++i)
    {
        pos1->X[i]=pos1->X[i+1];
        pos1->Y[i]=pos1->Y[i+1];
    }
}



void move_head(snake *snake1, snake_pos *pos1)
{
    switch (snake1->direction)
        {
            case right:
                if (snake1->prev_direction==left)
                {
                    snake1->head_X--;
                    break;
                }
                    snake1->head_X++;
                    break;

            case left:
                if (snake1->prev_direction==right)
                {
                    snake1->head_X++;
                    break;
                }
                    snake1->head_X--;
                    break;


            case up:
                if (snake1->prev_direction==down)
                {
                    snake1->head_Y++;
                    break;
                }
                    snake1->head_Y--;
                    break;


            case down:
                if (snake1->prev_direction==up)
                {
                    snake1->head_Y--;
                    break;
                }
                    snake1->head_Y++;
                    break;


            default:
                 break;
        }


        // update tail position
        pos1->X[snake1->size]=snake1->head_X;
        pos1->Y[snake1->size]=snake1->head_Y;

        gotoxy(pos1->X[snake1->size],pos1->Y[snake1->size]);
        printf("%c",snake1->symbol);
}



void food_init(food *food1)
{
    food1->X=(rand()%(horizontal-3))+2;
    food1->Y=(rand()%(vertical-3))+2;
    food1->symbol='F';
}


void food_print(food *food1)
{
    gotoxy(food1->X,food1->Y);
    printf("%c",food1->symbol);

}


void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}



void pos_init(snake_pos *pos1)
{
    memset(pos1, 0, sizeof(*pos1));
}


int game_over(snake *snake1, snake_pos *pos1)
{
    int i;

    for (i=0; i<snake1->size-1; ++i)
    {
        if ((pos1->X[i]==snake1->head_X) && (pos1->Y[i]==snake1->head_Y))
        {
            return 1;
        }
    }


    if ((snake1->head_X==horizontal) || (snake1->head_X==1) || (snake1->head_Y==vertical) || (snake1->head_Y==1))
        {
            return 1;
        }


    return 0;
}



int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

/*추가 : 메뉴에서 기록검색시 호출되어 기러고을 검색하는 함수*/
int search_result(void)
{
	FILE *fp = NULL;
	char name[30];
	char ch;
	int find = 0;

	fp = fopen("result", "rb");

	if(fp == NULL)
		return 0;

	system("clear");

	printf("\n\n\t\t검색할 이름을 입력하세요.  : ");
	scanf("%s%*c", name);

	printf("\n\t\t\t\tText Tetris");
	printf("\n\t\t\t\t 게임 기록\n\n");
	printf("\n\t\t이름\t\t점수\t   날짜\t\t 시간");

	while(1)
	{
		fread(&temp_result, sizeof(struct result), 1, fp);
		if(!feof(fp))
		{
			if(!strcmp(temp_result.name, name))
			{
				find = 1;
				printf("\n\t========================================================");
				printf("\n\t\t%s\n\t\t\t\t%d\t%d. %d. %d.  |  %d : %d\n", temp_result.name, temp_result.point, temp_result.year, temp_result.month, temp_result.day, temp_result.hour, temp_result.min);
			}
		}
		else
		{
			break;
		}
	}

	if(find == 0)
		printf("\n\n\n\t\t검색된 이름이 없습니다.");


	exit(0);
}

/* 추가 : 메뉴에서 기록출력시 호출되어 기록을 출력하는 함수*/
int print_result(void)
{
	FILE *fp = NULL;
	char ch = 1 ;

	fp = fopen("result", "rb");

	if(fp == NULL)
		return 0;

	system("clear");

	printf("\n\t\t\t\tText Tetris");
	printf("\n\t\t\t\t 게임 기록\n\n");
	printf("\n\t\t이름\t\t점수\t   날짜\t\t 시간");

	while(1)
	{
			fread(&temp_result, sizeof(struct result), 1, fp);
			if(!feof(fp))
			{
				printf("\n\t========================================================");
				printf("\n\t\t%s\n\t\t\t\t%d\t %d. %d. %d.  |  %d : %d\n", temp_result.name, temp_result.point, temp_result.year, temp_result.month, temp_result.day, temp_result.hour, temp_result.min);
			}
			else
			{
				break;
			}
	}

	fclose(fp);

	exit(0);

}
