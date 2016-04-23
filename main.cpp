// uLCD-144-G2 demo program for uLCD-4GL LCD driver library
//
#include "mbed.h"
#include "uLCD_4DGL.h"
#include "rtos.h"
#include "SDFileSystem.h"
#include <string>
#include <vector>
#include "joystick.h"

Mutex mutex;
uLCD_4DGL lcd1(p9,p10,p11); // serial tx, serial rx, reset pin;
uLCD_4DGL lcd2(p28,p27,p29);
SDFileSystem sd(p5, p6, p7, p8, "sd");
//InterruptIn pb1(p22);
//InterruptIn pb2(p21);
volatile bool up1 = false;
volatile bool down1 = false;
volatile bool left1 = false;
volatile bool right1 = false;
volatile bool fire1 = false;
volatile bool up2 = false;
volatile bool down2 = false;
volatile bool left2 = false;
volatile bool right2 = false;
volatile bool fire2 = false;
volatile int song1 = 0;
volatile int song2 = 0;
Nav_Switch joy1( p18, p15, p16, p14, p17, &up1, &down1, &left1, &right1, &fire1, &song1); //up, down, left, right, center
Nav_Switch joy2( p21, p24, p23, p25, p22, &up2, &down2, &left2, &right2, &fire2, &song2);

volatile bool pushed1 = false;
volatile bool pushed2 = false;
volatile bool selected1 = false;
volatile bool selected2 = false;

volatile bool process1 = false;
volatile bool process2 = false;

vector<string> filenames;

void read_file_names(char *dir)
{
    DIR *dp;
    struct dirent *dirp;
    dp = opendir(dir);
    //read all directory and file names in current directory into filename vector
    while((dirp = readdir(dp)) != NULL) {
        string filename = string(dirp->d_name);
        if ((filename[0] >= 97 && filename[0] <=122) || (filename[0] >= 65 && filename[0] <=90)) {
            filenames.push_back(filename);
        }
    }
    closedir(dp);
}

/**
void pb1_hit_interrupt (void) {
    pushed1 = true;
    selected1 = true;
}

void pb2_hit_interrupt (void) {
    pushed2 = true;
    selected2 = true;
}
**/

void lcd1_thread(void const *args) {
    
    // 1. title screen
    mutex.lock();
    //lcd1.cls();
    //lcd1.reset();
    lcd1.textbackground_color(BLACK);
    lcd1.color(RED);
    lcd1.locate(4,0);  
    lcd1.printf("Dance Dance");
    lcd1.locate(4,1);
    lcd1.printf("Revolution!");
    lcd1.locate(6,15);
    lcd1.printf("Player 1");
    mutex.unlock();
    while(!pushed1) {
        mutex.lock();
        lcd1.locate(4,7);
        lcd1.printf("Press CENTER");
        mutex.unlock();
        Thread::wait(1000);
        mutex.lock();
        lcd1.locate(4,7);
        lcd1.printf("            ");
        mutex.unlock(); 
        pushed1 = fire1;   
    }

    // 2. ready screen
    mutex.lock();
    lcd1.cls();
    lcd1.locate(0,7);
    lcd1.printf("Player 1 is ready!");
    mutex.unlock();
    Thread::wait(1000);
    while(!pushed2) {
        mutex.lock();
        lcd1.locate(0,9); 
        lcd1.printf("Wait player 2...");
        mutex.unlock();
    }

    // 3. song selection screen
    while(!process1) {
        Thread::yield();
    }
    mutex.lock();
    lcd1.cls();
    lcd1.printf("Select a song");
    mutex.unlock();
    fire1 = false;
    while(!selected1 || !selected2) {
        for (int i = 0; i < filenames.size(); i++) {
            mutex.lock();
            if (i == song1) {
                lcd1.color(RED);
            } else {
                lcd1.color(GREEN);
            }
            lcd1.locate(0,i+2);
            lcd1.printf("%s\n\r", filenames[i].c_str());
            lcd1.locate(16,i+2);
            lcd1.printf("  ");
            if (i == song2) {
                lcd1.color(BLUE);
                lcd1.locate(16,i+2);
                lcd1.printf("<-");
            }
            mutex.unlock();
        }
        if (selected1) {
            mutex.lock();
            lcd1.locate(0,8);
            lcd1.color(RED);
            lcd1.printf("Ready!");
            mutex.unlock();
        }
        selected1 = fire1;
    }


    // 5. count down
    mutex.lock();
    lcd1.cls();
    lcd1.color(RED);
    lcd1.printf("%s\n\r", filenames[song1].c_str());
    lcd1.locate(0,1);
    lcd1.printf("is picked");
    mutex.unlock();
    while(!process2) {
        Thread::yield();
    }
    mutex.lock();
    lcd1.cls();
    lcd1.text_width(4); //4X size text
    lcd1.text_height(4);
    mutex.unlock();
    lcd1.color(RED);
    for (int i=5; i>=0; --i) {
        mutex.lock();
        lcd1.locate(1,2);
        lcd1.printf("%2D",i);
        mutex.unlock();
        Thread::wait(1000);
    }

}

