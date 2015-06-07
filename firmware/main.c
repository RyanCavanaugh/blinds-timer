#include <stdio.h>
#include <stdlib.h>
#include <pic.h>
#include <pic16f690.h>
#include <xc.h>

// A0 Add Small Time (PU)
// A1 Add Large Time (PU)
#define ADD_SMALL (!(RA0))
#define ADD_SMALL_PIN TRISA0
#define ADD_LARGE (!(RA1))
#define ADD_LARGE_PIN TRISA1

// A3 N/C (input only)

// A4 Go Up (PU)
// A5 Go Down (PU)
#define GO_UP (!(RA4))
#define GO_UP_PIN TRISA4
#define GO_DOWN (!(RA5))
#define GO_DOWN_PIN TRISA5

// B4 DIP 1 (PU)
// B5 DIP 2 (PU)
// B6 DIP 3 (PU)
// B7 DIP 4 (PU)
#define DIP_1 (!RB4)
#define DIP_1_PIN TRISB4
#define DIP_2 (!RB5)
#define DIP_2_PIN TRISB5
#define DIP_3 (!RB6)
#define DIP_3_PIN TRISB6
#define DIP_4 (!RB7)
#define DIP_4_PIN TRISB7

// C0 M1 Control
// C1 M2 Control
#define MOTOR_UP RC0
#define MOTOR_UP_PIN TRISC0
#define MOTOR_DOWN RC1
#define MOTOR_DOWN_PIN TRISC1

// C2 N/C
// C3 Time: 8 hr
// C4 Time: 4 hr
// C5 Time: 2 hr
// C6 Time: 1 hr
// C7 Time: 30m
#define TIME_5 RC3
#define TIME_5_PIN TRISC3
#define TIME_4 RC4
#define TIME_4_PIN TRISC4
#define TIME_3 RC5
#define TIME_3_PIN TRISC5
#define TIME_2 RC6
#define TIME_2_PIN TRISC6
#define TIME_1 RC7
#define TIME_1_PIN TRISC7

// Macros
#define BLINK(pin) { pin = 1; wait_entire_halfsecond(); pin = 0; }
#define FLASH(pin, value) { pin = value; NOP(); pin = 0; }
#define DIAGNOSE(pin, led) { if(pin) { while(1) { BLINK(led); wait_entire_halfsecond(); } } }
#define BIT(v, b) ((v >> b) & 1)

// Configuration
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit

#define TICKS_PER_HALF_SECOND 210

char halfHours = 0;

void tinysleep() {
    char x = 0;
    for(x = 0; x < 24; x++) {}
}

void nop() { }

void until_next_halfsecond(void func()) {
    while(TMR0 < TICKS_PER_HALF_SECOND) {
        func();
    }
    TMR0 = 0;
}

void for_entire_halfsecond(void func()) {
    TMR0 = 0;
    until_next_halfsecond(func);
}

void for_entire_bounce(void func()) {
    TMR0 = 0;
    while(TMR0 < 30) {
        func();
    }
}

void wait_entire_halfsecond() {
    for_entire_halfsecond(nop);
}

void wait_until_next_halfsecond() {
    until_next_halfsecond(nop);
}

// Executes one cycle of LED light-up with these values
void flash(char t1, char t2, char t3, char t4, char t5) {
    FLASH(TIME_1, t1);
    FLASH(TIME_2, t2);
    FLASH(TIME_3, t3);
    FLASH(TIME_4, t4);
    FLASH(TIME_5, t5);
}

// Wrapper for flash(1, 1, 1, 1, 1)
void flash_all() {
    flash(1, 1, 1, 1, 1);
}

// Flash or display the current half-hour
// countdown value
void flash_halfHours() {
    flash(
            BIT(halfHours, 0),
            BIT(halfHours, 1),
            BIT(halfHours, 2),
            BIT(halfHours, 3),
            BIT(halfHours, 4));
}
void display_halfHours_for_halfsecond() {
    for_entire_halfsecond(&flash_halfHours);
}

