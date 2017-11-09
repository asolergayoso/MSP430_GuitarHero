/************** ECE2049 LAB 2 ******************/
/**************  13 April 2017   ******************/
/***************************************************/

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include "peripherals.h"

#define TSTAR 42
#define PIRATES 63

typedef struct {
	int pitch;
	char duration;
	char LED;
}note;



// Function Prototypes
void swDelay(char numLoops);// causes delay
void configbutt();// configures buttons
int statebutt();//chooses what state buttons are in after input
void configLED(char inbits);//configures and lights up LEDs
void notecreator(int pitchy, char duration, note* first);
void songCreator(note* array[], int notes[], char durations[], char length, char locations[]);
void displayWelcome();
//void cascade(note* note);
void countDown();
void printScreen(char durations[], int notes[], char length);
char chooseSong();
void playSong (note* note);
void refreshScreen();
char checkExit();
//void takeInput();




// Declare globals here
unsigned int timer_cnt = 0; //timer															//long long int?
unsigned int leap = 0;
char state = 0;
char counter = 0x30;
unsigned char currKey=0;
char press;
int points = 0;
note* twinkleStar[TSTAR];
note* pirates[PIRATES];



//Notes in Hz
int A = 440;
int Bf = 466;
int B  = 494;
int CC = 523;
int Cs = 554;
int D = 587;
int Ef = 622;
int E = 659;
int F = 698;
int Fs = 740;
int G = 784;
int Af = 831;
int A1 = 880;
int Bf1 = 932;



void runtimerA2(void){
	TA2CTL = TASSEL_1 | ID_0 | MC_1;
	TA2CCR0 = 81;//8191;  //0.0025 seconds
	TA2CCTL0 = CCIE;
}

//ISR
#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void)
{
		if(timer_cnt <= 100){
			timer_cnt ++;
		}else{
			timer_cnt = 1;
			leap++;
		}
		if (leap == 65535)leap = 1;
}

