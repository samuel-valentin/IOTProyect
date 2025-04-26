#ifndef ALSA_PLAY_H
#define ALSA_PLAY_H

// Devuelve 0 si pudo reproducir el archivo completo, 
// valor negativo en caso de error.
int play_audio(const char *filename);

#endif // ALSA_PLAY_H
