#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H

#include "ui_spr_viewer.h"
#include "sprite.h"
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

class Spr_viewer : public QMainWindow, public Ui::main_window
{
Q_OBJECT
    QString archivo;
    Sprite *spr;
    QTimer timer;
    int timeout;
    bool transparente;

public:
    //Constructor de la clase Spr_viewer, donde seteamos los valores por defecto de la app
	Spr_viewer( QWidget * parent = 0, Qt::WFlags f = 0 );

    //Destructor de la clase. libera el espacio si fue cargado un sprite
    ~Spr_viewer();

    //Funcion que abre un archivo .spr pasado como argumento
    void abrir_archivo(QString file);

private:
    //Actualiza los controles en base a el sprite cargado
    void cargar_sprites();

/*
signals:
    void valueChanged(int newValue);
*/

private slots:
    //Funcion sobrecargada para actuar como SLOT del action_abrir
    void abrir_archivo();

    //abre un dialogo para cambiar la paleta de colores
    void abrir_palete();

    //funcion para cambiar la imagen dentro del label
    void cambiar_imagen(int numero);

    //slot destinado a decidir cuando el sprite tiene que animarse
    void play();

    //funcion llamada por timer para que avance en uno la imagen en el sprite
    void adv_frame();

    //funcion para cambiar la velocidad de actualizacion de las imagenes
    void set_timeout();

    //funcion para exportar las imagenes del sprite a una carpeta
    void spr_to_bmp();

    //Muestra el acerca de embebido de QT
    void about_qt();

    //Muestra el Acerca de de la app [REVISAR]
    void about();

    //Si el action ta checked (actualiza la variable de la clase) [REVISAR]
    void trig_transp(bool check);
};
#endif
