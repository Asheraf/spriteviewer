#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H

#include "ui_spr_viewer.h"
#include "pal_edit.h"
#include "sprite.h"
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

class Spr_viewer : public QMainWindow, public Ui::main_window
{
Q_OBJECT
    bool MOSTRAR_VEL;
    bool PLAY;
    QActionGroup *opcGroup;
    QString archivo;
    Sprite *spr;
    QTimer timer;
    int timeout;
    bool transparente;
    Pal_edit *ventana;

public:
    //Constructor de la clase Spr_viewer, donde seteamos los valores por defecto de la app
	Spr_viewer( QWidget * parent = 0, Qt::WFlags f = 0 );

    //Destructor de la clase. libera el espacio si fue cargado un sprite
    ~Spr_viewer();

private:
    //Actualiza los controles en base a el sprite cargado
    void cargar_sprites();

public slots:

    //muestra el dialogo para editar la paleta
    void ventana_pal_ed();

    //Funcion que abre un archivo .spr pasado como argumento
    void abrir_archivo(QString file=QString());

    //abre un dialogo para cambiar la paleta de colores
    void abrir_palete();

    //funcion para cambiar la imagen dentro del label
    void cambiar_imagen(int numero = -1);

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

    //Si el action ta checked (actualiza la variable de la clase)
    void trig_transp(bool check);

    //Slot que recibe la señal para cambiar un color de la paleta
    void cambio_color_paleta(int id, unsigned int r, unsigned int g, unsigned int b);

    //Slot que recibe la señal del boton cancelar.
    void pal_ed_cancelar();//SpritePalette palette);

    //se fija si tiene que mostrar la velocidad en ms/frame o en FPS
    void mostrar_vel_anim(QAction *action);
};
#endif
