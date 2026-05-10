//Khoa Duong kduon044@ucr.edu
//Discussion 23
//Final project
/*I intergrated a ducking system alongside another sprite to increase the diffculty a bit. I decided not to add the buzzer as I didn't have enough time and had already reached my three build upons. The system now works perfectly for what
I wanted it to do such as reset,start, save game, jump, duck, and game end conditions.  */

//I achknowledge all content contained herin, excluding template or example code, is my own orginal work.

//https://youtube.com/shorts/h7p_Fh6AwQ0

#include <avr/io.h>
#include <util/delay.h>
#include "timerISR.h"
#include "LCD.h"
#include <stdio.h>
#include <avr/eeprom.h>
#include "spiAVR.h"


int game_playing = 0;
int dino_pos_y = 100;
int dino_jump = 0;
int jump_speed = 0;
int cactus_pos_x = 100;
int cactus_speed = 2;
int dino_duck = 0;
int ptero_pos_y = 90;
int obstacle = 0;
unsigned int score = 0;


void Send_Command(int cmd);
void Send_Data(int data);
void HardwareReset();
void ST7735_init();


#define NUM_TASKS 5



void save_score() {
    eeprom_write_word((uint16_t*)0, score);
}

unsigned int load_score() {
    return eeprom_read_word((uint16_t*)0);
}


typedef struct _task {
    unsigned int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

task tasks[NUM_TASKS];

const unsigned long TASK1_PERIOD = 50;
const unsigned long TASK2_PERIOD = 100;
const unsigned long TASK3_PERIOD = 100;
const unsigned long TASK4_PERIOD = 10;
const unsigned long TASK5_PERIOD = 10;
const unsigned long TASK6_PERIOD = 100;
const unsigned long GCD_PERIOD = 10;

void TimerISR() {
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}



const uint8_t dino_sprite[15][12] = {
    {0,0,0,0,0,0,0,1,1,1,1,1},
    {0,0,0,0,0,0,0,1,0,1,1,1},
    {0,0,0,0,0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0},
    {1,0,0,0,0,0,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,1,1,0,0},
    {1,1,0,0,0,0,1,1,1,1,0,0},
    {1,1,1,0,0,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,0,0,0,0},
    {0,0,1,1,1,1,1,1,0,0,0,0},
    {0,0,1,1,0,0,0,1,1,0,0,0},
    {1,1,1,0,0,0,0,0,1,1,0,0},
    {1,1,1,0,0,0,0,0,0,1,1,0}
}; //dinosaur sprite for tft

const uint8_t duck_dino_spirte[10][15] = {
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,0,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
    {1,1,0,0,0,0,0,0,1,1,1,1,1,1,1},
    {1,1,1,0,0,0,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,1,1,1,1,0,0,0,1,1,0,0,0,0},
    {0,0,1,1,1,1,0,0,0,0,1,1,0,0,0}
}; // Ducking dinosaur sprite for tft

const uint8_t ptero_sprite[10][12] = {
    {0,0,0,0,1,1,1,1,0,0,0,0},  
    {0,0,0,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,0,0},  
    {0,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1,1},  
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1,0},  
    {0,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,1,1,1,0,0,0}   
}; //ptero dinosaur or well tried 


const uint8_t cactus_sprite[20][10] = {
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {1,1,0,1,1,1,1,0,0,0},
    {1,1,0,1,1,1,1,0,0,0},
    {1,1,0,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,0,1,1},
    {1,1,1,1,1,1,1,0,1,1},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,0,0,0}
}; //cactus sprite for tft

void HardwareReset() {
    PORTB &= ~(1 << PB4);
    _delay_ms(200);
    PORTB |= (1 << PB4);
    _delay_ms(200);
}

void ST7735_init() {
    HardwareReset();
    Send_Command(0x01);
    _delay_ms(150);
    Send_Command(0x11);
    _delay_ms(200);
    Send_Command(0x3A);
    Send_Data(0x05); //18 bit color mode
    _delay_ms(10);
    Send_Command(0x36);
    Send_Data(0xC0);
    Send_Command(0x29);
    _delay_ms(200);
}

