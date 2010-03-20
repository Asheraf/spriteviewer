#include "spr_viewer.h"

//Constructor de la clase Spr_viewer, donde seteamos los valores por defecto de la app
Spr_viewer::Spr_viewer( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
    switch(QLocale::system().language()) {
    case QLocale::Spanish:
        trans.load(QString::fromUtf8(":/i18n/spritev_es"));
        break;
    default:
        trans.load(QString::fromUtf8(":/i18n/spritev_en"));
        break;
    }
    QApplication::installTranslator(&trans);
    emit lang_change(&trans);

    //inicializamos la UI
	setupUi(this);

    //seteamos algunas variables a un valor por defecto
    spr=0;
    PLAY=false;

    timeout=80;
    //Por defecto muestra la vel de la animacion en ms/frame
    timeout_label->setText(QString::number(timeout) + tr(" ms / frame"));
    MOSTRAR_VEL=true;

    //Setea el grupo para la internacionalizaci�n (i18n)
    i18nGroup = new QActionGroup(this);
    i18nGroup->addAction(action_English);
    i18nGroup->addAction(action_Espa_ol);

    //Setea el grupo para las opciones (fps o tpf)
    opcGroup = new QActionGroup(this);
    opcGroup->addAction(action_fps);
    opcGroup->addAction(action_tpf);

    //por defecto le fondo del sprite lo hacemos transparente
    transparente=true;

    //conectamos las se�ales
    //NOTA: la coneccion entre la action_salir con el slot close() 
    //y el slide con el LCD lo hago desde el Designer
    connect(action_abrir, SIGNAL(triggered()), this, SLOT(abrir_archivo()));
    connect(action_abrir_palete, SIGNAL(triggered()), this, SLOT(abrir_palete()));
    connect(nimages,SIGNAL(valueChanged(int)),this,SLOT(cambiar_imagen(int)));
    connect(play_button, SIGNAL(clicked()), this, SLOT(play()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(adv_frame()));
    connect(action_set_timeout, SIGNAL(triggered()), this, SLOT(set_timeout()));
    connect(action_spr_bmp, SIGNAL(triggered()), this, SLOT(spr_to_bmp()));
    connect(action_about_qt,SIGNAL(triggered()),this,SLOT(about_qt()));
    connect(action_about,SIGNAL(triggered()),this,SLOT(about()));
    connect(action_transparente,SIGNAL(toggled(bool)),this,SLOT(trig_transp(bool)));
    connect(action_editar_paleta, SIGNAL(triggered()), this, SLOT(ventana_pal_ed()));
    connect(opcGroup, SIGNAL(triggered(QAction *)), this, SLOT(mostrar_vel_anim(QAction *)));
    connect(i18nGroup, SIGNAL(triggered(QAction *)), this, SLOT(i18n_translate(QAction *)));
}

//internacionalizacion
void Spr_viewer::i18n_translate(QAction *action)
{
    if (action==action_Espa_ol)
        trans.load(QString::fromUtf8(":/i18n/spritev_es"));
    else if (action==action_English)
        trans.load(QString::fromUtf8(":/i18n/spritev_en"));

    QApplication::installTranslator(&trans);
    this->retranslateUi(this);

    if (MOSTRAR_VEL==true)
        timeout_label->setText(QString::number(timeout) + tr(" ms / frame"));
    else if (MOSTRAR_VEL==false)
        timeout_label->setText(QString::number(1000/timeout) + tr(" FPS"));

    emit lang_change(&trans);
}

//se fija si tiene que mostrar la velocidad en ms/frame o en FPS
void Spr_viewer::mostrar_vel_anim(QAction *action)
{
    if (action==action_fps)
    {
        //actualiza el label que indica cuantas imagenes se presentan en 1 segundo
        timeout_label->setText(QString::number(1000/timeout) + tr(" FPS"));
        action_set_timeout->setText(tr("&Configurar FPS"));
        MOSTRAR_VEL=false;
    } else if (action==action_tpf) {
        //actualiza el label que indica cuanto tiempo dura una imagen antes de presentar otra
        timeout_label->setText(QString::number(timeout) + tr(" ms / frame"));
        action_set_timeout->setText(tr("&Configurar Timeout"));
        MOSTRAR_VEL=true;
    }
}

//muestra el dialogo para editar la paleta
void Spr_viewer::ventana_pal_ed()
{
    //Si no hay un sprite cargado (por lo tanto no hay una paleta)
    if (!spr)
    {
        //muestra el dialogo con el color por defecto (todo negro)
        ventana= new Pal_edit(this);
    } else {
        //inicializa el dialogo con la paleta actual
        ventana= new Pal_edit(this, 0, spr->palette, spr->palette_size);

        //conecta las se�ales necesarias para cambiar el color automaticamente
        connect(ventana, SIGNAL(color_changed(int, unsigned int, unsigned int, unsigned int)),
                this, SLOT(cambio_color_paleta(int, unsigned int, unsigned int, unsigned int)));
        //Si se hace click en el boton cancelar vuelve a la paleta anterior (no implementado)
        connect(ventana, SIGNAL(cancelar_ed()),
                this, SLOT(pal_ed_cancelar()));
    }

    //Si quieren que el dialogo sea modal
    //(que no se pueda usar la ventana ppal, mientras el dialogo esta abierto),
    //descomenten esta linea
//    ventana->setModal(true);

    //muestra la ventana
    ventana->show();
}

//Slot que recibe la se�al del boton cancelar.
void Spr_viewer::pal_ed_cancelar()
{
    
}

//Slot que recibe la se�al para cambiar un color de la paleta
void Spr_viewer::cambio_color_paleta(int id, unsigned int r, unsigned int g, unsigned int b)
{
    spr->palette[id].r=r;
    spr->palette[id].g=g;
    spr->palette[id].b=b;
    cambiar_imagen();
}

//Si el action ta checked (actualiza la variable de la clase)
void Spr_viewer::trig_transp(bool check)
{
    transparente=check;
    cambiar_imagen();
}

//Muestra el acerca de embebido de QT
void Spr_viewer::about_qt()
{
    QMessageBox::aboutQt ( this, tr("Acerca de QT"));
}

//Muestra el Acerca de de la app [REVISAR]
void Spr_viewer::about()
{
    QMessageBox::about ( this, tr("Acerca de Sprite Viewer"), tr("Visite la pagina principal: "
            "spriteviewer.sf.net"));
}

//Destructor de la clase. libera el espacio si fue cargado un sprite
Spr_viewer::~Spr_viewer()
{
    if (spr)
        sprite_free(spr);
}

//Funcion que abre un archivo .spr pasado como argumento
void Spr_viewer::abrir_archivo(QString file)
{
    static QDir last_dir;

    //si el argumento no es un archivo valido muestra el dialogo para abrir uno
    if (file.isEmpty()) archivo = QFileDialog::getOpenFileName(this,
                                               tr("Abrir Sprite"),
                                               last_dir.absolutePath(),
                                               tr("Sprite Files (*.spr)"));
    else archivo=file;

    //si el archivo dentro del dialogo existe
    if (QFile::exists(archivo))
    {
        //guarda el directorio
        last_dir = QDir(archivo);
        //abre el sprite
        spr=sprite_open(archivo.toLocal8Bit(),NULL);
        if (spr==NULL)
        {
            qWarning() << tr("No se pudo cargar el archivo: %1").arg(archivo);
            QMessageBox::warning(this,
                                 "SpriteViewer",
                                 tr("No se pudo cargar el archivo: %1").arg(archivo));

            setWindowTitle("Sprite Viewer");
            return;
        }
        //actualiza el titulo de la ventana con el path absoluto
        setWindowTitle("Sprite Viewer - " + QFileInfo::QFileInfo(spr->filename).fileName());
        //actualiza los controles del formulario
        cargar_sprites();
        //va a la primera imagen del sprite
        cambiar_imagen(1);
    }
}

//abre un dialogo para cambiar la paleta de colores
void Spr_viewer::abrir_palete()
{
    //si no hay un archivo abierto sale de la funcion
    if (!spr) return;

    static QDir last_dir;

    //muestra el dialogo para abrir el archivo .pal
    archivo = QFileDialog::getOpenFileName(this,
                                           tr("Abrir Sprite"),
                                           last_dir.absolutePath(),
                                           tr("Palette Files (*.pal)"));

    //Si el archivo es valido
    if (QFile::exists(archivo))
    {
        //guarda el ultimo dir visitado
        last_dir = QDir(archivo);
        //llama a la funcion creada por mi =) para cambiar la paleta
        change_palete(spr, archivo.toAscii(), NULL);
        //Redibuja la imagen actual
        cambiar_imagen();
    }
}

//Actualiza los controles en base a el sprite cargado
void Spr_viewer::cargar_sprites()
{
    //setea el rango del slidebar
    nimages->setRange(1, spr->nimages);

    //los spin box
    spin_min->setMaximum(spr->nimages);
    spin_min->setMinimum(1);
    spin_min->setValue(1);

    spin_max->setMaximum(spr->nimages);
    spin_max->setMinimum(1);
    spin_max->setValue(spr->nimages);

    //y lleva el slidebar al 1er elemento
    nimages->setValue(0);
}

//funcion para cambiar la imagen dentro del label
void Spr_viewer::cambiar_imagen(int numero)
{
    if(!spr) return;

    QPixmap mapa;
    int size[]={'\0'};
    unsigned char *buf;
    static int actual;
    if (numero!=-1) actual = numero;

    //obtengo un puntero a donde esta la imagen en BMP a mostrar
    buf=(unsigned char *) sprite_to_bmp(spr, actual-1, size, NULL);
    
    //si transparente es true
    if (transparente)
    {
        QPalette app_pal;
        //obtiene el color de fondo de la ventana actual
        QColor color_fondo = app_pal.color (QPalette::Window);
        //y lo asigna al primer elemento BGRx (en la paleta R y B estan invertidos)
        //de la paleta del buffer
        buf[56]=(const char)color_fondo.red();
        buf[55]=(const char)color_fondo.green();
        buf[54]=(const char)color_fondo.blue();
    }

    //carga los datos dentro del mapa de pixeles
    mapa.loadFromData(buf, *size);

    //setea la imagen dentro del label
    label->setPixmap(mapa);

    //libera la memoria utilizada por el buffer
    //esto arregla un problema que cada vez que se cambiaba la imagen
    //el programa ocupaba mas memoria =).
    free(buf);
}

//slot destinado a decidir cuando el sprite tiene que animarse
void Spr_viewer::play()
{
    //si esta animando
    if (PLAY)
    {
        //parar el timeout que llama a la funcion adv_frame()
        timer.stop();
        //actualiza el boton play_button
        play_button->setText(">");
        //indica que ya paramos de animar
        PLAY=false;

    //si no
    } else {
        //inicia el timeout. Esta funcion llama a adv_frame cada 'timeout' milisegundos.
        timer.start(timeout);

        if (MOSTRAR_VEL)
        {
            //actualiza el label que indica cuanto tiempo dura una imagen antes de presentar otra
            timeout_label->setText(QString::number(timeout) + tr(" ms / frame"));
        } else {
            //actualiza el label que indica cuanto tiempo dura una imagen antes de presentar otra
            timeout_label->setText(QString::number(1000/timeout) + tr(" FPS"));
        }

        //actualiza el boton play_button (es lo mas simple que encontre =) )
        play_button->setText("||");

        //indica que iniciamos la animacion
        PLAY=true;
    }
}

//funcion llamada por timer para que avance en uno la imagen en el sprite
void Spr_viewer::adv_frame()
{
    //si seguimos en rango (menor que el valor del spin_max)
    if (nimages->value() < spin_max->value()) {
        //avanza al proximo
        nimages->setValue(nimages->value()+1);
    //si no
    } else {
        // vamos al valor del spin_min
        nimages->setValue(spin_min->value());
    }
}

//funcion para cambiar la velocidad de actualizacion de las imagenes
//timeout es la cantidad de milisegundos entre cada imagen
//para valores bajos, no va a ser preciso, ya que depende de la
//carga actual del procesador.
void Spr_viewer::set_timeout()
{
    bool ok;
    
    if (MOSTRAR_VEL)
    {
        timeout = QInputDialog::getInteger(this, tr("Inserte valor"),
                                                 tr("Valor del timeout (ms):"),
                                                 timeout, 1, 5000, 1, &ok);
    } else {
        timeout = QInputDialog::getInteger(this, tr("Inserte valor"),
                                                 tr("Inserte FPS:"),
                                                 1000/timeout, 1, 1000, 1, &ok);
        timeout = 1000/timeout;
    }

    if (PLAY && ok)
    {
        //parar el timeout que llama a la funcion adv_frame()
        timer.stop();
        //inicia el timeout. Esta funcion llama a adv_frame cada 'timeout' milisegundos.
        timer.start(timeout);
    }

    if (MOSTRAR_VEL)
    {
        //actualiza el label que indica cuanto tiempo dura una imagen antes de presentar otra
        timeout_label->setText(QString::number(timeout) + tr(" ms / frame"));
    } else {
        //actualiza el label que indica cuanto tiempo dura una imagen antes de presentar otra
        timeout_label->setText(QString::number(1000/timeout) + tr(" FPS"));
    }
}

//funcion para exportar las imagenes del sprite a una carpeta
void Spr_viewer::spr_to_bmp()
{
    //si no hay sprite cargado retorna
    if (!spr) return;

    static QString dir;
    QString fileN;

    //muestra el dialogo de seleccion de carpeta para seleccionar el directorio
    //de destino de las imagenes
    dir = QFileDialog::getExistingDirectory(this,
                  tr("Destino de los archivos:"),
                  dir,
                  QFileDialog::ShowDirsOnly 
                  | QFileDialog::DontResolveSymlinks);

    //si apreto cancelar
    if(dir=="") return;

    //para cada imagen en el sprite
    for(uint i=0;i<spr->nimages;i++)
    {
        QString str;
        QString num;

        //en base a en n� de sprite coloca uno o dos ceros a la izda (o nada si es mayor a 100)
        if (i<10) {
            num = "00" + QString::number(i);
        } else if(i<100) {
            num = "0" + QString::number(i);
        } else {
            num = QString::number(i);
        }

        //ambito para obtener el nombre de archivo abierto (sin ruta ni extension)
        {
            int tmp;

            fileN=QFileInfo::QFileInfo(spr->filename).fileName();
            tmp = fileN.lastIndexOf(".");
            fileN.chop(fileN.size()-tmp);
        }

        //str contiene el nombre de la imagen de destino (con ruta)
        //ej. C:/imagenes_spr/phoenix_wings005.bmp 
        //(QT usa las barras no invertidas en sus rutas, en windows funciona =) )
        str = dir + "/" + fileN + num + ".bmp";

        //exporta la imagen a la carpeta
        sprite_to_bmp_file(spr, i,str.toAscii(), NULL);
    }
}
