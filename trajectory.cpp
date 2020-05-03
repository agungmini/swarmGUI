#include "trajectory.h"
#include "ui_trajectory.h"

float initialPos[6];

trajectory::trajectory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::trajectory)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
    //start up
    openFile("./cfg/pos");
    chart_cfg();
}

trajectory::~trajectory()
{
    delete ui;
}

void trajectory::openFile(char *dir)
{
    QFile file(dir);
    if(file.open(QFile::ReadWrite)){
        QTextStream in(&file);
        int i=0;
        while(!in.atEnd()){
            QString line= in.readLine();
            initialPos[i]= line.toInt();
            i++;
        }
    }

    file.close();
}

void trajectory::get_LiDAR(QVector<double> Ax,QVector<double> Ay,QVector<double> Bx,QVector<double> By,QVector<double> Cx,QVector<double> Cy){
    ui->widget->graph(0)->setData(Ax,Ay);
    ui->widget_2->graph(0)->setData(Bx,By);
    ui->widget_3->graph(0)->setData(Cx,Cy);
}

void trajectory::draw_trajectory(float *val)
{
    pathAx.append(val[0]+initialPos[0]);
    pathAy.append(val[1]+initialPos[1]);
    pathBx.append(val[2]+initialPos[2]);
    pathBy.append(val[3]+initialPos[3]);
    pathCx.append(val[4]+initialPos[4]);
    pathCy.append(val[5]+initialPos[5]);

    plot_graph(ui->widget_4,pathAx,pathAy,pathBx,pathBy,pathCx,pathCy);
}

void trajectory::plot_graph(QCustomPlot* plot,QVector<double> val,QVector<double> val1,QVector<double> val2,QVector<double> val3,QVector<double> val4,QVector<double> val5)
{
    std::array<QVector<double>,6> aray={val,val1,val2,val3,val4,val5};
    for(int i=0;i<3;i++){
        plot->graph(i)->setData(aray[i*2],aray[(i*2)+1]);
        plot->graph(i)->rescaleAxes(true);
    }
    plot->replot();
    plot->update();
}

void trajectory::chart_cfg()
{
    std::array<QCustomPlot*,4> chart={ui->widget,ui->widget_2,ui->widget_3,ui->widget_4};
    QString titel[4]={"Robot A LiDAR view","Robot A LiDAR view","Robot A LiDAR view","collected robot trajectory"};
    QString Ax[2]= {"y (cm)","x (cm)"};
    for(int i=0;i<4;i++){
        if(i<3){
            chart[i]->yAxis2->setVisible(true);
            chart[i]->xAxis2->setVisible(true);
            chart[i]->xAxis2->setLabel(titel[i]);
            chart[i]->xAxis->setRange(-300,300);
            chart[i]->yAxis->setRange(-300,300);
            chart[i]->xAxis2->setRange(-300,300);
            chart[i]->yAxis2->setRange(-300,300);
            chart[i]->addGraph();
            chart[i]->graph(0)->setLineStyle((QCPGraph::lsNone));
            chart[i]->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,12));
            chart[i]->graph(0)->setPen(QPen(Qt::red));
            chart[i]->xAxis->setLabel(Ax[1]);
            chart[i]->yAxis->setLabel(Ax[0]);
        }
        else{
            chart[i]->yAxis2->setVisible(true);
            chart[i]->xAxis2->setVisible(true);
            chart[i]->xAxis2->setLabel(titel[i]);
            chart[i]->yAxis->rescale(true);
            chart[i]->yAxis2->rescale(true);
            chart[i]->xAxis->rescale(true);
            chart[i]->xAxis2->rescale(true);

            QString robotName[3]= {"robot A","robot B","robot C"};
            QPen pen[3]= {QPen(Qt::red),QPen(Qt::green),QPen(Qt::blue)};

            for(int j=0;j<3;j++){
                chart[i]->addGraph();
                chart[i]->graph(j)->setLineStyle(QCPGraph::lsNone);
                chart[i]->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,6));
                chart[i]->graph(j)->setName(robotName[j]);
                chart[i]->graph(j)->setPen(pen[j]);
            }
            QCPLayoutGrid *subLayout= new QCPLayoutGrid;
            chart[i]->plotLayout()->addElement(0,1,subLayout);
            subLayout->addElement(0,0,new QCPLayoutElement);
            subLayout->addElement(1,0,chart[i]->legend);
            subLayout->addElement(2,0,new QCPLayoutElement);
            chart[i]->plotLayout()->setColumnStretchFactor(1,0.001);
            chart[i]->legend->setVisible(true);
            QFont legendFont= font();
            legendFont.setPointSize(10);
            chart[i]->legend->setFont(legendFont);
        }
    }
}
