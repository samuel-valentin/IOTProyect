#include "alsa_play.h"
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <unistd.h>             // sleep() :contentReference[oaicite:2]{index=2}
#include <alsa/asoundlib.h>

#define CHANNEL   2
#define RATE     48000
#define FRAMES    768

int play_audio(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen audio");
        return -1;
    }

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hwparams;
    int ret = snd_pcm_open(&handle, "plughw:1", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        fprintf(stderr, "snd_pcm_open error: %s\n", snd_strerror(ret));
        fclose(file);
        return ret;
    }

// Reserva e inicializa espacio de configuracion
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_hw_params_any(handle, hwparams);

    // Configurar parametros
    ret = snd_pcm_hw_params_set_channels(handle, hwparams, CHANNEL);

    // Formato
    ret = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S32_LE);

    // Tipo de acceso interleaved
    ret = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

    // Configurar tasa de muestreo
    unsigned int rate = RATE;
    ret = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rate, 0);

    // Aplicar parametros
    ret = snd_pcm_hw_params(handle, hwparams);

    // Tamaño del buffer en bytes
    size_t sample_size = sizeof(int32_t);
    size_t frame_bytes = CHANNEL * sample_size;
    size_t bufsize = FRAMES * frame_bytes;
    char *buffer = malloc(bufsize);
    if (!buffer) {
        perror("malloc audio buffer");
        snd_pcm_close(handle);
        fclose(file);
        return 1;
    }

    // Reproducir hasta EOF
    size_t bytes;
    while ((bytes = fread(buffer, 1, bufsize, file)) > 0) {
        snd_pcm_uframes_t frames = bytes / frame_bytes;
        snd_pcm_sframes_t written = snd_pcm_writei(handle, buffer, frames);
        if (written < 0) {
            written = snd_pcm_recover(handle, written, 0);  // EAGAIN, XRUN… :contentReference[oaicite:3]{index=3}
        }
        if (written < 0) {
            fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror(written));
            break;
        }
    }

    // Esperar a que suene todo antes de cerrar :contentReference[oaicite:4]{index=4}
    snd_pcm_drain(handle);

    snd_pcm_close(handle);
    free(buffer);
    fclose(file);
    return 0;
}
