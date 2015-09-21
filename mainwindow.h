#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
typedef struct
{
char mode,parameter[10];
}option;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    void cdir(char* pathname,option* opt,int n);
    void init_parser(const char* input);
    void chp(const char* pathname,char* val1,option* opt,int n);
    void mdir(const char* pathname,option* opt , int n);
    mode_t find_mode(char* para);
    void mve(char *src, char *dest,option opt[5],int o);
    void cpy(char *src,char *dest,option opt[5],int o);
};

#endif // MAINWINDOW_H