void Send_Command(int cmd) {
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB2);
    SPI_SEND(cmd);
    PORTB |= (1 << PB2);
}

void Send_Data(int data) {
    PORTB |= (1 << PB0);   
    PORTB &= ~(1 << PB2);  
    SPI_SEND(data);
    PORTB |= (1 << PB2);   
}

enum TFT_STATES {tft_start , tft_draw};
int tft_tick(int state) {
    static unsigned char initialized = 0;
    static int old_dino_y = 100;
    static int old_cactus_x = 100;
    static int old_duck = 0;
    static int old_obstacle = 0;
    

    switch (state) {
        case tft_start:
            state = tft_draw;
            break;

        case tft_draw:
            state = tft_draw;
            break;

        default:
            break;
    }

    switch (state) {
        case tft_draw:
            if (!initialized) {
                // Initialize draw
                Send_Command(0x2C);
                for(int i = 0; i < 128*160; i++) {
                    Send_Data(0xFF);
                    Send_Data(0xFF);
                }
             
                // Draw dinosaur for game start
                for (int row = 0; row < 15; row++) {
                    for (int col = 0; col < 12; col++) {
                        if (dino_sprite[row][col] == 1) {
                            Send_Command(0x2A);  
                            Send_Data(0x00);
                            Send_Data(10 + col);
                            Send_Data(0x00);
                            Send_Data(10 + col);
                            
                            Send_Command(0x2B);  
                            Send_Data(0x00);
                            Send_Data(dino_pos_y + row);
                            Send_Data(0x00);
                            Send_Data(dino_pos_y + row);
                            
                            Send_Command(0x2C);  
                            Send_Data(0x00);  
                            Send_Data(0xE0);
                        }
                    }
                }
                
                // Draw cactus for game start
                for (int row = 0; row < 20; row++) {
                    for (int col = 0; col < 10; col++) {
                        if (cactus_sprite[row][col] == 1) {
                            Send_Command(0x2A);
                            Send_Data(0x00);
                            Send_Data(cactus_pos_x + col);  
                            Send_Data(0x00);
                            Send_Data(cactus_pos_x + col);
                            
                            Send_Command(0x2B);
                            Send_Data(0x00);
                            Send_Data(95 + row);
                            Send_Data(0x00);
                            Send_Data(95 + row);
                            
                            Send_Command(0x2C);
                            Send_Data(0x07);  
                            Send_Data(0x00);
                        }
                    }
                }
                
                //Ground line
                for (int x = 0; x < 128; x++) {
                    Send_Command(0x2A);  
                    Send_Data(0x00);
                    Send_Data(x);
                    Send_Data(0x00);
                    Send_Data(x);

                    Send_Command(0x2B);  
                    Send_Data(0x00);
                    Send_Data(115);
                    Send_Data(0x00);
                    Send_Data(115);

                    Send_Command(0x2C);  
                    Send_Data(0x00);  
                    Send_Data(0x00);
                }
           
                initialized = 1;
            } 
            else {
                // UPDATE DRAW
                
                // Update dinosaur 
                if (old_dino_y != dino_pos_y || old_duck != dino_duck) {
                    // Erase old dino position 
                    if (old_duck) {
                        // Erase duck sprite (10x15)
                        for (int row = 0; row < 10; row++) {
                            for (int col = 0; col < 15; col++) {
                                if (duck_dino_spirte[row][col] == 1) {
                                    Send_Command(0x2A);  
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    
                                    Send_Command(0x2B);  
                                    Send_Data(0x00);
                                    Send_Data(old_dino_y + 5 + row);
                                    Send_Data(0x00);
                                    Send_Data(old_dino_y + 5 + row);
                                    
                                    Send_Command(0x2C);  
                                    Send_Data(0xFF);  
                                    Send_Data(0xFF);
                                }
                            }

                        }

                    } 
                    else {
                        // Erase standing sprite (15x12)
                        for (int row = 0; row < 15; row++) {
                            for (int col = 0; col < 12; col++) {
                                if (dino_sprite[row][col] == 1) {
                                    Send_Command(0x2A);  
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    
                                    Send_Command(0x2B);  
                                    Send_Data(0x00);
                                    Send_Data(old_dino_y + row);
                                    Send_Data(0x00);
                                    Send_Data(old_dino_y + row);
                                    
                                    Send_Command(0x2C);  
                                    Send_Data(0xFF);  
                                    Send_Data(0xFF);
                               
                                }
                            }
                        }

                    }
                    
                    // Draw dino at new position
                    if (dino_duck) {
                        // Draw duck sprite (10x15)
                        for (int row = 0; row < 10; row++) {
                            for (int col = 0; col < 15; col++) {
                                if (duck_dino_spirte[row][col] == 1) {
                                    Send_Command(0x2A);  
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    
                                    Send_Command(0x2B);  
                                    Send_Data(0x00);
                                    Send_Data(dino_pos_y + 5 + row);
                                    Send_Data(0x00);
                                    Send_Data(dino_pos_y + 5 + row);
                                    
                                    Send_Command(0x2C);  
                                    Send_Data(0x00);  
                                    Send_Data(0xE0);
                                }
                            }
                        }
                    } else {
                        // Draw standing sprite (15x12)
                        for (int row = 0; row < 15; row++) {
                            for (int col = 0; col < 12; col++) {
                                if (dino_sprite[row][col] == 1) {
                                    Send_Command(0x2A);  
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    Send_Data(0x00);
                                    Send_Data(10 + col);
                                    
                                    Send_Command(0x2B);  
                                    Send_Data(0x00);
                                    Send_Data(dino_pos_y + row);
                                    Send_Data(0x00);
                                    Send_Data(dino_pos_y + row);
                                    
                                    Send_Command(0x2C);  
                                    Send_Data(0x00);  
                                    Send_Data(0xE0);
                                }


                            }
                        }

                    }
                    
                    old_dino_y = dino_pos_y;
                    old_duck = dino_duck;
                }
                // Update obstacle 
                if (old_cactus_x != cactus_pos_x || old_obstacle != obstacle) {
                    // Erase old obstacle
                    if (old_obstacle == 0) {
                        // Erase cactus
                        for (int row = 0; row < 20; row++) {
                            for (int col = 0; col < 10; col++) {
                                if (cactus_sprite[row][col] == 1) {
                                    Send_Command(0x2A);
                                    Send_Data(0x00);
                                    Send_Data(old_cactus_x + col);
                                    Send_Data(0x00);
                                    Send_Data(old_cactus_x + col);
                                    
                                    Send_Command(0x2B);
                                    Send_Data(0x00);
                                    Send_Data(95 + row);
                                    Send_Data(0x00);
                                    Send_Data(95 + row);
                                    
                                    Send_Command(0x2C);
                                    Send_Data(0xFF);
                                    Send_Data(0xFF);
                                }
                            }


                        }

                    } 

                    else {
                        // Erase ptero
                        for (int row = 0; row < 10; row++) {
                            for (int col = 0; col < 12; col++) {
                                if (ptero_sprite[row][col] == 1) {
                                    Send_Command(0x2A);
                                    Send_Data(0x00);
                                    Send_Data(old_cactus_x + col);
                                    Send_Data(0x00);
                                    Send_Data(old_cactus_x + col);
                                    
                                    Send_Command(0x2B);
                                    Send_Data(0x00);
                                    Send_Data(ptero_pos_y + row);
                                    Send_Data(0x00);
                                    Send_Data(ptero_pos_y + row);
                                    
                                    Send_Command(0x2C);
                                    Send_Data(0xFF);
                                    Send_Data(0xFF);
                                }
                            }

                        }
                    }
                    
                    // Draw new obstacle
                    if (obstacle == 0) {
                        // Draw cactus
                        for (int row = 0; row < 20; row++) {
                            for (int col = 0; col < 10; col++) {
                                if (cactus_sprite[row][col] == 1) {
                                    Send_Command(0x2A);
                                    Send_Data(0x00);
                                    Send_Data(cactus_pos_x + col);
                                    Send_Data(0x00);
                                    Send_Data(cactus_pos_x + col);
                                    
                                    Send_Command(0x2B);
                                    Send_Data(0x00);
                                    Send_Data(95 + row);
                                    Send_Data(0x00);
                                    Send_Data(95 + row);
                                    
                                    Send_Command(0x2C);
                                    Send_Data(0x07);
                                    Send_Data(0x00);
                                }

                            }

                        }
                    }
                    
                    else {
                        // Draw ptero
                        for (int row = 0; row < 10; row++) {
                            for (int col = 0; col < 12; col++) {
                                if (ptero_sprite[row][col] == 1) {
                                    Send_Command(0x2A);
                                    Send_Data(0x00);
                                    Send_Data(cactus_pos_x + col);
                                    Send_Data(0x00);
                                    Send_Data(cactus_pos_x + col);
                                    
                                    Send_Command(0x2B);
                                    Send_Data(0x00);
                                    Send_Data(ptero_pos_y + row);
                                    Send_Data(0x00);
                                    Send_Data(ptero_pos_y + row);
                                    
                                    Send_Command(0x2C);
                                    Send_Data(0xF8);
                                    Send_Data(0x00);
                                }
                           
                            }
                        }

                    }
                    
                    old_cactus_x = cactus_pos_x;
                    old_obstacle = obstacle;
                }
            }

            break;
    }

    return state;
}
                
                
       
