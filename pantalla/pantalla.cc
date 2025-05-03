#include "pantalla.h"
#include <opencv2/opencv.hpp>
#include <cstring> // para strcmp

void mostrar_pantalla(const char* color, const char* texto) {
    const int WIDTH = 1280;
    const int HEIGHT = 720;

    // Color de fondo
    cv::Scalar bgColor;
    if (strcmp(color, "green") == 0) {
        bgColor = cv::Scalar(0, 255, 0);
    } else {
        bgColor = cv::Scalar(0, 0, 255);
    }

    // Crear imagen
    cv::Mat img(HEIGHT, WIDTH, CV_8UC3, bgColor);

    // Texto centrado
    int font = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 2.0;
    int thickness = 4;
    cv::Size sz = cv::getTextSize(texto, font, scale, thickness, 0);
    cv::Point org((WIDTH - sz.width) / 2, (HEIGHT + sz.height) / 2);

    // Dibujar texto blanco
    cv::putText(img, texto, org, font, scale, cv::Scalar(255,255,255), thickness);

    // Mostrar
    cv::imshow("Pantalla de acceso", img);
    cv::waitKey(2000); // Mostrar por 2 segundos
    cv::destroyWindow("Pantalla de acceso");
}
