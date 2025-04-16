#include <stdio.h>
#include "mfrc522/mfrc522.h"

int main(int argc, char * argv[]) {

    char UID[16];
    char TagType;  
    
    printf("Hi from RFID test\n");

    MFRC522_Init("/dev/gpiochip2", 8, "/dev/spidev1.0");

    for(;;) {
        if (MFRC522_isCard (&TagType)) {      
            printf("Tag Type: %X\n", TagType);
           //Read ID
           if (MFRC522_ReadCardSerial ((char *)&UID[0])) {
                printf("UID: %X%X%X%X%X\n", UID[0],UID[1],UID[2],UID[3],UID[4]);
           }
           MFRC522_Halt();
        }

    }


    return 0;
}