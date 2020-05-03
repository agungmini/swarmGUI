#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <QWidget>
#include <qcustomplot.h>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>

namespace Ui {
class trajectory;
}

class trajectory : public QWidget
{
    Q_OBJECT

public:
    explicit trajectory(QWidget *parent = nullptr);
    ~trajectory();

public slots:
    void draw_trajectory(float *val);
    void openFile(char *dir);
    void get_LiDAR(QVector<double> Ax,QVector<double> Ay,QVector<double> Bx,QVector<double> By,QVector<double> Cx,QVector<double> Cy);
    void plot_graph(QCustomPlot* plot,QVector<double> val,QVector<double> val1,QVector<double> val2,QVector<double> val3,QVector<double> val4,QVector<double> val5);

private:
    Ui::trajectory *ui;
    QVector<double> pathAx,pathAy,pathBx,pathBy,pathCx,pathCy;

private slots:
    //fungsi fungsi
    void chart_cfg();
};

#endif // TRAJECTORY_H
