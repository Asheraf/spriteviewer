#include <QApplication>
#include <QTextCodec>
#include "spr_viewer.h"

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	Spr_viewer win;

    //Esto sirve para poder escribir directamente en utf8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    if (argc>=2)
    {
        QString archivo=QString::QString().fromLocal8Bit(argv[1]);
        if (QFile::exists(archivo))
            win.abrir_archivo(archivo.toLocal8Bit());
    }

	win.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
