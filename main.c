#include <stdio.h>
#include "mfrc522/mfrc522.h"    //Librerias para lector
#include "database/db.h"        //Librerias para manejar la Base de Datos
#include "alsa/alsa_play.h"
#include <unistd.h>
#include "pantalla/pantalla.h"


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
            char message[128];
            // Lee el UID
            
            if (MFRC522_ReadCardSerial(UID)) {
                uid_to_string(UID, uid_str);
                printf("UID: %s\n", uid_str);
                if (checkUID(uid_str, name)) {
                    printf("Acceso autorizado: Bienvenido %s\n", name);
                    // Construimos el saludo completo
                    snprintf(message, sizeof(message), "Acceso autorizado: Bienvenido %s", name);
                    show_screen("green", message);
                    play_audio("success.raw");
                } else {
                    printf("Acceso denegado\n");
                    // Mensaje de acceso denegado
                    snprintf(message, sizeof(message), "Acceso denegado");
                    show_screen("red", message);
                    play_audio("failure.raw");
                }
            }
            // Pausa breve para evitar retriggers inmediatos
            sleep(3);
            MFRC522_Halt();
        }
    }

    return 0;
}