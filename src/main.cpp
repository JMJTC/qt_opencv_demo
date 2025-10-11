#include "videowidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    VideoWidget widget;
    widget.setWindowTitle("🎥 Qt + OpenCV 跨平台实时预览");
    widget.show();

    return app.exec();
}
