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
int ball_hori_speed = 0;
int ball_vert_speed = -2;
int const ball_radius = 3;
int ball_center_x;
int ball_center_y;
int player_score = 0;
bool again;
int lifes = 3;
bool complete = false;
int const max_level = 15;
int lvl_coll = 1;
int const rows = 5;
int block_w;
int const block_h = 10;
int block_array[max_level][rows][3];

void initial(){
  int block_w_sum = 0;
  int block_h_sum = 0;
  int space = 0;
  again = true;
  ball_center_x = scr_w/2;
  ball_center_y = scr_h/2;
  block_w = (scr_w/lvl_coll)-1;
  
  for (int i = 0;i<lvl_coll;i++){
    if (block_w_sum + block_w <= scr_w){
        block_w_sum += block_w + 1;
    }
    space = (scr_w-block_w_sum)/2;
  }
  
  for (int i = 0;i<rows;i++){ //NEED TO FIX SOMETHING WITH ROWS...
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

void win(){
  if (complete == true){
    TFTscreen.background(0,0,0);
    TFTscreen.stroke(0,255,0);
    TFTscreen.textSize(3);
    TFTscreen.text("Good Job!",10,40);
    delay(1000);
    
    if (lvl_coll < max_level){
      TFTscreen.stroke(255,255,255);
      TFTscreen.textSize(2);
      TFTscreen.text("Next Level:",5,80);
      char lvl[3];
      lvl_coll++;
      String(lvl_coll).toCharArray(lvl,3);
      TFTscreen.text(lvl,135,80);
    }else{
      TFTscreen.background(0,0,0);
      TFTscreen.stroke(0,255,0);
      TFTscreen.textSize(3);
      TFTscreen.text("YOU WON!",10,40);
      delay(1000);
      TFTscreen.stroke(255,255,255);
      TFTscreen.textSize(2);
      TFTscreen.text("Your Score:",5,80);
      char score[4];
      String(player_score).toCharArray(score,4);
      TFTscreen.text(score,120,80);
    }
    again = false;
    delay(2000);
    TFTscreen.background(0,0,0);
  }
}

void lose(){
  if (lifes == 0){
    TFTscreen.background(0,0,0);
    TFTscreen.stroke(255,0,0);
    TFTscreen.textSize(3);
    TFTscreen.text("GAME OVER",0,40);
    delay(1000);
    TFTscreen.stroke(255,255,255);
    TFTscreen.textSize(2);
    TFTscreen.text("Restarting:",0,80);
    
    TFTscreen.text("Score:",0,0);
    char score[4];
    String(player_score).toCharArray(score,4);
    TFTscreen.stroke(0,255,0);
    TFTscreen.text(score,90,0);
    for (int i = 10;i >= 0;i--){
      char timmer[3];
      String(i).toCharArray(timmer,3);
      if (i <= 3){
        TFTscreen.stroke(255,0,0);
      }else{
        TFTscreen.stroke(0,255,0);
      }
      
      TFTscreen.text(timmer,138,80);
      delay(1000);
      TFTscreen.stroke(0,0,0);
      TFTscreen.text(timmer,138,80);
    }
    TFTscreen.stroke(0,0,0);
    TFTscreen.text(score,120,80);
    again = true;
    lifes = 3;
    player_score = 0;
    TFTscreen.background(0,0,0);
  }
}

void draw_pad(){
  int pad_x_old = pad_x;
  pad_x = map(analogRead(A0),0,1023,0,scr_w-pad_w);

  if (pad_x != pad_x_old){
    TFTscreen.fill(0,0,0);
    TFTscreen.stroke(0,0,0);
    TFTscreen.rect(pad_x_old,scr_h-pad_dist-pad_h,pad_w,pad_h);
    
    TFTscreen.fill(255,255,255);
    TFTscreen.stroke(255,255,255);
    TFTscreen.rect(pad_x,scr_h-pad_dist-pad_h,pad_w,pad_h);
  }
}

void draw_ball(){

  TFTscreen.fill(0,0,0);
  TFTscreen.stroke(0,0,0);

  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);

  ball_center_y -= ball_vert_speed;
  ball_center_x -= ball_hori_speed;

  TFTscreen.fill(255,255,255);
  TFTscreen.stroke(255,255,255);
  
  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);
}

void main_collisions(){
  if (((ball_center_y + ball_radius) >= (scr_h - pad_dist - pad_h)) && ((ball_center_y - ball_radius) <= (scr_h - pad_dist))){
    if ((ball_center_x >= pad_x) && (ball_center_x <= (pad_x + pad_w))){
      if ((ball_center_x < (pad_x + (pad_w/6))) || (ball_center_x > (pad_x + pad_w - (pad_w/6)))){
        ball_hori_speed = 3;
      }else if ((ball_center_x < (pad_x + (pad_w/4))) || (ball_center_x > (pad_x + pad_w - (pad_w/4)))){
        ball_hori_speed = 2;
      }else if ((ball_center_x < (pad_x + (pad_w/2))) || (ball_center_x > (pad_x + pad_w - (pad_w/2)))){
        ball_hori_speed = 1;
      }
      if ((ball_center_x < pad_x + pad_w/2) && (ball_hori_speed < 0)){
        ball_hori_speed *= -1;        
      }else if ((ball_center_x > pad_x + pad_w/2) && (ball_hori_speed > 0)){
        ball_hori_speed *= -1;
      }
      ball_vert_speed *= -1;
    }
  }
  if (((ball_center_x + ball_radius) >= scr_w)  || (ball_center_x - ball_radius) <= 0){
    ball_hori_speed *= -1;
  }
  if ((ball_center_y - ball_radius) <= 0){
    ball_vert_speed *= -1;
  }
  if ((ball_center_y + ball_radius) >= scr_h){
    char print_lifes[2];
    String(lifes).toCharArray(print_lifes,2);
    TFTscreen.stroke(0,0,0);
    TFTscreen.text(print_lifes,150,100);
    lifes -= 1;
    again = false;
  }
}


void block_collisions(){
  complete = true;
  for (int i = 0;i<rows;i++){
    for (int j = 0;j<lvl_coll;j++){
      if (block_array[i][j][0] == 1){
        complete = false;
        if ((abs(ball_center_y - (block_array[i][j][2] + block_h/2)) <= (block_h/2 + ball_radius)) && (abs(ball_center_x - (block_array[i][j][1] + block_w/2)) <= (block_w/2 + ball_radius))){
          if ((ball_center_y < block_array[i][j][2]) || (ball_center_y > (block_array[i][j][2] + block_h))){
            ball_vert_speed *= -1;
          }else{
            ball_hori_speed *= -1;
          }  
          block_array[i][j][0] = 0;
          player_score += lvl_coll;
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
  lose();

  if (lifes == 3){
      TFTscreen.textSize(3);
  
      TFTscreen.stroke(255,255,255);
      TFTscreen.text("READY?!",25,50);
      delay(2000);
      TFTscreen.stroke(0,0,0);
      TFTscreen.text("READY?!",25,50);
  }
  
  draw_blocks();
  delay(1000);

  TFTscreen.textSize(1);
  TFTscreen.stroke(255,255,255);
  TFTscreen.text("Lifes:",100,100);
  char print_lifes[2];
  String(lifes).toCharArray(print_lifes,2);
  TFTscreen.stroke(0,255,0);
  TFTscreen.text(print_lifes,150,100);
  
  while (again){
    draw_ball();
    draw_pad();
    main_collisions();
    block_collisions();
    win();
    delay(20);
  }
}
