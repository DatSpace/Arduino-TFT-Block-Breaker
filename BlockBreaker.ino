#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

int const scr_w = TFTscreen.width();
int const scr_h = TFTscreen.height();
int const pad_dist = 8;
int const pad_w = 30;
int const pad_h = 5;
int pad_x = scr_w/2;
int ball_hori_speed = 2;
int ball_vert_speed = 2;
int const ball_radius = 3;
int ball_center_x = scr_w/2;
int ball_center_y = scr_h/2;
char ball_direction = 'd';
int player_score = 0;
char print_score[3];
bool alive = true;
int const max_level = 15;
int lvl_coll = 5;
int const rows = 5;
int block_w = (scr_w/lvl_coll)-1;
int const block_h = 10;
int block_array[max_level][rows][3];

void initial(){
  int block_w_sum = 0;
  int block_h_sum = 0;
  int space = 0;
  
  for (int i = 0;i<lvl_coll;i++){
    if (block_w_sum + block_w <= scr_w){
        block_w_sum += block_w + 1;
    }
    space = (scr_w-block_w_sum)/2;
  }
  
  block_h_sum = 0;
  for (int i = 0;i<rows;i++){ //Epireazete apo to row !!!
    block_w_sum = space;
    for (int j = 0;j<lvl_coll;j++){
      block_array[i][j][1] = block_w_sum;
      block_array[i][j][2] = block_h_sum;
      if (block_w_sum + block_w <= scr_w){
        block_w_sum += block_w + 1;
        block_array[i][j][0] = 1;
      }
    }
    block_h_sum += block_h + 1;
  }
}

void draw_blocks(){
  for (int i = 0;i<rows;i++){
    for (int j = 0;j<lvl_coll;j++){
      if (block_array[i][j][0] == 1){
        TFTscreen.fill(255,255,255);
        TFTscreen.stroke(255,255,255);
        TFTscreen.rect(block_array[i][j][1],block_array[i][j][2],block_w,block_h);
      }else if (block_array[i][j][0] == 0){
        TFTscreen.fill(0,0,0);
        TFTscreen.stroke(0,0,0);
        TFTscreen.rect(block_array[i][j][1],block_array[i][j][2],block_w,block_h);
      }
    }
  } 
}

void win_lose(){
  
}

void drawpad(){
  TFTscreen.fill(0,0,0);
  TFTscreen.stroke(0,0,0);

  TFTscreen.rect(pad_x,scr_h-pad_dist-pad_h,pad_w,pad_h);

  pad_x = map(analogRead(A0),0,1023,0,scr_w-pad_w);
  
  TFTscreen.fill(255,255,255);
  TFTscreen.stroke(255,255,255);

  TFTscreen.rect(pad_x,scr_h-pad_dist-pad_h,pad_w,pad_h);
}

void drawball(){

  if (!alive){
    ball_direction = 'd';
  }

  TFTscreen.fill(0,0,0);
  TFTscreen.stroke(0,0,0);

  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);

  if (ball_direction == 'u'){
    ball_center_y -= ball_vert_speed;
  }else if (ball_direction == 'd'){
    ball_center_y += ball_vert_speed;
  }
  if (ball_direction == 'l'){
    ball_center_x -= ball_hori_speed;
  }else if (ball_direction == 'r'){
    ball_center_y += ball_hori_speed;
  } 

  TFTscreen.fill(255,255,255);
  TFTscreen.stroke(255,255,255);
  
  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);
}

void main_collisions(){
  if (((ball_center_y + ball_radius) >= (scr_h - pad_dist - pad_h)) && ((ball_center_y - ball_radius) <= (scr_h - pad_dist))){
    if ((ball_center_x >= pad_x) && (ball_center_x <= (pad_x + pad_w))){
      ball_vert_speed *=-1;
    }
  }
  if (((ball_center_x + ball_radius) >= scr_w)  || (ball_center_x - ball_radius) <= 0){
    ball_hori_speed *= -1;
  }
  if ((ball_center_y - ball_radius) <= 0){
    ball_vert_speed *= -1;
  }
}


void block_collisions(){
  for (int i = 0;i<rows;i++){
    for (int j = 0;j<lvl_coll;j++){
      if (block_array[i][j][0] == 1){
        if (abs(ball_center_y - (block_array[i][j][2] + block_h/2)) <= block_h/2 + ball_radius && 
			abs(ball_center_x - (block_array[i][j][1] + block_w/2)) <= block_w/2 + ball_radius
		   ){ //with block_h/2 being blockcenter_y and block_w/2 being blockcenter_x
		   if(ball_center_y < block_array[i][j][2] || ball_center_y > block_array[i][j][2] + block_h){ //collision from up or down
			   ball_vert_speed *= -1;
		   }else{
			   ball_hori_speed *= -1;
		   }            
            block_array[i][j][0] = 0;
            draw_blocks();
            break;
        }
      }
    }
  }
}

void setup(){
  TFTscreen.begin();
  TFTscreen.background(0,0,0);
}

void loop(){
  initial();
  
  while (alive){
    win_lose();
    drawpad();
    drawball();
    main_collisions();
    block_collisions();
  }
}
