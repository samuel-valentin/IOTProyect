#include <stdio.h>
#include "mfrc522/mfrc522.h"    //Librerias para lector
#include "database/db.h"        //Librerias para manejar la Base de Datos

void uid_to_string(const char *uid, char *str) {
    sprintf(str, "%02X%02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3], uid[4]);
}

int main(int argc, char * argv[]) {

    char UID[16];
    char TagType;  
    char uid_str[11]; // 10 caracteres para 5 bytes en formato hex + 1 para el '\0'
    char name[NAME_SIZE];

    printf("Ready for read!\n");

    MFRC522_Init("/dev/gpiochip2", 8, "/dev/spidev1.0");

    for (;;) {
        if (MFRC522_isCard(&TagType)) {      
            printf("Tag Type: %X\n", TagType);
            // Lee el UID
            if (MFRC522_ReadCardSerial(UID)) {
                uid_to_string(UID, uid_str);
                printf("UID: %s\n", uid_str);
                if (checkUID(uid_str, name)) {
                    printf("Acceso autorizado: Bienvenido %s\n", name);
                } else {
                    printf("Acceso denegado\n");
                }
            }
            MFRC522_Halt();
        }
    }

    return 0;
}