enum LCD_STATES {lcd_start, lcd_display, lcd_idle};

int lcd_tick(int state) {
    switch(state) {
        case lcd_start:
            state = lcd_display;
            break;
        case lcd_display:
            state = lcd_idle;
            break;
        case lcd_idle:
            state = lcd_idle;
            break;
        default:
        break;
    }

        
    
    switch(state) {
        case lcd_display:
            lcd_init();
            lcd_clear();

            score = load_score();

            lcd_goto_xy(0, 0);
            lcd_write_str("Dinosaur Game!");
            lcd_goto_xy(1, 0);
    
            char buffer[16];
            sprintf(buffer, "Score: %u", score);  
            lcd_write_str(buffer);
            break;
        case lcd_idle:
            break;
        
        
            default:
            break;
    }
    return state;
}

enum SCORE_STATES {score_start, score_update};

int score_tick(int state) {
    switch(state) {
        case score_start:
            state = score_update;
            break;
        case score_update:
            state = score_update;
            break;
        default:
            state = score_start;
            break;
    }
    
    switch(state) {
        case score_update:
           if(game_playing){
             score++;
            char buffer[16];
            sprintf(buffer, "Score: %u", score);
            lcd_goto_xy(1, 0);
            lcd_write_str(buffer); 
            

            if(score % 10 ==0) {
                save_score();
            }
        }
            break;
        default:
            break;
    }
    return state;
}


 enum BUTTON_STATES {button_start, button_start_reset};

