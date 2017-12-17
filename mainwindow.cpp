#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QColorDialog>
#include <QAction>
#include <QIODevice>
#include <QTextStream>
#include <QMenu>
#include <QFont>
#include <QFile>
#define SAVEFILE    "save.label"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    color = new QColorDialog(this);
    c = QColor(Qt::black);
    init_UI();
    connect_signals();

    read_history();

}

MainWindow::~MainWindow()
{
    save();
    delete ui;
}

void MainWindow::enterEvent(QEvent*)
{
    if(ui->stackedWidget->currentIndex() == 1)
        return;

    ui->pushButton_unlock->setHidden(false);

    timer.start(2000);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(ui->stackedWidget->currentIndex()==0
            &&!ui->checkBox_background->isChecked())
        return;

    if (event->button() == Qt::LeftButton)
    {
         dragPosition = event->globalPos() - frameGeometry().topLeft();
         event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(ui->stackedWidget->currentIndex()==0
            &&!ui->checkBox_background->isChecked())
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        QPoint pos = event->globalPos() - dragPosition;
        if(frameGeometry().topLeft().y()+pos.y() < 0)
            pos.setY(0);
        move(pos);
        event->accept();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    return;
    QMenu* menu = new QMenu(this);

    QAction* font = new QAction("字体",this);

    menu->addAction(font);

    menu->show();

    menu->move(event->globalPos());

    event->accept();
}

void MainWindow::init_UI()
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->fontComboBox->setHidden(true);
    this->resize(240,240);
    ui->stackedWidget->setMaximumHeight(70);

    ui->spinBox_size->setValue(32);
    ui->textEdit->setFontPointSize(32);

    ui->pushButton_unlock->setIcon(QIcon(":unlock.ico"));
    ui->pushButton_unlock->setIconSize(QSize(30,30));
    ui->pushButton_unlock->setMaximumWidth(30);
    ui->pushButton_unlock->setFlat(true);

    ui->pushButton_lock->setIcon(QIcon(":lock.ico"));
    ui->pushButton_lock->setIconSize(QSize(30,30));
    ui->pushButton_lock->setMaximumWidth(30);
    ui->pushButton_lock->setFlat(true);

    ui->pushButton_close->setIcon(QIcon(":close.ico"));
    ui->pushButton_close->setIconSize(QSize(30,30));
    ui->pushButton_close->setMaximumWidth(30);
    ui->pushButton_close->setFlat(true);

    ui->textEdit->setStyleSheet("background-color:lightblue;");
    ui->stackedWidget->setStyleSheet("background-color:rgb(255,255,127);");
    ui->checkBox_background->setStyleSheet("color:gray;");
    ui->label->setStyleSheet("color:gray;");
    ui->fontComboBox->setStyleSheet("color:gray;");
    ui->pushButton_close->setStyleSheet("QPushButton{border:0px;}");
    ui->pushButton_lock->setStyleSheet("QPushButton{border:0px;}");
    ui->pushButton_unlock->setStyleSheet("QPushButton{border:0px;}");
    ui->pushButton_color->setStyleSheet("QPushButton{border:0px;"
                                        "color:gray;"
                                        "background-color:black;"
                                        "height:20;width:20;}");

    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    ui->textEdit->setTextColor(Qt::black);
    ui->pushButton_color->setMaximumWidth(20);
}

void MainWindow::connect_signals()
{
    connect(ui->pushButton_unlock, SIGNAL(clicked(bool)),
            this, SLOT(setUnlock()) );

    connect(ui->pushButton_lock, SIGNAL(clicked(bool)),
            this, SLOT(setLock()) );

    connect(ui->pushButton_close, SIGNAL(clicked(bool)),
            this, SLOT(Close()) );

    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(fontChanged()) );

    connect(ui->spinBox_size, SIGNAL(valueChanged(int)),
            this, SLOT(sizeChanged()) );

    connect(ui->pushButton_color, SIGNAL(clicked(bool)),
            this, SLOT(selectColor()) );

    connect(color, SIGNAL(colorSelected(QColor)),
            this, SLOT(colorDone()) );

    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timer_out()) );
}

