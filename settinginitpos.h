#ifndef SETTINGINITPOS_H
#define SETTINGINITPOS_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>

namespace Ui {
class settingInitPos;
}

class settingInitPos : public QWidget
{
    Q_OBJECT

public:
    explicit settingInitPos(QWidget *parent = nullptr);
    ~settingInitPos();

public slots:

private:
    Ui::settingInitPos *ui;

private slots:
    void startUp();
    void openFile(char *dir);
    void saveFile(char *dir);

    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // SETTINGINITPOS_H
