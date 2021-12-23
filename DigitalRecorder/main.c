/*
 * The code herein is the sole property of Thomas Trombino and Dr. Tony Givargis (where noted)
 * of the University of California, Irvine. The use of this code for any purpose other than personal projects 
 * is prohibited. 
 * 3/21/21
 * 
 */ 

#include "avr.h"
#include "avr.c"
#include "lcd.h"
#include "lcd.c"
#include <string.h>
#include <stdio.h>


#define SPK 3
#define SONG_SIZE 50
#define A0 45 
#define A0s 43
#define B0 40
#define C0 38
#define C0s 36
#define D0 34
#define D0s 32
#define E0 30
#define F0 29
#define F0s 27
#define G0 26
#define G0s 24
#define A1 23
#define A1s 21
#define B1 20
#define C1 19
#define C1s 18
#define D1 17
#define D1s 16
#define E1 15
#define F1 14
#define F1s 14
#define G1 13
#define G1s 12
#define REST 0

int noteArray[] = {
	0,
	C0,
	D0 - 1,
	E0 - 2,
	0,
	F0 - 4,
	G0 - 5,
	A1 - 5,
	0,
	B1 - 6,
	C1 - 7,
	D1 - 8
};

int trueArray[] = {
	0,
	C0,
	D0,
	E0 + 3,
	0,
	F0 - 1 ,
	G0 - 1,
	A1 - 1,
	0,
	B1,
	C1 - 1,
	D1 - 1
};


int is_pressed(int r, int c) 
{
	/* set all pins of PORTC to N/C */ 
	DDRC = 0;
	PORTC = 0;
	
	
	/* set Pin C at location r to 0. Make that particular pin, using bit widdling function, 
		to be output and then set its state to be 0 (i.e. strong output 0 on row r) */ 
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);
	
	/* set Pin C at location c to W1 (weak 1) */ 
	CLR_BIT(DDRC, c + 4);
	SET_BIT(PORTC, c + 4);
	
	/* if (Pin C at location c is 1) return 0 (i.e. not pressed) else return 1 (pressed) */
	
	//return 1;
	avr_wait_note(1);
	if (GET_BIT(PINC, c + 4)) {
		return 0;
	}
	else {
		return 1;
	} 
	
	//return !(GET_BIT(PINC, c));
}

int get_key() {
	int r, c; 
	
	for(r = 0; r < 4; ++r) {
		for(c = 0; c < 4; ++c) {
			if(is_pressed(r, c)) {
				return 1 + (r * 4) + c;
			}
		}
	}
	return 0;  
	
}

typedef struct note {
	
	int freq;
	int duration;
	
	}note;
	
typedef struct song {
	
	note * sng;
	char artist[18];
	char title[18];
	
}song;


void playNote(int freq, int duration)
{
	int i;
	if (freq == 0) {
		for(i = 0; i < duration; ++i) {
			CLR_BIT(PORTB, SPK);
			avr_wait_note(53);
			CLR_BIT(PORTB, SPK);
			avr_wait_note(53);
		}
	}
	else {
		for(i = 0; i < duration; ++i) {
			SET_BIT(PORTB, SPK);
			avr_wait_note(freq);
			CLR_BIT(PORTB, SPK);
			avr_wait_note(freq);
		}
	}
}

void performNote()
{
	int k = get_key();
	int freq = noteArray[k];
	while (k != 0 && k != 13) {
		SET_BIT(PORTB, SPK);
		avr_wait_note(freq);
		CLR_BIT(PORTB, SPK);
		avr_wait_note(freq);
		k = get_key();
	}
		
}

int recordNote(int freq)
{
	int duration = 0;
	int k;
	
	while (k != 0) {
		SET_BIT(PORTB, SPK);
		avr_wait_note(freq);
		CLR_BIT(PORTB, SPK);
		avr_wait_note(freq);
		k = get_key();
		duration++;
	}
	return duration + 20;
}

int recordRest()
{
	int duration = 0;
	int k = 0;
	
	while (k == 0) {
		CLR_BIT(PORTB, SPK);
		avr_wait_note(100);
		CLR_BIT(PORTB, SPK);
		avr_wait_note(100);
		k = get_key();
		duration++;
	}
	return duration;
}

int recordSong (note * song)
{
	int k;
	int i = 0;
	for (i = 0; i < SONG_SIZE; i++) {
		k = get_key();
		if (k == 0) {
			int restDuration = recordRest();
			note n = {0, restDuration};
			song[i] = n;
			continue;
		}
		else if (k != 13) {
			k = get_key();
			if (k != 13) {
				int freq = noteArray[k];
				int duration = recordNote(freq);
				note n = {trueArray[k], duration};
				song[i] = n;
			}
		}
		else if(k == 13) {
			break;
		}
	}
	return i;
}

void playSong (const note * song, int n) 
{
	char buf[18];
	int dur = song[0].duration;
	int tempAdj = 0;
	int freq = 0; 
	int pitchAdj = 0;
	int k = 0;
	for (int i = 0; i < n; ++i) {
		
		dur = song[i].duration;
		freq = song[i].freq;
		k = get_key();
		if(k == 13) {
			break;
		}
			
		playNote(freq, dur);
	}
}

int main(void)
{
	note song1[SONG_SIZE];
	note song2[SONG_SIZE]; 
	note song3[SONG_SIZE];
	int songLengths[3] = {0, 0, 0};
	char buf[6];
	SET_BIT(DDRB, SPK);
	lcd_init();
	lcd_clr();
	int k;
	while(1) {
		lcd_clr();
		lcd_pos(0,0);
		lcd_puts("Select Mode");
		k = get_key();
		if (k == 4) {
			lcd_clr();
			while (k != 13) {
				lcd_pos(0,0);
				lcd_puts("Monitor");
				performNote();
				k = get_key();
			}
		}
		else if (k == 8) {
			int track = 0;
			lcd_clr();
			lcd_pos(0,0);
			lcd_puts("Choose track to");
			lcd_pos(1,0);
			lcd_puts("record...");
			k = 0; 
			while (k != 1 && k != 2 && k != 3){
				k = get_key(); 
				track = k - 1;
			};
		
			lcd_clr();
			lcd_pos(0,0);
			lcd_puts("Recording");
			lcd_pos(1,0);
			sprintf(buf,"Track %d...", track+1); 
			lcd_puts(buf);
			avr_wait(1000);
			
			switch (track) {
				case 0:
					songLengths[0] = recordSong(song1);
					break;
				case 1:
					songLengths[1] = recordSong(song2);
					break;
				case 2:
					songLengths[2] = recordSong(song3);
					break;
			}
			
		}
		else if (k == 12) {
			int track = 0;
			//note songToRecord[SONG_SIZE];
			lcd_clr();
			lcd_pos(0,0);
			lcd_puts("Choose track to play");
			lcd_pos(1,0);
			lcd_puts("play...");
			k = get_key();
			while (k != 1 && k != 2 && k != 3){
				k = get_key();
				track = k - 1;
			};
			
			lcd_clr();
			lcd_pos(0,0);
			lcd_puts("Playing");
			lcd_pos(1,0);
			sprintf(buf,"Track %d...", track+1);
			lcd_puts(buf);
			
			switch (track) {
				case 0:
					playSong(song1, songLengths[0]);
					break;
				case 1:
					playSong(song2, songLengths[1]);
					break;
				case 2:
					playSong(song3, songLengths[2]);
					break;
			}
			
		}
	}
}
