## ATMega32 Monophonic Digital Recorder

This was the final project for UCI's Embedded Systems course where students are tasked with creating an original project 
using the ATMega32 microcontroller by Microchip. 

The digital recorder allows for the user to both play and record "songs" using a 16-key keypad and LCD screen. A short video demonstration and explanation is provided in the link below. 

[Video demonstration](https://youtu.be/3wSD1wcsxZw)

## Structure
In `main.c`, frequency values of the C major scale are hard-coded for the 9 numerical buttons on the keypad (1-9). The ATMega sends high and low digital signals to the attached speaker
at the frequency provided. Thus, the note is produced by the speaker for the duration of the key press. The instrument is monophonic due to only 1 note being able to be played at a given time. 

## Monitor Mode
While in monitor mode, the user can play the "instrument" indefinitely since there are no memory restrictions. A note is played for as long as its key is pressed.

## Record Mode
When recording mode is activated, the pitch and duration of each note (as well as the rests between notes) are saved into 1 of 3 
"song" arrays using the `recordNote()` and `recordRest()` functions respectively. 

Due to memory limitations, each song array is restricted to 50 notes. A note is a `struct` consisting of two 4 byte `int`s for frequency and duration. Thus, there is a total of 8 bytes per note and 400 bytes per song. 

## Breadboard Wire Diagram
![bb](bb.jpg "Breadboard")
