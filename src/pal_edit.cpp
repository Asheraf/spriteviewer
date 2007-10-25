#include "pal_edit.h"

//Constructor de la clase Pal_edit, donde seteamos los valores por defecto del dialogo
Pal_edit::Pal_edit( QWidget * parent, Qt::WFlags f,
    SpritePalette *palette, unsigned int palette_size)
	    : QDialog(parent, f)
{
    QToolButton *toolButton;

    //inicializamos la UI
    setupUi(this);
    cancel_btn->setVisible(false); //<-----------REVISARRRR!!!!!!!!!!!!!!!

    //iteramos atravez de los elementos de la paleta
    for (int i=0;i<256;i++)
    {
        //inicializamos el boton y le damos valores por defecto
        toolButton = new QToolButton(frame_pal);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        //lo posicionamos
        toolButton->setGeometry(QRect(((i%16)*17)+5, ((i/16)*17)+5, 15, 15));
        //le damos un cursor diferente
        toolButton->setCursor(QCursor(Qt::PointingHandCursor));

        //si le pasamos alguna paleta
        if (palette!=NULL)
        {
            //seteamos el estilo
            toolButton->setStyleSheet("QToolButton {background-color: rgb(" +
                QString::number(palette[i].r) + "," + 
                QString::number(palette[i].g) + "," + 
                QString::number(palette[i].b) + ") ; border:0}");
            //y el tooltip 
            toolButton->setToolTip("(" +
                    QString::number(palette[i].r) + "," + 
                    QString::number(palette[i].g) + "," + 
                    QString::number(palette[i].b) + ")");
            //y lo guardamos en una variable de la clase
            pal_nueva[i].r=palette[i].r;
            pal_nueva[i].g=palette[i].g;
            pal_nueva[i].b=palette[i].b;
            pal_nueva[i].unused=0;
        } else {
            //si no se paso ninguna paleta lo inicializa a Negro todos los botones
            toolButton->setStyleSheet("QToolButton {background-color: black ; border:0}");
            toolButton->setToolTip(tr("(no inicializado)"));

            pal_nueva[i].r=0;
            pal_nueva[i].g=0;
            pal_nueva[i].b=0;
            pal_nueva[i].unused=0;
        }
        toolButton->setText(QString());

        button_grp.addButton(toolButton, i);
    }

    //conectamos las señales del dialogo
    connect(&button_grp, SIGNAL(buttonClicked(int)), this, SLOT(button_click(int)));
    connect(guardar_btn, SIGNAL(clicked()), this, SLOT(guardar_palette()));
    connect(cancel_btn, SIGNAL(clicked()), this, SLOT(cancelar_ed_btn()));
}

//Destructor sin nada que destruir =)
Pal_edit::~Pal_edit()
{
    
}

//Slot que recibe la señal de guardar paleta
void Pal_edit::guardar_palette()
{
    static QDir last_dir;

    //muestra el dialogo Guardar Como...
    QString archivo = QFileDialog::getSaveFileName(
                                    this, 
                                    tr("Salvar Paleta"), 
                                    last_dir.absolutePath(), 
                                    tr("Archivos de paletas (*.pal)"));

    //Si no hiciste click en cancelar
    if (!archivo.isEmpty())
    {
        FILE *fh;

        //Guarda el ultimo dir visitado
        last_dir=QDir(archivo);

        //Abre el archivo en modo binario, escribe en el la paleta y lo cierra
        fh=fopen(archivo.toAscii(), "wb");
        fwrite(get_pal(pal_nueva, 1024), sizeof(SpritePalette), 256, fh);
        fclose(fh);
    }
}

//Slot para recibir el click en algun cuadrado para cambiar el color
void Pal_edit::button_click(int id)
{
    QColor color;
    static QColor last_color;

    //se fija cual de los radio boxes esta seleccionado y en base a eso elije el color inicial
    if (ult_btn->isChecked()) {
        color = QColorDialog::getColor(last_color);
    } else if (act_btn->isChecked()) {
        color = QColorDialog::getColor(QColor::QColor(pal_nueva[id].r,
                                                      pal_nueva[id].g,
                                                      pal_nueva[id].b));
    } else {
        color = QColorDialog::getColor(QColor::QColor(255,255,255));
    }

    //si no se hiso click en el boton cancelar
    if (color.isValid())
    {
        //Guarda el ultimo color
        last_color=color;

        //setea el nuevo color del boton
        button_grp.button(id)->setStyleSheet("QToolButton {background-color: rgb(" + 
            QString::number(color.red()) + "," + 
            QString::number(color.green()) + "," + 
            QString::number(color.blue()) + ") ; border:0}");

        //setea el nuevo tooltip
        button_grp.button(id)->setToolTip("(" +
            QString::number(color.red()) + "," + 
            QString::number(color.green()) + "," + 
            QString::number(color.blue()) + ")");

        //Guarda la el color dentro de la paleta de la clase
        pal_nueva[id].r=color.red();
        pal_nueva[id].g=color.green();
        pal_nueva[id].b=color.blue();
        pal_nueva[id].unused=0;

        //emite la señal para cambiar el color en el sprite
        emit color_changed(id, color.red(), color.green(), color.blue());
    }

}

//Slot para el boton cancelar [No Implementado]
void Pal_edit::cancelar_ed_btn()
{
    emit cancelar_ed();
}
