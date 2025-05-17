#ifndef PANTALLA_H
#define PANTALLA_H

#ifdef __cplusplus
extern "C" {
#endif

// Muestra una ventana de color "green" o "red"
// con el texto indicado centrado.
// Devuelve 0 si todo OK, <0 en error.
int show_screen(const char *color, const char *text);

#ifdef __cplusplus
}
#endif

#endif // PANTALLA_H
