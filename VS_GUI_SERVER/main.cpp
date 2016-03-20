#include <QApplication>

#include "siform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    Q3Canvas canvas(350,25);
//    canvas.setAdvancePeriod(250);
//    ServerNForm serverF( c_port_UI, canvas );
    ServerNForm serverF( c_port_UI);
//    app.setMainWidget( &serverF );
    /////////////////////////////
    // for entering 'sudo password', the GUI is shown after first connection.
//    serverF.show();
    return app.exec();
}