void MainWindow::save()
{
    QString text = ui->textEdit->toPlainText();
    int fondSize = ui->spinBox_size->value();
    bool checked = ui->checkBox_background->isChecked();
    int w = this->width();
    int h = this->height();
    int x = this->pos().x();
    int y = this->pos().y();

    QFile file(SAVEFILE);
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);
    out<<fondSize<<"\n";
    out<<c.red()<<"\n";
    out<<c.green()<<"\n";
    out<<c.blue()<<"\n";
    out<<checked<<"\n";
    out<<w<<"\n";
    out<<h<<"\n";
    out<<x<<"\n";
    out<<y<<"\n";
    out<<text;

    file.close();
}

void MainWindow::read_history()
{
    QString text;
    int fondSize;
    int checked;
    int r,g,b,w,h,x,y;

    QFile file(SAVEFILE);
    if( !file.open(QIODevice::ReadOnly) )
        return;

    QTextStream in(&file);
    in>>fondSize>>r>>g>>b>>checked;
    in>>w>>h>>x>>y;

    while(!in.atEnd())
    {
        QString tmp;
        in>>tmp;
        tmp += "\n";
        text += tmp;
    }
    if(text.length())
        text[ text.length()-1 ]=0;
    file.close();

    c = QColor(r,g,b);
    ui->textEdit->setTextColor(c);
    ui->spinBox_size->setValue(fondSize);
    sizeChanged();

    this->resize(w,h);
    this->move(x,y);

    QString background = QString("QPushButton{border:0px;"
                                 "color:gray;"
                                 "background-color:rgb(%1,%2,%3);"
                                 "height:20;width:20;}")
            .arg(c.red())
            .arg(c.green())
            .arg(c.blue());

    ui->pushButton_color->setStyleSheet(background);

    ui->textEdit->setText(text);

    if(text.isEmpty())
        ui->stackedWidget->setCurrentIndex(1);
    else
    {
        if(checked)
            ui->checkBox_background->setChecked(true);
        else
            ui->checkBox_background->setChecked(false);
        setLock();
    }
}

void MainWindow::setLock()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setDisabled(true);


    if(!ui->checkBox_background->isChecked())
    {
        ui->stackedWidget->setStyleSheet("background-color:transparent;");
        ui->textEdit->setStyleSheet("background-color:transparent;");
        ui->label_title->setHidden(true);
        SetWindowPos((HWND)this->winId(),
                     HWND_BOTTOM,
                     this->pos().x(),
                     this->pos().y(),
                     this->width(),
                     this->height(),
                     SWP_SHOWWINDOW);
    }
    else
    {
        ui->label_title->setHidden(false);
        SetWindowPos((HWND)this->winId(),
                     HWND_TOPMOST,
                     this->pos().x(),
                     this->pos().y(),
                     this->width(),
                     this->height(),
                     SWP_SHOWWINDOW);
    }

}

void MainWindow::setUnlock()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->textEdit->setReadOnly(false);
    ui->textEdit->setDisabled(false);
    ui->textEdit->setStyleSheet("background-color:lightblue;");
    ui->stackedWidget->setStyleSheet("background-color:rgb(255,255,127);");
}

void MainWindow::fontChanged()
{
    QFont font = ui->fontComboBox->currentFont();

    ui->textEdit->setFont(font);
}

void MainWindow::sizeChanged()
{
    int size = ui->spinBox_size->value();

    ui->textEdit->setFontPointSize(size);
}

void MainWindow::selectColor()
{
    color->show();
}

void MainWindow::colorDone()
{
    c = color->currentColor();

    ui->textEdit->setTextColor(c);

    QString background = QString("QPushButton{border:0px;"
                                 "color:gray;"
                                 "background-color:rgb(%1,%2,%3);"
                                 "height:20;width:20;}")
            .arg(c.red())
            .arg(c.green())
            .arg(c.blue());

    ui->pushButton_color->setStyleSheet(background);
}

void MainWindow::Close()
{
    save();
    exit(0);
}

void MainWindow::timer_out()
{
    timer.stop();

    if(ui->stackedWidget->currentIndex() == 1)
        return;

    ui->pushButton_unlock->setHidden(true);
}