int button_tick(int state) {
    unsigned char reset_button = (!(PINC & (1 << PC0)) == 0) ;
    static unsigned char prev_reset = 0;

   
    switch (state) {
        case button_start:
            if (reset_button && !prev_reset) {  
                state = button_start_reset;
            }
            break;

        case button_start_reset:
            if (!reset_button) {  
                state = button_start;
            }
            break;

        default:
            state = button_start;
            break;
    }

    
    switch (state) {
        case button_start_reset:
            if (!prev_reset) { 
                
                if (!game_playing) {
                score = 0;
                game_playing = 1; 
            
                lcd_goto_xy(0, 0);
                lcd_write_str("Dinosaur Game!             ");
                lcd_goto_xy(1, 0);
                lcd_write_str("Score: 0      ");}
                else{
                save_score();
                game_playing = 0;
                score = 0;
                cactus_pos_x = 100;

                lcd_goto_xy(1,0);
                lcd_write_str("             ");
                
                char buffer[16];
                sprintf(buffer, "Score: %u", score);
                lcd_goto_xy(1, 0);
                lcd_write_str(buffer);
            }
        }
            break;

        default:
            break;
    }

    prev_reset = reset_button;
    return state;

}

enum GAME_STATES {game_idle, game_running, game_end};

int game_tick(int state) {

    unsigned char jump = (!(PINC & (1 << PC1)) == 0);
    unsigned char duck = (!(PINC & (1 << PC2))==0);
    static unsigned char prev_jump = 0;

    switch(state) { 
        case game_idle:
        if (game_playing) {
            state = game_running;
        }
        break;

        case game_running:

        if (obstacle == 0 && cactus_pos_x < 22 && cactus_pos_x + 10 > 10) {
            if (dino_pos_y + 15 > 95) {
                state = game_end;
            }
         }
         else if (obstacle == 1 && cactus_pos_x < 22 && cactus_pos_x + 12 > 10){
            if ( cactus_pos_x < 22 && cactus_pos_x + 12 > 10) {
                if (!dino_duck ) {
                    state = game_end;
                }
            }
         }
        break;

        case game_end:
        if(!game_playing) {
            state = game_idle;
        }
        break;
        
    }
    switch(state){
        case game_idle:

        dino_pos_y = 100;
        dino_jump = 0;
        cactus_pos_x = 100;
        dino_duck = 0;
        obstacle = 0;


        break;

        case game_running:

        if ( duck && !dino_jump && game_playing){
            dino_duck = 1;
        }
        else {
            dino_duck = 0;
        }
        if ( jump && !prev_jump && !dino_jump && game_playing ) {
            dino_jump = 1;
            jump_speed = -12;
        }

        if (dino_jump) {
            dino_pos_y += jump_speed;
            jump_speed += 1;

            if (dino_pos_y >= 100) {
                dino_pos_y = 100;
                dino_jump = 0;
                jump_speed = 0;
            }
        }

        if (game_playing) {
            cactus_pos_x -= cactus_speed;

            if(cactus_pos_x < -8) {
                cactus_pos_x = 128;
                obstacle = (obstacle + 1) % 2;
            }
        }
        break;

        case game_end:
        game_playing = 0;
        save_score();

        lcd_goto_xy(0,0);
        lcd_write_str("   GAME OVER!      ");
        lcd_goto_xy(1,0);
        char buffer[16];
        sprintf(buffer, "Score: %u", score);
        lcd_write_str(buffer);
        break;
    }

    prev_jump = jump;
    return state;
}



