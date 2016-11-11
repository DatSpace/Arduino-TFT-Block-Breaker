#include <TFT.h>  //Arduino LCD library
#include <SPI.h> //Arduino SPI library

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

int const scr_w = TFTscreen.width(); //Variable to hold screen width
int const scr_h = TFTscreen.height(); //Variable to hold screen height
int const pad_dist = 8; //Variable to hold the pad distance from screen
int const pad_w = 30; //Variable to hold pad width
int const pad_h = 5; //Variable to hold pad height
int pad_x = scr_w/2; //Variable to hold left top corner of pad (x)
int ball_hori_speed = 0; //Variable to hold left top corner of pad (y)
int ball_vert_speed = -2; //Ball's vertical speed (Downwards)
int const ball_radius = 3; //Ball's radius
int ball_center_x; //Ball center x
int ball_center_y; //Ball center y
unsigned int player_score = 0; //Player score
bool again; //Boolean to hold if the programm should exit or not, the while loop
int lifes = 3; //Lifes
bool complete = false; //boolean to hold if user completed a level
int const max_level = 15; //max_level (needed fot array declaration
unsigned int lvl_coll = 1; //The initial level (increases)
int const rows = 5; //Rows
unsigned int block_w; //Blocks width(Determined by lvl_col
int const block_h = 10; //Blocks height
int block_array[max_level][rows][3]; //Three diamensional array that holds if a block is destroyd (0 = destroyd, 1 = exists)(needs to be drawn or check collisions)
//and the coordinated of the possible blocks per level (re-saves at each level)
//block_w is determined by the currentl level. If level is one block_w is the whole screen, if level is two, block_w is half the screen and so on...

void initial(){ //Runs every time the user starts the game or loses a life
  int block_w_sum = 0; //Holds the sum of all blocks width
  int block_h_sum = 0; //Holds the sum of all blocks height
  int space = 0; //Holds the space left in the screen after all blocks are drawn (all blocks are equal and some times space is left
  again = true; //Makes sure that the programm will run into the while loop at least once
  ball_center_x = scr_w/2; //Sets the ball center x initially at the center of the screen
  ball_center_y = scr_h/2; //Sets the ball center y initially at the center of the screen
  block_w = (scr_w/lvl_coll)-1; //Calculation of block width, the -1 serves a space between the blocks
  
  for (int i = 0;i<lvl_coll;i++){
    if (block_w_sum + block_w <= scr_w){
        block_w_sum += block_w + 1; //Calculates the width sum
    }
    space = (scr_w-block_w_sum)/2; //Calculates the space needed
  }
  
  for (int i = 0;i<rows;i++){
    block_w_sum = space; //In every loop block width sum becomes equal to space, so that the starting point of every first block is plus the space
    for (int j = 0;j<lvl_coll;j++){
      block_array[i][j][1] = block_w_sum; //Makes the x coordinate of that block equal to the current block width sum
      block_array[i][j][2] = block_h_sum; //Makes the y coordinate of that block equal to the current block height sum
      if (block_w_sum + block_w <= scr_w){ //If the existing block coordinate plus the block width is less than the screen
        block_w_sum += block_w + 1; //Add the block width to the block width sum
        block_array[i][j][0] = 1; //Make the block appear
      }
    }
    block_h_sum += block_h + 1; //Add block height to block height sum
  }
}

