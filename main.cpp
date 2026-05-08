#include "simpleplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    SimplePlayer window;
    window.show();

    return app.exec();
}
