#include "db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../third_party/cjson/cJSON.h"

#define DB_FILE "db.json"  // Nombre del archivo que actúa como "base de datos"

int checkUID(const char *uid, char *name) {
    FILE *file = fopen(DB_FILE, "r");
    if (!file) {
        perror("Error al abrir la base de datos");
        return 0;  // Si no se puede abrir el archivo, se deniega el acceso por seguridad
    }
    
    // Determinar el tamaño del archivo
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error al buscar fin de archivo");
        fclose(file);
        return 0;
    }
    long filesize = ftell(file);
    if (filesize < 0) {
        perror("Error al obtener tamaño de archivo");
        fclose(file);
        return 0;
    }
    rewind(file);
    
    // Reservar memoria para leer todo el contenido
    char *data = malloc(filesize + 1);
    if (!data) {
        perror("Error en malloc");
        fclose(file);
        return 0;
    }
    
    // Leer el contenido y verificar cantidad de bytes leídos
    size_t bytes_read = fread(data, 1, filesize, file);
    if (bytes_read != (size_t)filesize) {
        perror("Error al leer la base de datos");
        free(data);
        fclose(file);
        return 0;
    }
    data[filesize] = '\0';
    fclose(file);
    
    // Parsear el JSON
    cJSON *json = cJSON_Parse(data);
    free(data);
    if (!json) {
        fprintf(stderr, "Error al parsear JSON en %s\n", DB_FILE);
        return 0;
    }
    
    // Obtener el array de usuarios
    cJSON *users = cJSON_GetObjectItem(json, "users");
    if (!users) {
        fprintf(stderr, "Error: no se encontró la clave 'users' en %s\n", DB_FILE);
        cJSON_Delete(json);
        return 0;
    }
    
    int encontrado = 0;
    cJSON *user = NULL;
    cJSON_ArrayForEach(user, users) {
        cJSON *json_uid = cJSON_GetObjectItem(user, "uid");
        cJSON *json_name = cJSON_GetObjectItem(user, "name");
        if (json_uid && json_name && strcmp(json_uid->valuestring, uid) == 0) {
            // Copiar el nombre encontrado en el buffer
            strncpy(name, json_name->valuestring, NAME_SIZE - 1);
            name[NAME_SIZE - 1] = '\0';
            encontrado = 1;
            break;
        }
    }
    
    cJSON_Delete(json);
    return encontrado;
}