void lcd2_thread(void const *args) {
    
    // 1. title screen
    mutex.lock();
    //lcd2.cls();
    //lcd2.reset();
    lcd2.textbackground_color(BLACK);
    lcd2.color(BLUE);
    lcd2.locate(4,0);  
    lcd2.printf("Dance Dance");
    lcd2.locate(4,1);
    lcd2.printf("Revolution!");
    lcd2.locate(6,15);
    lcd2.printf("Player 2");
    mutex.unlock();
    while(!pushed2) {
        mutex.lock();
        lcd2.locate(4,7);
        lcd2.printf("Press CENTER");
        mutex.unlock();
        wait(1);
        mutex.lock();
        lcd2.locate(4,7);
        lcd2.printf("            ");
        mutex.unlock();   
        pushed2 = fire2; 
    }

    // 2. ready screen
    mutex.lock();
    lcd2.cls();
    lcd2.locate(0,7);
    lcd2.printf("Player 2 is ready!");
    mutex.unlock();
    Thread::wait(1000);
    while(!pushed1){
        mutex.lock();
        lcd2.locate(0,9); 
        lcd2.printf("Wait player 1...");
        mutex.unlock();
    }

    // 3. song selection screen
    Thread::wait(1000);
    process1 = true;
    mutex.lock();
    lcd2.cls();
    lcd2.printf("Select a song");
    mutex.unlock();
    fire2 = false;
    while(!selected2 || !selected1) {
        for (int i = 0; i < filenames.size(); i++) {
            mutex.lock();
            if (i == song2) {
                lcd2.color(BLUE);
            } else {
                lcd2.color(GREEN);
            }
            lcd2.locate(0,i+2);
            lcd2.printf("%s\n\r", filenames[i].c_str());
            lcd2.locate(16,i+2);
            lcd2.printf("  ");
            // add a marker to notify player 2 about player 1's selection
            if (i == song1) {
                lcd2.color(RED);
                lcd2.locate(16,i+2);
                lcd2.printf("<-");
            }
            mutex.unlock();
        }
        if (selected2) {
            mutex.lock();
            lcd2.locate(0,8);
            lcd2.color(BLUE);
            lcd2.printf("Ready!");
            mutex.unlock();
        }
        selected2 = fire2;
    }

    // 5. count down
    mutex.lock();
    lcd2.cls();
    lcd2.color(BLUE);
    lcd2.printf("%s\n\r", filenames[song1].c_str());
    lcd2.locate(0,1);
    lcd2.printf("is picked");
    mutex.unlock();
    Thread::wait(3000);
    process2 = true;
    mutex.lock();
    lcd2.cls();
    lcd2.text_width(4); //4X size text
    lcd2.text_height(4);
    mutex.unlock();
    lcd2.color(BLUE);
    for (int i=5; i>=0; --i) {
        mutex.lock();
        lcd2.locate(1,2);
        lcd2.printf("%2D",i);
        mutex.unlock();
        Thread::wait(1000);
    }

}

int main()
{
    //pb1.mode(PullUp);
    //pb2.mode(PullUp);
    //wait(.01);
    //pb1.fall(&pb1_hit_interrupt);
    //pb2.fall(&pb2_hit_interrupt);
    lcd1.baudrate(3000000);
    lcd2.baudrate(3000000);
    read_file_names("/sd/music");
    Thread thread1(lcd1_thread);
    Thread thread2(lcd2_thread);
    while(true)
    {
    }
}