// Flash or display the status of the DIP switches
// and pushbuttons
void flash_dip_and_buttons() {
    if(ADD_SMALL || ADD_LARGE || GO_UP || GO_DOWN) {
        flash(ADD_SMALL, ADD_LARGE, GO_UP, GO_DOWN, 1);
    } else {
        flash(DIP_1, DIP_2, DIP_3, DIP_4, 0);
    }
}

// Diagnostics for buttons - they should all be off during startup.
// If one is stuck on, blink its LED
void diagnose_buttons() {
    DIAGNOSE(ADD_LARGE, TIME_1);
    DIAGNOSE(ADD_SMALL, TIME_2);
    DIAGNOSE(GO_UP, TIME_3);
    DIAGNOSE(GO_DOWN, TIME_4);
}

// Light up each indicator light in sequence, twice
void diagnose_lights() {
    // Light up each timer LED in order
    char i;
    for(i = 0; i < 2; i++) {
        BLINK(TIME_1); BLINK(TIME_2);
        BLINK(TIME_3); BLINK(TIME_4);
        BLINK(TIME_5);
    }
}

// Wake-up sequence for diagnostic purposes.
// Lights up each indicator LED in sequence twice, then verifies
// that buttons are correctly in an unpressed state, then
// shows the current status of the DIPs for ~2 seconds
void wakeup() {
    // Ensure indicator lights are set up
    diagnose_lights();
    // Ensure all buttons are 'off'
    diagnose_buttons();
    
    // Show DIP/button values for 2 seconds
    char i;
    for(i = 0; i < 2 * 2; i++) {
        for_entire_halfsecond(&flash_dip_and_buttons);
    }
}

void setup() {
    TIME_1 = TIME_2 = TIME_3 = TIME_4 = TIME_5 = 0;
    MOTOR_UP = MOTOR_DOWN = 0;

    ANSEL = 0; // 00000000, all inputs/outputs are digital
    ANSELH = 0; // 00000000, all inputs/outputs are digital

    // 31 kHz LFINTOSC
    IRCF0 = IRCF1 = IRCF2 = 0;

    // Turn on all pull-ups
    WPUA = 0b11111111;
    WPUB = 0b11111111;

    // A 0: !RABPU, PORTA/B pull-ups enabled
    // B 1: INTEDG, Rising edge interrupt on INT
    // C 0: T0CS, internal instruction cycle clock
    // D 0: T0SE, increment on low-to-hich T0CKI pin
    // E 0: PSA, prescale the Timer0 module
    // F G H 0, 1, 1: PS2:0, prescaler settings, 011 = 16:1
    //             ABCDEFGH
    OPTION_REG = 0b01000011;
    PSA = 0;

    // Output pins 
    TIME_1_PIN = 0; TIME_2_PIN = 0;
    TIME_3_PIN = 0; TIME_4_PIN = 0;
    TIME_5_PIN = 0;
    MOTOR_UP_PIN = 0; MOTOR_DOWN_PIN = 0;
    
    // Input pins
    ADD_SMALL_PIN = 1; ADD_LARGE_PIN = 1;
    GO_UP_PIN = 1; GO_DOWN_PIN = 1;
    DIP_1_PIN = 1; DIP_2_PIN = 1;
    DIP_3_PIN = 1; DIP_4_PIN = 1;

    // All lights and motors off
    TIME_1 = TIME_2 = TIME_3 = TIME_4 = TIME_5 = 0;
    MOTOR_UP = MOTOR_DOWN = 0;
}

void motor_full_up() {
    char cycles = 
        (DIP_1 << 0) |
        (DIP_2 << 1) |
        (DIP_3 << 2) |
        (DIP_4 << 3);
    char i;

    MOTOR_UP = 1;
    for(i = 0; i < cycles; i++) {
        TIME_1 = 1;
        wait_until_next_halfsecond();
        TIME_1 = 0;
        wait_until_next_halfsecond();
    }
    MOTOR_UP = 0;
}