// Main
void main(void){

	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    _BIS_SR(GIE);                   // enable interrupts

    //setup functions
    initLeds();
    configDisplay();
    configButt();
    //configKeypad();

    int piratesnote[] = {CC,D,D,D,E,F,F,F,G,E,E,D,CC,CC,D,0,A,CC,D,D,D,E,F,F,F,G,E,E,D,CC,D,0,A,CC,D,D,D,F,G,G,G,A1,Bf1,Bf1,A1,G,A1,D,D,E,F,F,G,A1,D,0,D,F,E,E,F,D,E};
    char piratesdur[] = {0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1};
    char locatStar[TSTAR];
	int twinklenote[] = {CC,CC,G,G,A1,A1,G,F,F,E,E,D,D,CC,G,G,F,F,E,E,D,G,G,F,F,E,E,D,CC,CC,G,G,A1,A1,G,F,F,E,E,D,D,CC};
	char twinkledur[] = {1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2 };
	char locatPirates[PIRATES];
	unsigned char disp[4];

    while(1){
    	   switch (state){
    	    case 0: //Welcome & Count down
    	    	displayWelcome();
    	    	char led = 0;
    	    	//char med;
    	    	int n = 1;

    	    	runtimerA2();

    	    	while(1){
    	    		setLeds(led);
    	    		currKey = getKey();
    	    		if (leap > n){
    	    			led = (2*led) + 1;  //turn a new LED each 0.25 sec
    	    			n = leap;
    	    		}else if (leap == 0){
    	    			n = 0;
    	    		}

    	    		if (led > 15)led = 0;

    	    		if (currKey == '*'){
    	    			state = 1;
    	    			setLeds(0);
    	    			break;
    	    		}
    	     	}
    	    	break;

    	     case 1: //Play (create random sequence and take user input)
    	    	points = 0;
    	    	GrClearDisplay(&g_sContext);
    	    	char song = chooseSong();
    	    	if (song == 1){
    	        	countDown();
    	        	songCreator(twinkleStar, twinklenote, twinkledur, TSTAR, locatStar);
    	    	}else if (song == 2){
    	    		countDown();
    	    		songCreator(pirates, piratesnote, piratesdur, PIRATES, locatPirates);
    	    	}
    	    	//state = 2;
    	    	break;
    	     default:
    	    		break;
    	    case 2: // Take Input and Check answer
    	        GrClearDisplay(&g_sContext); // Clear the display
    	    	disp[0] = ' ';
    	        disp[1] = 0x30 + (points / 10);
    	    	disp[2]=  0x30 + (points % 10);
    	    	disp[3] = ' ';
    	    	char* point0 = "Good Job!";
    	    	char* point1 = "You scored";
    	    	char* point2 = "points!";
    	    	GrStringDrawCentered(&g_sContext, point0, AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    	    	GrStringDrawCentered(&g_sContext, point1, AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
    	    	GrStringDrawCentered(&g_sContext, disp, 4, 26, 55, TRANSPARENT_TEXT);
    	    	GrStringDrawCentered(&g_sContext, point2, AUTO_STRING_LENGTH, 58, 55, TRANSPARENT_TEXT);
    	    	GrFlush(&g_sContext);
    	    	state = 0;
    	    	while(1){
    	    	  if (checkExit() == 1)break;
    	    	}
    	    	break;
    	    case 3:{
    	        GrClearDisplay(&g_sContext); // Clear the display
    	    	disp[0] = ' ';
    	        disp[1] = 0x30 + (points / 10);
    	    	disp[2]=  0x30 + (points % 10);
    	    	disp[3] = ' ';
    	        char* point3 = "Horrible!";
    	        char* point4 = "Try Again...";
    	        char* point5 = "Score:";
    	        GrStringDrawCentered(&g_sContext, point3, AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    	        GrStringDrawCentered(&g_sContext, point4, AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    	        GrStringDrawCentered(&g_sContext, point5, AUTO_STRING_LENGTH, 38, 65, TRANSPARENT_TEXT);
    	        GrStringDrawCentered(&g_sContext, disp, 4, 62, 65, TRANSPARENT_TEXT);
    	        GrFlush(&g_sContext);
    	        state = 0;
    	    	while(1){
    	        	if (checkExit() == 1)break;
    	        }

    	        break;
    	    }
    	 }
}
}

void displayWelcome(){
		configDisplay();
		configKeypad();
        tRectangle myRectangle = {12, 18, 84, 32};
	    GrClearDisplay(&g_sContext); // Clear the display
	    // Write welcome message
	    char* word1 = "MSP430 Hero";
	    char* word2 = "Press * to start";
	  	GrStringDrawCentered(&g_sContext, word1, AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
	  	GrStringDrawCentered(&g_sContext, word2, AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
	    GrRectDraw(&g_sContext, &myRectangle);
	  	GrFlush(&g_sContext);
}
void swDelay(char numLoops)
{
	volatile unsigned int i,j;	// volatile to prevent optimization
			                            // by compiler

	for (j=0; j<numLoops; j++)
    {
    	i = 30000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}

char chooseSong(){
	char song;
	GrClearDisplay(&g_sContext); // Clear the display
	tRectangle myRectangle = {2, 28, 94, 42};
	tRectangle myRectangle2 = {2, 43, 94, 57};
	char* choose= "Choose your song";
	char* press = "Press * to play";
    char* song1 = "1. Twinkle Star";
	char* song2 = "2. Pirates";
	GrStringDrawCentered(&g_sContext, choose, AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, song1, AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, song2, AUTO_STRING_LENGTH, 48, 50, TRANSPARENT_TEXT);
	GrFlush(&g_sContext);

	//GrContextBackgroundSet(&g_sContext, ClrWhite);
	while(1){

			currKey = getKey();
			switch (currKey){
			case '1':
				song = 1;
			    GrRectDraw(&g_sContext, &myRectangle);
			    GrStringDrawCentered(&g_sContext, press, AUTO_STRING_LENGTH, 48, 75, TRANSPARENT_TEXT);
			    GrFlush(&g_sContext);
			    break;
			case '2':
				 //myRectangle = {2,43, 94, 57};
				 song = 2;
				 GrRectDraw(&g_sContext, &myRectangle2);
				 GrStringDrawCentered(&g_sContext, press, AUTO_STRING_LENGTH, 48, 75, TRANSPARENT_TEXT);
				 GrFlush(&g_sContext);
				 break;
			default:
				break;
			}
			while(1){
				unsigned char  cKey = getKey();
				if (cKey == '*'){
					return song;
				}else{
					//refreshScreen();
					currKey = cKey;
					break;
				}
			}

	}
}

void noteCreator(int pitchy, char duration, note* first){
	first->pitch = pitchy;
	first->duration = duration;
	if (first->pitch == CC || first->pitch == G){
		first->LED = 8;
	}else if (first->pitch == D || first->pitch == A1){
		first->LED = 4;
	}else if (first->pitch == E || first->pitch == Bf1){
		first->LED = 2;
	}else if (first->pitch == F || first->pitch == A){
		first->LED = 1;
	}
}

void countDown(){

		unsigned char dispSz = 5;
	    unsigned char disp[5];

	    disp[0] = ' ';
	    disp[4] = ' ';
		GrClearDisplay(&g_sContext); // Clear the display

		char* welcome = "Game starts in";
	 	GrStringDrawCentered(&g_sContext, welcome, AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	  	GrFlush(&g_sContext);

	 	char num = 0x33;
	 	timer_cnt = 1;
	 	leap = 1;
	 	runtimerA2();
	 	while(num > 0x30){
	 		leap = 1;
	 		disp[1] = num;
	 		GrStringDrawCentered(&g_sContext, disp, dispSz, 48, 55, OPAQUE_TEXT);
	 		if (num % 2 == 0){
	 			setLeds(4);
	 		}else{
	 			setLeds(8);
	 		}
	 		GrFlush(&g_sContext);
	 		BuzzerOn(500);
	 		while(1){
	 		 if (leap % 3 == 0){
	 		   BuzzerOff();
	 		   num--;
	 		   break;
	 	   	 }
	 		}
	 	}
	 	disp[1] = 0x47, disp[2] = 0X4F, disp[3] = 0x21;
	 	GrStringDrawCentered(&g_sContext, disp, dispSz, 48, 55, OPAQUE_TEXT);
	 	GrFlush(&g_sContext);
	 	setLeds(15);
	 	BuzzerOn(600);
		leap = 0;
		while(1){
			if (leap >= 2){
				break;
			}
		}
		setLeds(0);
	 	BuzzerOff();
	 	GrClearDisplay(&g_sContext);
	 	leap = 0;
	 	while(1){
	 		if (leap >= 3){
	 			break;
	 		}
	 	}

}

char takeInput(char LED){

	press = stateButt();
	if (stateButt() != 15){
		if (press == 7 && LED == 8)return 0;
		else if (press == 11 && LED == 4){return 0;}
		else if (press == 13 && LED == 2){return 0;}
		else if (press == 14 && LED == 1){return 0;}
		else { return 1;}
	}
	return 2;
}




void printScreen(char durations[], int notes[], char length){

    char n; //loop counter
   // while(1){
    for (n = 0; n < length; n++){
   // if (timer_cnt % 20)return;
	 if (durations[n] < 96){
	   if (notes[n] == CC || notes[n] == G){
		  GrCircleFill(&g_sContext, 20, durations[n], 5);
	   }else if (notes[n] == D || notes[n] == A1){
		  GrCircleFill(&g_sContext, 40, durations[n], 5);
	   }else if (notes[n] == E || notes[n] == Bf1){
		  GrCircleFill(&g_sContext, 60, durations[n], 5);
	   }else if (notes[n] == F || notes[n] == A){
		  GrCircleFill(&g_sContext, 80, durations[n], 5);
	  }
	}

	GrFlush(&g_sContext);
   }
}

void refreshScreen(){

    tRectangle myRectangle = {0,0,96,96};
	GrContextBackgroundSet(&g_sContext, ClrWhite);
	GrRectFill(&g_sContext, &myRectangle);
	//DpyRectFill(&g_sContext, &myRectangle, ClrWhite);
	GrFlush(&g_sContext);
}


void playSong (note* note){
	char played = 0;
	configLEDs(0);
	leap = 0;
	int timer = 0;
	char correct; // checks for correct user input
	BuzzerOn(note->pitch);
	setLeds(note->LED);
	while(1){
	  if (checkExit() == 1)return;
      correct = takeInput(note->LED);

      if (correct == 0 && played == 0){
    	  points += 2;
    	  played = 1;
      }else if (correct == 1){configLEDs(1);}

	  if (leap > (timer + note->duration)){
		BuzzerOff();
		setLeds(0);
		//timer = leap;
		break;
	  }
	}
	/*if (correct == 0){points += 2;}
	else if (correct == 1){configLEDs(1);}*/
}


void songCreator(note* array[], int notes[], char durations[], char length, char locations[]){

	unsigned char disp[4];
	char n; //loop counter
	char yaxis = 96;
	char p;  //loop counter
	disp[0] = ' ';
	disp[3] = ' ';
	char*score = "Score:";
	GrStringDrawCentered(&g_sContext, score, AUTO_STRING_LENGTH, 38, 65, TRANSPARENT_TEXT);
	GrFlush(&g_sContext);
	for (p = 1; p <= length; p++){
		locations[p] = durations[p-1] - 5;
	}
	for (n = 0; n < length; n++){
		array[n] = malloc(sizeof(note));
		noteCreator(notes[n],durations[n], array[n]);

		while(1){

		    disp[1] = 0x30 + (points / 10);
		    disp[2]=  0x30 + (points % 10);
		    GrStringDrawCentered(&g_sContext, disp, 4, 62, 65, OPAQUE_TEXT);
		    GrFlush(&g_sContext);

			//GrClearDisplay(&g_sContext); // Clear the display
			//printScreen(locations, notes, length);
			//GrFlush(&g_sContext);
			//refreshScreen();
			if (checkExit() == 1)return;
			//if (locations[n] == 80){
			playSong(array[n]);
			break;
			//}
			//for (p = 0; p < length; p++){
			//	locations[p] += 10;
			//}
		}
		timer_cnt = 1;
		while (1){
			if (timer_cnt % 10 == 0)break;
		}
		free(array[n]);
    }
	if (points >= ((length*2) / 3)){state = 2;}
	else {state = 3;}
}

char checkExit(){
	if (getKey() == '#'){
		BuzzerOff();
		setLeds(0);
		state = 0;
		return 1;
	}else{
		return 0;
	}
}








