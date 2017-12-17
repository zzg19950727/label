#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QEvent>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void enterEvent(QEvent*);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void contextMenuEvent(QContextMenuEvent*);

    void init_UI();

    void connect_signals();

    void save();

    void read_history();

private slots:
    void setLock();

    void setUnlock();

    void fontChanged();

    void sizeChanged();

    void selectColor();

    void colorDone();

    void Close();

    void timer_out();

private:
    Ui::MainWindow *ui;

    QColorDialog* color;

    QPoint dragPosition;

    QTimer timer;

    QColor c;
};

#endif // MAINWINDOW_H