int main(void) {
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRD = 0xFF;
    PORTD = 0x00;

    DDRC = 0x00;
    PORTC = 0xFF;

    PORTB |= (1 << PB4);  
    _delay_ms(10);
    SPI_INIT();


    ST7735_init();


    
    tasks[0].period = TASK1_PERIOD;
    tasks[0].state = tft_start;
    tasks[0].elapsedTime = 0;
    tasks[0].TickFct = &tft_tick;
    
    tasks[1].period = TASK2_PERIOD;
    tasks[1].state = lcd_start;
    tasks[1].elapsedTime = 0;
    tasks[1].TickFct = &lcd_tick;

    tasks[2].period = TASK3_PERIOD;
    tasks[2].state = score_start;
    tasks[2].elapsedTime = 0;
    tasks[2].TickFct = &score_tick;

    tasks[3].period = TASK4_PERIOD;
    tasks[3].state = button_start;
    tasks[3].elapsedTime = 0;
    tasks[3].TickFct = &button_tick;

    tasks[4].period = TASK5_PERIOD;
    tasks[4].state = game_idle;
    tasks[4].elapsedTime = 0;
    tasks[4].TickFct = &game_tick;
    
    TimerSet(GCD_PERIOD);
    TimerOn();
    
    while(1) {}
    
    return 0;
}