#define _KEYBOARD_C
#include "config.h"
#include "keyboard.h"
#include "main.h"


unsigned char code KeyCodeMap[4][4] = { 
    { 0x31,  0x32,  0x33, 0x26 }, 
    { 0x34,  0x35,  0x36, 0x25 }, 
    { 0x37,  0x38,  0x39, 0x28 }, 
    { 0x30,  0x1B,  0x0D, 0x27 }  
};
unsigned char pdata KeySta[4][4] = {  
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

void KeyDriver()
{
    unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    
    for (i=0; i<4; i++)  
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])  
            {
                if (backup[i][j] != 0)           
                {
                   KeyAction(KeyCodeMap[i][j]) ; 
                }
                backup[i][j] = KeySta[i][j];    
            }
        }
    }
}

void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   
    static unsigned char keybuf[4][4] = {  
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    for (i=0; i<4; i++)  
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   
            KeySta[keyout][i] = 1;
        }
    }
    keyout++;         
    keyout &= 0x03;   
    switch (keyout)   
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
