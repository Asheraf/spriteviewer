#include <QApplication>
#include "spr_viewer.h"

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	Spr_viewer win;

    if (argc>=2)
        if (QFile::exists(QString::QString(argv[1])))
            win.abrir_archivo(QString::QString(argv[1]));

	win.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