void draw_blocks(){ //Draws all the blocks by checking if the array for the block is 1 or 0, this is called in every collission so that the blocks can disappear as well
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

void win(){ //Check if the user has won
  if (complete == true){ //If the user has completed the level, clear the screen and print a message
    TFTscreen.background(0,0,0);
    TFTscreen.stroke(0,255,0);
    TFTscreen.textSize(3);
    TFTscreen.text("Good Job!",10,40);
    delay(1000);
    
    if (lvl_coll < max_level){ //If the current level is not the latest level, print next level
      TFTscreen.stroke(255,255,255);
      TFTscreen.textSize(2);
      TFTscreen.text("Next Level:",5,80);
      char lvl[3];
      lvl_coll++;
      String(lvl_coll).toCharArray(lvl,3);
      TFTscreen.text(lvl,135,80);
    }else{//Else print that the user has won, print the score
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
    again = false; //Make again false so that it doesnt go in the loop and it needs to go to the initial()
    delay(2000);
    TFTscreen.background(0,0,0); //Delay and clear everything to start again
  }
}

void lose(){ //Check if user have lost, when the ball passes the screen
  if (lifes == 0){ //If lifes become 0 then the user has lost, print GAVE OVER, countdown to restart and show score
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
    lifes = 3; //Make restart initiliazations
    lvl_coll = 1;
    player_score = 0;
    TFTscreen.background(0,0,0);
  }
}

void draw_pad(){ //Draws the pad whenever the potition changes
  int pad_x_old = pad_x; //Make the old potition equal to the previus one
  pad_x = map(analogRead(A0),0,1023,0,scr_w-pad_w); //Set the new potition to the reading from the INPUT

  if (pad_x != pad_x_old){ //If the potition has changed draw the new pad and delete the old
    TFTscreen.fill(0,0,0);
    TFTscreen.stroke(0,0,0);
    TFTscreen.rect(pad_x_old,scr_h-pad_dist-pad_h,pad_w,pad_h);
    
    TFTscreen.fill(255,255,255);
    TFTscreen.stroke(255,255,255);
    TFTscreen.rect(pad_x,scr_h-pad_dist-pad_h,pad_w,pad_h);
  }
}

void draw_ball(){ //Draw the ball, needs to be done constanlty as the ball never stops, deletes the previeus ball and draws the new
  TFTscreen.fill(0,0,0);
  TFTscreen.stroke(0,0,0);
  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);

  ball_center_y -= ball_vert_speed; //Changes the ball center y according to the potition
  ball_center_x -= ball_hori_speed; //Changes the ball center x accoriding to the potition

  TFTscreen.fill(255,255,255);
  TFTscreen.stroke(255,255,255);
  TFTscreen.circle(ball_center_x,ball_center_y,ball_radius);
}

void main_collisions(){ //Checks for collissions of the ball and the screen boundaries
  if (((ball_center_y + ball_radius) >= (scr_h - pad_dist - pad_h)) && ((ball_center_y - ball_radius) <= (scr_h - pad_dist))){ //If the ball is between the top and bottom values of the pad
    if ((ball_center_x >= pad_x) && (ball_center_x <= (pad_x + pad_w))){ //If the ball is between the left and right values of the pad
      if ((ball_center_x < (pad_x + (pad_w/6))) || (ball_center_x > (pad_x + pad_w - (pad_w/6)))){ //If the ball hits on the edge of the pad set the speed
        ball_hori_speed = 3;
      }else if ((ball_center_x < (pad_x + (pad_w/4))) || (ball_center_x > (pad_x + pad_w - (pad_w/4)))){ //If the ball hits near the edge of the pad set the speed
        ball_hori_speed = 2;
      }else if ((ball_center_x < (pad_x + (pad_w/2))) || (ball_center_x > (pad_x + pad_w - (pad_w/2)))){ //If the ball hits near the center of the pad set the speed
        ball_hori_speed = 1;
      }
      if (ball_center_x < pad_x + pad_w/2){ //If the ball hits on the left side of the pad make it go on the left side
        ball_hori_speed *= -1;
      }//Else if the ball hits on the right side of the pad leave it with a positive speed
      ball_vert_speed *= -1; //Because it hit on the pad, change vertical speed to the oposit so it goes upwards
    }
  }
  if (((ball_center_x + ball_radius) >= scr_w)  || (ball_center_x - ball_radius) <= 0){ //If the ball hits in the left or right side of the screen change direction
    ball_hori_speed *= -1;
  }
  if ((ball_center_y - ball_radius) <= 0){ //If the ball hits on top of the screen change direction
    ball_vert_speed *= -1;
  }
  if ((ball_center_y + ball_radius) >= scr_h){ //If the ball passes the bottom of the screen remove one life and check if the user lost, make again = false, so that it prints the blocks again
    char print_lifes[2];
    String(lifes).toCharArray(print_lifes,2);
    TFTscreen.stroke(0,0,0);
    TFTscreen.text(print_lifes,150,100);
    lifes -= 1;
    lose();
    again = false;
  }
}


void block_collisions(){ //Check if ball hits any of the blocks
  if((rows * block_h + (rows - 1)) <= (ball_center_y - ball_radius)){ //Check for block collision only when the ball is in the possible collisions area
	  complete = true; //Make completing the level true
	  for (int i = 0;i<rows;i++){
		  for (int j = 0;j<lvl_coll;j++){
		    if (block_array[i][j][0] == 1){ //If the current block is visible
			    complete = false; //If there is at least one block, that means that the user havent won yet
			    if ((abs(ball_center_y - (block_array[i][j][2] + block_h/2)) <= (block_h/2 + ball_radius)) && 
			    (abs(ball_center_x - (block_array[i][j][1] + block_w/2)) <= (block_w/2 + ball_radius))){ //If the ball gits the block
			      if ((ball_center_y < block_array[i][j][2]) || (ball_center_y > (block_array[i][j][2] + block_h))){ //If the ball center is above or bellow the block, then change vertical speed
				    ball_vert_speed *= -1;
			      }else{ //Else if the ball center is between the block then it hit from the side, so change the horizontal speed
				    ball_hori_speed *= -1;
			      }  
			      block_array[i][j][0] = 0; //Set the block to 0, so its not drawn again
			      player_score += lvl_coll; //Add the block score
			      draw_blocks(); //Remove the block from the screen
			      break; //Break so it doesnt check for every one of the possible collisions
			    }
		    }
		  }
	  }
	}
}

void setup(){
  TFTscreen.begin(); //Start the screen
  TFTscreen.background(0,0,0); //Clear the background
}

void loop(){
  
  initial();

  if (lifes == 3){ //If it is out of the while loop and the lifes are 3, that means its the initial go. So print some messages
      TFTscreen.textSize(3);
  
      TFTscreen.stroke(255,255,255);
      TFTscreen.text("READY?!",25,50);
      delay(2000);
      TFTscreen.stroke(0,0,0);
      TFTscreen.text("READY?!",25,50);
  }
  
  draw_blocks(); //Draw the blocks for the first time (and then again whenever collision happens)
  delay(1000);

  TFTscreen.textSize(1);
  TFTscreen.stroke(255,255,255);
  TFTscreen.text("Lifes:",100,100); //Print lifes on the screen
  char print_lifes[2];
  String(lifes).toCharArray(print_lifes,2);
  TFTscreen.stroke(0,255,0);
  TFTscreen.text(print_lifes,150,100);
  
  while (again){ //The while loop for the game to work
    draw_ball();
    draw_pad();
    main_collisions();
    block_collisions();
    win();
    delay(20);
  }
}
