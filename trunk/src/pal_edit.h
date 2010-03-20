#ifndef __PAL_EDIT_H__
#define __PAL_EDIT_H__

#include "ui_pal_edit.h"
#include "sprite.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QToolButton>
#include <QMouseEvent>
#include <QFrame>
#include <QTranslator>

class Pal_edit : public QDialog, public Ui::pal_edit_dlg
{
Q_OBJECT
    QButtonGroup button_grp;
    SpritePalette pal_nueva[256];

public:
    QFrame *frame;

    //Constructor de la clase Pal_edit, donde seteamos los valores por defecto del dialogo
	Pal_edit(QWidget *parent=0, Qt::WFlags f=0, SpritePalette *palette=NULL, unsigned int palette_size=0);

    //Destructor sin nada que destruir =)
    ~Pal_edit();

private slots:

    //Slot para recibir el click en algun cuadrado para cambiar el color
    void button_click(int id);

    //Slot que recibe la señal de guardar paleta
    void guardar_palette();

    //Slot para el boton cancelar [No Implementado]
    void cancelar_ed_btn();

    //Slot que recibe el pedido de cambio de idioma
    void i18n_translate(QTranslator *trans);

signals:
    //señal que se emite cada vez que se cambia un color del dialogo
    void color_changed(int id, unsigned int r, unsigned int g, unsigned int b);

    //señal que se emite al presionar sobre el boton cancelar
    void cancelar_ed();
};

#endif // __PAL_EDIT_H__