void run_cycle() {
    // Observe all buttons until we get time locked in
    char idleCounter = 0;
    char idle;
    
    while(1) {
        flash_halfHours();
        idle = 0;
        if(GO_UP) {
            MOTOR_UP = 1;
            while(GO_UP) { }
            MOTOR_UP = 0;
        } else if(GO_DOWN) {
            MOTOR_DOWN = 1;
            while(GO_DOWN) {}
            MOTOR_DOWN = 0;
        } else if(ADD_SMALL) {
            halfHours++;
            halfHours = halfHours % 32;
            while(ADD_SMALL) flash_halfHours();
            for_entire_bounce(&flash_halfHours);
        } else if(ADD_LARGE) {
            halfHours += 4;
            halfHours = halfHours % 32;
            while(ADD_LARGE) flash_halfHours();
            for_entire_bounce(&flash_halfHours);
        } else {
            idle = 1;            
        }
        
        // Check idle counting
        if(idle == 0) {
            TMR0 = 0;
            idleCounter = 0;
        } else {
            if(TMR0 >= TICKS_PER_HALF_SECOND) {
                TMR0 = 0;
                idleCounter++;
            }
        }
        
        // After 5 seconds (10 half-seconds), lock in a value
        if(halfHours > 0 && idleCounter >= 10) break;
    }
    
    char i;
    // Enter timer mode, flash confirmation of time setting
    for(i = 0; i < 4; i++) {
        for_entire_halfsecond(&flash_halfHours);
        wait_entire_halfsecond();
    }

    // Execute countdown. User can bail out
    // by pressing GO_UP / GO_DOWN,
    // which will cancel the countdown.
    char halfSecondCounter = 0;
    char minutesCounter = 0;
    char shouldBreak = 0;
    while(!shouldBreak) {
        while((TMR0 <= TICKS_PER_HALF_SECOND) && (shouldBreak == 0)) {
            if(GO_UP || GO_DOWN) {
                // Up/Down pressed, abort countdown
                for(i = 0; i < 4; i++) {
                    for_entire_halfsecond(flash_all);
                    wait_entire_halfsecond();
                }
                while(GO_UP || GO_DOWN) { }
                shouldBreak = 1;
                halfHours = 0;
            }
        }
        TMR0 = 0;
        flash_halfHours();
        
        // Waited half a second
        halfSecondCounter++;
        if(halfSecondCounter == 120) {
            halfSecondCounter = 0;
            minutesCounter++;
        }
        
        // Waited half an hour
        if(minutesCounter == 30) {
            halfHours--;

            // Timer expired, raise up and quit cycle
            if(halfHours == 0) {
                motor_full_up();
                shouldBreak = 1;
            }
        }
    }
}

// Count up to 16 on half-second timing
void clock_test() {
    char i = 0;
    char a, b, c, d;
    while(i < 16) {
        a = (i >> 0) & 1;
        b = (i >> 1) & 1;
        c = (i >> 2) & 1;
        d = (i >> 3) & 1;
        flash(a, b, c, d, 0);
        wait_until_next_halfsecond();
        i++;
    }
}

int main(int argc, char** argv) {
    setup();
    wakeup();

    while(1) {
        // See if we're in a diagnostic mode or regular run cycle
        char dipValue =
            (DIP_1 << 0) |
            (DIP_2 << 1) |
            (DIP_3 << 2) |
            (DIP_4 << 3);
    
        if(dipValue == 0) {
            clock_test();
        } else if(dipValue == 1) {
            // Do 16 seconds of DIP testing
            for(char i = 0; i < 32; i++) {
                for_entire_halfsecond(&flash_dip_and_buttons);
            }
        } else {
            run_cycle();
        }
    }
    
    return (EXIT_SUCCESS);
}
