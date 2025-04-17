#ifndef DB_H
#define DB_H
#define NAME_SIZE 64

// Retorna 1 (acceso autorizado) si el UID se encuentra en la base de datos, 0 en caso contrario.
int checkUID(const char *uid, char *name);

#endif // DB_H
