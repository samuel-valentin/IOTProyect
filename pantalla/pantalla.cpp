#include "pantalla.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using std::string;

#define WIDTH 1280
#define HEIGHT 720

int show_screen(const char *color, const char *text) {
    // 1) Elegir color de fondo
    Scalar bg;
    if (strcmp(color, "green") == 0) {
        bg = Scalar(0,255,0);
    } else if (strcmp(color, "red") == 0) {
        bg = Scalar(0,0,255);
    } else {
        return -1;  // color inválido
    }

    // 2) Crear imagen sólida
    Mat img(HEIGHT, WIDTH, CV_8UC3, bg);

    // 3) Configurar texto
    string s(text);
    int font = FONT_HERSHEY_SIMPLEX;
    double scale = 1.5;
    int thickness = 3;

    // 4) Calcular posición para centrar
    Size sz = getTextSize(s, font, scale, thickness, 0);
    Point org((WIDTH - sz.width)/2, (HEIGHT + sz.height)/2);

    // 5) Dibujar texto
    putText(img, s, org, font, scale, Scalar(255,255,255), thickness);

    // 6) Mostrar y refrescar la ventana
    imshow("Pantalla", img);
    waitKey(1);  // procesa eventos GUI y refresca :contentReference[oaicite:2]{index=2}

    return 0;
}
