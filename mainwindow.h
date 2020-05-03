#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "qcustomplot.h"
#include <QDebug>
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include "math.h"
#include <QtMath>
#include <QtCore/qmath.h>
#include <QHBoxLayout>

//other form
#include "form.h"   //form untuk about us
#include "detailed_table.h" //form untuk tabel yang lebar
#include "trajectory.h" //untuk trajectory
#include "settinginitpos.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
    Form *aboutusForm;
    detailed_table *tableForm;
    trajectory *gerak;
    settingInitPos *setPos;

public slots:
    void myTimer();

signals:
    void pass_tableContent(float time,char *charBuff);
    void pass_robotLiDAR(QVector<double> Ax,QVector<double> Ay,QVector<double> Bx,QVector<double> By,QVector<double> Cx,QVector<double> Cy);
    void pass_trajectory(float *val);

private:
                   //gas//pwm//cartesian radar//trajectory robot
    QVector<double> gL1,gR1,wL1_sp,wR1_sp,crx1,cry1,trAX1,trAY1,trBX1,trBY1,trCX1,trCY1,t1,jaraka1,jarakb1,anglea1,angleb1,orientasi1,spY1,spT1,selfX1,selfY1,fna1,fnb1,fnc1,apit1;
    QVector<double> gL2,gR2,wL2_sp,wR2_sp,crx2,cry2,trAX2,trAY2,trBX2,trBY2,trCX2,trCY2,t2,jaraka2,jarakb2,anglea2,angleb2,orientasi2,spY2,spT2,selfX2,selfY2,fna2,fnb2,fnc2,apit2;
    QVector<double> gL3,gR3,wL3_sp,wR3_sp,crx3,cry3,trAX3,trAY3,trBX3,trBY3,trCX3,trCY3,t3,jaraka3,jarakb3,anglea3,angleb3,orientasi3,spY3,spT3,selfX3,selfY3,fna3,fnb3,fnc3,apit3;
    QStringList tableHeader;

private slots:
    void on_pb0_clicked();
    void on_pb1_clicked();
    void clickedGraph(QMouseEvent *event);
    void serialRecv();
    void on_pb4_clicked();
    void on_actionexit_triggered();
    void on_actionsave_to_csv_file_triggered();
    void on_actionAbout_us_triggered();
    void on_pushButton_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_tabWidget_2_tabBarDoubleClicked(int index);

    //fungsi-fungsi
    void start_up();
    void table_cfg();
    void chart_cfg();
    void clearGraph();
    void pilih_radiobtn(int val);
    void isi_table(QTableWidget* tabel,int orientasi,float itera,float* data);
    void plot_graph(QCustomPlot* plot1,QCustomPlot* plot2,QCustomPlot* plot3, QCustomPlot* plot4,QVector<double> time,QVector<double> leftgas,QVector<double> rightgas,QVector<double> leftpwm,QVector<double> rightpwm,QVector<double> LiDARX,QVector<double> LiDARY,QVector<double> trajectoryAX,QVector<double> trajectoryAY,QVector<double> trajectoryBX,QVector<double> trajectoryBY,QVector<double> trajectoryCX,QVector<double> trajectoryCY);
    void savetoFile(char *dir,QVector<double> time,QVector<double> leftgas,QVector<double> rightgas,QVector<double> leftpwm,QVector<double> rightpwm,QVector<double > orint,QVector<double> polar1,QVector<double> polar2,QVector<double> polar3,QVector<double> polar4,QVector<double> setpY,QVector<double> setpT,QVector<double> stepX,QVector<double> stepY,QVector<double> trX,QVector<double> trY,QVector<double> trX1,QVector<double> trY1,QVector<double> trX2,QVector<double> trY2,QVector<double> fna,QVector<double> fnb,QVector<double> fnc,QVector<double> angl);
    void on_tabWidget_tabBarDoubleClicked(int index);
    void on_radioButton_4_clicked();
    void on_actionInitial_position_triggered();
};

#endif // MAINWINDOW_H
