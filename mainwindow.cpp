#include "mainwindow.h"
#include "ui_mainwindow.h"

#define PI (180/3.14)

int runParam= 1;
double iterasi1=0;
double iterasi2=0;
double iterasi3=0;
char id;
int gasL,wLsp,wL;
int gasR,wRsp,wR;
int orientasi;
int pos1,teta1,pos2,teta2,v_1,v_2;
int cartsY,cartsT;
int cartX,cartY;
int cartgX,cartgY;
int pid0,pid1;
int fn0,fn1,fn2;
double t;
int state_timer;
bool state;
float path[6];
int y_ax1,y_ax2,x_ax1,x_ax2;
int apit;

QTimer *tim;
QSerialPort *serial;
QByteArray buff;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    runParam= 1;
    pilih_radiobtn(runParam);
    state_timer=0;

    tableForm= new detailed_table();
    aboutusForm= new Form();
    gerak= new trajectory();
    setPos= new settingInitPos();

    state= false;

    start_up();     //start up
    table_cfg();    //configurate table
    chart_cfg();    //configurate chart
}

MainWindow::~MainWindow()
{
    delete ui;      //when window closed
    aboutusForm->close();
    tableForm->close();
    gerak->close();
    serial->close();    //close serial
}

void MainWindow::on_pb0_clicked()
{
    ui->cb0->clear();   //dropbox untuk serial
    Q_FOREACH(QSerialPortInfo port,QSerialPortInfo::availablePorts()){
        ui->cb0->addItem(port.portName());
    }
}

void MainWindow::clickedGraph(QMouseEvent *event)
{
    QPoint point= event->pos(); //posisi mouse dalam cartesian
}

void MainWindow::on_actionexit_triggered()
{
    MainWindow::close();    //close window
    aboutusForm->close();
    tableForm->close();
    gerak->close();
    setPos->close();
}

void MainWindow::on_actionAbout_us_triggered()
{
    aboutusForm->show();    //about us information
}

void MainWindow::on_actionInitial_position_triggered()
{
    setPos->show();
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Start"){    //kirim serial untuk jalan
        ui->pushButton->setText("Stop");
        switch(runParam){
            case 1: serial->write("a"); //combination
            break;
            case 2: serial->write("b"); //formation
            break;
            case 3: serial->write("c"); //gas
            break;
            case 4: serial->write("e"); //combination plus metaheuristic
            break;
        }
    }
    else{
        ui->pushButton->setText("Start");   //kirim serial untuk stop
        serial->write("d");
    }
}

void MainWindow::on_radioButton_clicked()
{
    runParam= 1;    //pilih radiobutton untuk start combination
    pilih_radiobtn(runParam);
}

void MainWindow::on_radioButton_2_clicked()
{
    runParam= 2;    //pilih radiobutton untuk start gas
    pilih_radiobtn(runParam);

}

void MainWindow::on_radioButton_3_clicked()
{
    runParam= 3;    //pilih radiobutton untuk start formation
    pilih_radiobtn(runParam);
}

void MainWindow::on_radioButton_4_clicked()
{
    runParam= 4;
    pilih_radiobtn(runParam);
}

void MainWindow::myTimer()
{
    t= (float)(t+ ((float)(ui->spinBox->text().toInt())/1000)); //time sampling untuk tampil  di chart
}

void MainWindow::on_tabWidget_2_tabBarDoubleClicked(int index)
{
    tableForm->show();   //menampilkan detailed table
}

void MainWindow::on_tabWidget_tabBarDoubleClicked(int index)
{
    if((t1.size()>0)&&(t2.size()>0)&&(t3.size()>0)){
        gerak->show();
    }
    else{
        QMessageBox::warning(this,"notif","not all robots are connected...");
    }
}

void MainWindow::start_up()
{
    ui->pb4->setText("clear");
    ui->groupBox_2->setTitle("Running paramter");
    std::array<QRadioButton*,4> rdbtn={ui->radioButton,ui->radioButton_2,ui->radioButton_3,ui->radioButton_4};
    QString teks[4]={"Combined","Gas","Formation","Intelligence"};
    for(int i=0;i<4;i++){
        rdbtn[i]->setText(teks[i]);
    }
    ui->radioButton->setChecked(true);

    ui->groupBox->setTitle("Serial Communication");
    ui->lab0->setText("Port");
    ui->lab1->setText("Baudrate");
    ui->pb0->setText("Refresh");
    ui->pb1->setText("Connect");

    Q_FOREACH(QSerialPortInfo port,QSerialPortInfo::availablePorts()){
        ui->cb0->addItem(port.portName());
    }

    ui->cb1->addItems({"115200","9600","1200"});

    ui->spinBox->setRange(0,1000);
    ui->spinBox->setSingleStep(200);
    ui->spinBox->setValue(200);

    QWidget::setWindowTitle("Mainwindow of Robot Monitoring");
}

void MainWindow::table_cfg()
{
    std::array<QTableWidget*,3> tbl={ui->tableWidget,ui->tableWidget_2,ui->tableWidget_3};
    QFont tblFont= font();
    tblFont.setPointSize(9);
    tableHeader<<"time(s)"<<"Left Gas(ppm)"<<"Right Gas(ppm)"<<"Orientation"<<"Left PWM(%)"<<"Right PWM(%)"<<"Polar Coordinate"<<"Polar Coordinate"<<"Angle"<<"SetPoint Step"<<"Actual Step"<<"Actual Pos"<<"My fitness value"<<"other fitness 1"<<"other fitness 2";
    for(int i=0;i<3;i++){
        tbl[i]->setColumnCount(15);
        tbl[i]->setRowCount(1);
        tbl[i]->setFont(tblFont);
        tbl[i]->horizontalHeader()->setStretchLastSection(true);
        tbl[i]->setHorizontalHeaderLabels(tableHeader);
    }
}

void MainWindow::chart_cfg()
{
    std::array<QCustomPlot*,12> chart={ui->plot0,ui->plot0L,ui->plot0R,ui->plot0V,ui->plot1,ui->plot1L,ui->plot1R,ui->plot1V,ui->plot2,ui->plot2L,ui->plot2R,ui->plot2V};
    QString y_ax[4]= {"Gas intensity (ppm)","duty cycle (%)","y (cm)","y (cm)"};
    QString x_ax[4]= {"t (s)","t (s)","x (cm)","x (cm)"};

    for(int i=0;i<12;i=i+4){
        for(int j=0;j<4;j++){
            chart[i+j]->yAxis->setLabel(y_ax[j]);
            chart[i+j]->xAxis->setLabel(x_ax[j]);
            chart[i+j]->setInteraction(QCP::iRangeDrag,true);
            chart[i+j]->setInteraction(QCP::iRangeZoom,true);
            chart[i+j]->setInteraction(QCP::iSelectPlottables,true);

            if(j<2){ //untuk PWM dan Gas
                chart[i+j]->xAxis->rescale(true);
                chart[i+j]->yAxis->rescale(true);
                for(int k=0;k<2;k++){
                    chart[i+j]->addGraph();
                    chart[i+j]->graph(k)->setLineStyle(QCPGraph::lsLine);
                    switch(k){
                    case 0:{
                        chart[i+j]->graph(k)->setPen(QPen(Qt::red));
                        chart[i+j]->graph(k)->setBrush(QBrush(QColor(255,0,0,20)));
                    }
                        break;
                    case 1:{
                        chart[i+j]->graph(k)->setPen(QPen(Qt::blue));
                        chart[i+j]->graph(k)->setBrush(QBrush(QColor(0,0,255,20)));
                    }
                        break;
                    }
                }

                switch(j){
                case 0:{
                    QString lgnd[2]= {"left sensor","right sensor"};
                    for(int k=0;k<2;k++){
                        chart[i+j]->graph(k)->setName(lgnd[k]);
                    }
                }
                    break;
                case 1:{
                    QString lgnd[2]= {"left motor","right motor"};
                    for(int k=0;k<2;k++){
                        chart[i+j]->graph(k)->setName(lgnd[k]);
                    }
                }
                    break;
                }

            }
            else if(j==2){ //untuk LiDAR
                chart[i+j]->yAxis2->setVisible(true);
                chart[i+j]->xAxis2->setVisible(true);
                chart[i+j]->xAxis2->setLabel("Robot LiDAR");
                chart[i+j]->xAxis->setRange(-300,300);
                chart[i+j]->xAxis2->setRange(-300,300);
                chart[i+j]->yAxis2->setRange(-300,300);
                chart[i+j]->yAxis->setRange(-300,300);
                chart[i+j]->addGraph();
                chart[i+j]->graph(0)->setLineStyle(QCPGraph::lsNone);
                chart[i+j]->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,10));
                chart[i+j]->graph(0)->setPen(QPen(Qt::red));
            }
            else{   //untuk trajectory
                chart[i+j]->yAxis2->setVisible(true);
                chart[i+j]->xAxis2->setVisible(true);
                chart[i+j]->xAxis2->setLabel("Robot Trajectory");
                chart[i+j]->yAxis->rescale(true);
                chart[i+j]->yAxis2->rescale(true);
                chart[i+j]->xAxis->rescale(true);
                chart[i+j]->xAxis2->rescale(true);
                for(int k=0;k<3;k++){
                    chart[i+j]->addGraph();
                    chart[i+j]->graph(k)->setLineStyle(QCPGraph::lsNone);
                    chart[i+j]->graph(k)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,4));
                    switch(k){
                    case 0:{
                        chart[i+j]->graph(k)->setName("this robot");
                        chart[i+j]->graph(k)->setPen(QPen(Qt::red));
                    }
                        break;
                    case 1:{
                        chart[i+j]->graph(k)->setName("other robot");
                        chart[i+j]->graph(k)->setPen(QPen(Qt::blue));
                    }
                        break;
                    case 2:{
                        chart[i+j]->graph(k)->setName("other robot");
                        chart[i+j]->graph(k)->setPen(QPen(Qt::green));
                    }
                        break;
                    }
                }
            }

            if(j!= 2){
                //legend untuk semua
                QCPLayoutGrid *subLayout= new QCPLayoutGrid;
                chart[i+j]->plotLayout()->addElement(0,1,subLayout);
                subLayout->addElement(0,0,new QCPLayoutElement);
                subLayout->addElement(1,0,chart[i+j]->legend);
                subLayout->addElement(2,0,new QCPLayoutElement);
                chart[i+j]->plotLayout()->setColumnStretchFactor(1,0.001);
                chart[i+j]->legend->setVisible(true);
                QFont legendFont= font();
                legendFont.setPointSize(10);
                chart[i+j]->legend->setFont(legendFont);
            }
        }
    }
}

void MainWindow::on_pb1_clicked()
{
    if(ui->pb1->text()=="Connect"){
        tim= new QTimer(this);
        connect(tim,SIGNAL(timeout()),this,SLOT(myTimer()));

        state_timer=0;
        t=0;

        serial= new QSerialPort(this);
        serial->setPortName(ui->cb0->itemText(ui->cb0->currentIndex()));
        switch(ui->cb1->currentIndex()){
        case 0: serial->setBaudRate(QSerialPort::Baud115200);
            break;
        case 1: serial->setBaudRate(QSerialPort::Baud9600);
            break;
        case 2: serial->setBaudRate(QSerialPort::Baud1200);
            break;
        }
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setReadBufferSize(512);
        serial->open(QIODevice::ReadWrite);
        connect(serial,SIGNAL(readyRead()),this,SLOT(serialRecv()));
        connect(this,SIGNAL(pass_tableContent(float,char*)),tableForm,SLOT(get_tableContent(float,char*)));
        connect(this,SIGNAL(pass_robotLiDAR(QVector<double>,QVector<double>,QVector<double>,QVector<double>,QVector<double>,QVector<double>)),gerak,SLOT(get_LiDAR(QVector<double>,QVector<double>,QVector<double>,QVector<double>,QVector<double>,QVector<double>)));
        connect(this,SIGNAL(pass_trajectory(float*)),gerak,SLOT(draw_trajectory(float*)));
        ui->pb1->setText("Disconnect");
    }
    else{
        serial->close();
        ui->pb1->setText("Connect");
        tim->stop();
        state_timer= 0;
    }
}

void MainWindow::on_pb4_clicked()
{
    clearGraph();
    gL1.clear();gR1.clear();wL1_sp.clear();wR1_sp.clear();crx1.clear();cry1.clear();trAX1.clear();trAY1.clear();trBX1.clear();trBY1.clear();trCX1.clear();trCY1.clear();t1.clear();jaraka1.clear();jarakb1.clear();anglea1.clear();angleb1.clear();fna1.clear();fnb1.clear();fnc1.clear();apit1.clear();
    gL2.clear();gR2.clear();wL2_sp.clear();wR2_sp.clear();crx2.clear();cry2.clear();trAX2.clear();trAY2.clear();trBX2.clear();trBY2.clear();trCX2.clear();trCY2.clear();t2.clear();jaraka2.clear();jarakb2.clear();anglea2.clear();angleb2.clear();fna2.clear();fnb2.clear();fnc2.clear();apit2.clear();
    gL3.clear();gR3.clear();wL3_sp.clear();wR3_sp.clear();crx3.clear();cry3.clear();trAX3.clear();trAY3.clear();trBX3.clear();trBY3.clear();trCX3.clear();trCY3.clear();t3.clear();jaraka3.clear();jarakb3.clear();anglea3.clear();angleb3.clear();fna3.clear();fnb3.clear();fnc3.clear();apit3.clear();
    t=0;

    ui->tableWidget->clear();
    ui->tableWidget_2->clear();
    ui->tableWidget_3->clear();
    tableHeader<<"time(s)"<<"Left Gas(ppm)"<<"Right Gas(ppm)"<<"Orientation"<<"Left PWM(%)"<<"Right PWM(%)"<<"Polar Coordinate"<<"Polar Coordinate"<<"Angle"<<"SetPoint Step"<<"Actual Step"<<"Actual Pos"<<"My fitness value"<<"other fitness 1"<<"other fitness 2";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget_2->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget_3->setHorizontalHeaderLabels(tableHeader);
}

void MainWindow::on_actionsave_to_csv_file_triggered()
{
    savetoFile("./data/robot1.csv",t1,gL1,gR1,wL1_sp,wR1_sp,orientasi1,jaraka1,anglea1,jarakb1,angleb1,spY1,spT1,selfX1,selfY1,trAX1,trAY1,trBX1,trBY1,trCX1,trCY1,fna1,fnb1,fnc1,apit1);
    savetoFile("./data/robot2.csv",t2,gL2,gR2,wL2_sp,wR2_sp,orientasi2,jaraka2,anglea2,jarakb2,angleb2,spY2,spT2,selfX2,selfY2,trAX2,trAY2,trBX2,trBY2,trCX2,trCY2,fna2,fnb2,fnc2,apit2);
    savetoFile("./data/robot3.csv",t3,gL3,gR3,wL3_sp,wR3_sp,orientasi3,jaraka3,anglea3,jarakb3,angleb3,spY3,spT3,selfX3,selfY3,trAX3,trAY3,trBX3,trBY3,trCX3,trCY3,fna3,fnb3,fnc3,apit3);
    QMessageBox::warning(this,"notif","saved...!!");
}

void MainWindow::savetoFile(char *dir,QVector<double> time,QVector<double> leftgas,QVector<double> rightgas,QVector<double> leftpwm,QVector<double> rightpwm,QVector<double> orint,QVector<double> polar1,QVector<double> polar2,QVector<double> polar3,QVector<double> polar4,QVector<double> setpY,QVector<double> setpT,QVector<double> stepX,QVector<double> stepY,QVector<double> trX,QVector<double> trY,QVector<double> trX1,QVector<double> trY1,QVector<double> trX2,QVector<double> trY2,QVector<double> fna,QVector<double> fnb,QVector<double> fnc,QVector<double> angl){
    if(time.size()> 0){
        QFile file(dir);
        if(file.open(QFile::WriteOnly|QFile::Truncate)){
            QTextStream stream(&file);
            stream<<"time (s)"<<"\t"<<"left sensor (ppm)"<<"\t"<<"right sensor (ppm)"<<"\t"<<"left pwm (%)"<<"\t"<<"right pwm (%)"<<"\t"<<"orientasi"<<"\t"<<"distance A (cm)"<<"\t"<<"degree A"<<"\t"<<"distance B"<<"\t"<<"degree B"<<"\t"<<"diff angle"<<"\t"<<"Y setpont step (mm)"<<"\t"<<"set point teta"<<"\t"<<"step X (mm)"<<"\t"<<"step Y(mm)"<<"\t"<<"trajectory X this robot (cm)"<<"\t"<<"trajectory Y this robot (cm)"<<"\t"<<"trajectory X other robot (cm)"<<"\t"<<"trajectory Y other robot (cm)"<<"\t"<<"trajectory X other robot (cm)"<<"\t"<<"trajectory Y other robot (cm)"<<"\t"<<"my fitness value"<<"\t"<<"other fitness value1"<<"\t"<<"other firness value2"<<"\n";
            for(int i=0;i<time.size();i++){
                stream<<time[i]<<"\t"<<leftgas[i]<<"\t"<<rightgas[i]<<"\t"<<leftpwm[i]<<"\t"<<rightpwm[i]<<"\t"<<orint[i]<<"\t"<<polar1[i]<<"\t"<<polar2[i]<<"\t"<<polar3[i]<<"\t"<<polar4[i]<<"\t"<<angl[i]<<"\t"<<setpY[i]<<"\t"<<setpT[i]<<"\t"<<stepX[i]<<"\t"<<stepY[i]<<"\t"<<trX[i]<<"\t"<<trY[i]<<"\t"<<trX1[i]<<"\t"<<trY1[i]<<"\t"<<trX2[i]<<"\t"<<trY2[i]<<"\t"<<fna[i]<<"\t"<<fnb[i]<<"\t"<<fnc[i]<<"\n";
            }
            file.close();
        }
    }
}

void MainWindow::pilih_radiobtn(int val){
    std::array<QRadioButton*,4> rdbtn={ui->radioButton,ui->radioButton_2,ui->radioButton_3,ui->radioButton_4};
    for(int i=1;i<5;i++){
        QPalette palette= rdbtn[i-1]->palette();
        if(i== val){
            palette.setColor(rdbtn[i-1]->backgroundRole(),Qt::red);
            palette.setColor(rdbtn[i-1]->foregroundRole(),Qt::red);
        }
        else{
            palette.setColor(rdbtn[i-1]->backgroundRole(),Qt::black);
            palette.setColor(rdbtn[i-1]->foregroundRole(),Qt::black);
        }
        rdbtn[i-1]->setPalette(palette);
    }
}

void MainWindow::serialRecv()
{
    if(state_timer==0){
        tim->start(ui->spinBox->text().toInt());
        state_timer= 1;
    }

    QByteArray shadowBuff;
    shadowBuff= serial->readAll();
    buff.append(shadowBuff);
    if(shadowBuff.count()< 32){
        char *buff1= buff.data();

        if((t1.size()>0)&&(t2.size()>0)&&(t3.size()>0)){
            state= true;
        }
        else{
            state= false;
        }
        emit pass_tableContent(t,buff1);

        sscanf(buff1,"%c;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",&id,&gasL,&gasR,&orientasi,&cartsY,&cartsT,&cartX,&cartY,&pid0,&pid1,&cartgX,&cartgY,&teta1,&pos1,&teta2,&pos2,&fn0,&fn1,&fn2,&apit);

        float tmp[28];
        tmp[0]=(float)gasL/10;  //ppm gas kiri
        tmp[1]=(float)gasR/10;  //ppm gas kanan
        tmp[2]=(float)pid0/10;  //pwm kiri
        tmp[3]=(float)pid1/10;  //pwm kanan
        tmp[4]=(float)cartgX/50;    //self global position dalam cartesian X (A)
        tmp[5]=(float)cartgY/50;    //self global position dalam cartesian Y (A)
        tmp[6]=((float)(pos1/10)*qSin((teta1+orientasi)/PI))+tmp[4]; //global position dalam cartesian X (B)
        tmp[7]=((float)(pos1/10)*qCos((teta1+orientasi)/PI))+tmp[5]; //global position dalam cartesian X (B)
        tmp[8]=((float)(pos2/10)*qSin((teta2+orientasi)/PI))+tmp[4]; //global position dalam cartesian X (C)
        tmp[9]=((float)(pos2/10)*qCos((teta2+orientasi)/PI))+tmp[5]; //global position dalam cartesian X (C)
        tmp[10]=pos1;   //B dist
        tmp[11]=teta1;  //B angle
        tmp[12]=pos2;   //C dist
        tmp[13]=teta2;  //B angle

        tmp[14]=(float)cartsT;   //set point orientasi
        tmp[15]=(float)cartsY;   //set point jarak Y (cm)
        tmp[16]=(float)cartX/10; //self step dalam cartesian X
        tmp[17]=(float)cartY/10; //self step dalam cartesian Y
        tmp[18]= apit;
        tmp[19]=0;   //self pos dalam LiDAR
        tmp[20]=0;   //self pos dalam LiDAR
        tmp[21]= (float)(pos1/10)*qSin(teta1/PI);    //LiDAR posisi robot lain dalam X
        tmp[22]= (float)(pos1/10)*qCos(teta1/PI);    //LiDAR posisi robot lain dalam Y
        tmp[23]= (float)(pos2/10)*qSin(teta2/PI);    //LiDAR posisi robot lain 1 dalam X
        tmp[24]= (float)(pos2/10)*qCos(teta2/PI);    //LiDAR posisi robot lain 2 dalam Y

        tmp[25]= fn0;
        tmp[26]= fn1;
        tmp[27]= fn2;

        if(id=='a'){
            iterasi1= t;
            t1.append(iterasi1);
            gL1.append(tmp[0]);
            gR1.append(tmp[1]);
            wL1_sp.append(tmp[2]);
            wR1_sp.append(tmp[3]);
            trAX1.append(tmp[4]);
            trAY1.append(tmp[5]);
            trBX1.append(tmp[6]);
            trBY1.append(tmp[7]);
            trCX1.append(tmp[8]);
            trCY1.append(tmp[9]);
            jaraka1.append(tmp[10]);
            anglea1.append(tmp[11]);
            jarakb1.append(tmp[12]);
            angleb1.append(tmp[13]);
            orientasi1.append(orientasi);
            spY1.append(tmp[15]);
            spT1.append(tmp[14]);
            selfX1.append(tmp[16]);
            selfY1.append(tmp[17]);
            fna1.append(tmp[25]);
            fnb1.append(tmp[26]);
            fnc1.append(tmp[27]);
            apit1.append(tmp[18]);

            crx1.clear();
            cry1.clear();
            for(int i=19;i<25;i=i+2){
                crx1.append(tmp[i]);
                cry1.append(tmp[i+1]);
            }
            path[0]=tmp[4];
            path[1]=tmp[5];
            if(ui->actionShow_graph_2->isChecked()==true){
                plot_graph(ui->plot0,ui->plot0L,ui->plot0R,ui->plot0V,t1,gL1,gR1,wL1_sp,wR1_sp,crx1,cry1,trAX1,trAY1,trBX1,trBY1,trCX1,trCY1);
            }
            else{
                clearGraph();
            }
            isi_table(ui->tableWidget,orientasi,iterasi1,tmp);
        }
        else if(id=='b'){
            iterasi2= t;
            t2.append(iterasi2);
            gL2.append(tmp[0]);
            gR2.append(tmp[1]);
            wL2_sp.append(tmp[2]);
            wR2_sp.append(tmp[3]);
            trAX2.append(tmp[4]);
            trAY2.append(tmp[5]);
            trBX2.append(tmp[6]);
            trBY2.append(tmp[7]);
            trCX2.append(tmp[8]);
            trCY2.append(tmp[9]);
            jaraka2.append(tmp[10]);
            anglea2.append(tmp[11]);
            jarakb2.append(tmp[12]);
            angleb2.append(tmp[13]);
            orientasi2.append(orientasi);
            spY2.append(tmp[15]);
            spT2.append(tmp[14]);
            selfX2.append(tmp[16]);
            selfY2.append(tmp[17]);
            fna2.append(tmp[25]);
            fnb2.append(tmp[26]);
            fnc2.append(tmp[27]);
            apit2.append(tmp[18]);

            crx2.clear();
            cry2.clear();
            for(int i=19;i<25;i=i+2){
                crx2.append(tmp[i]);
                cry2.append(tmp[i+1]);
            }
            path[2]=tmp[4];
            path[3]=tmp[5];
            if(ui->actionShow_graph_2->isChecked()==true){
                plot_graph(ui->plot1,ui->plot1L,ui->plot1R,ui->plot1V,t2,gL2,gR2,wL2_sp,wR2_sp,crx2,cry2,trAX2,trAY2,trBX2,trBY2,trCX2,trCY2);
            }
            else{
                clearGraph();
            }
            isi_table(ui->tableWidget_2,orientasi,iterasi2,tmp);
        }
        else if(id=='c'){
            iterasi3= t;
            t3.append(iterasi3);
            gL3.append(tmp[0]);
            gR3.append(tmp[1]);
            wL3_sp.append(tmp[2]);
            wR3_sp.append(tmp[3]);
            trAX3.append(tmp[4]);
            trAY3.append(tmp[5]);
            trBX3.append(tmp[6]);
            trBY3.append(tmp[7]);
            trCX3.append(tmp[8]);
            trCY3.append(tmp[9]);
            jaraka3.append(tmp[10]);
            anglea3.append(tmp[11]);
            jarakb3.append(tmp[12]);
            angleb3.append(tmp[13]);
            orientasi3.append(orientasi);
            spY3.append(tmp[15]);
            spT3.append(tmp[14]);
            selfX3.append(tmp[16]);
            selfY3.append(tmp[17]);
            fna3.append(tmp[25]);
            fnb3.append(tmp[26]);
            fnc3.append(tmp[27]);
            apit3.append(tmp[18]);

            crx3.clear();
            cry3.clear();
            for(int i=19;i<25;i=i+2){
                crx3.append(tmp[i]);
                cry3.append(tmp[i+1]);
            }
            path[4]=tmp[4];
            path[5]=tmp[5];
            if(ui->actionShow_graph_2->isChecked()==true){
                plot_graph(ui->plot2,ui->plot2L,ui->plot2R,ui->plot2V,t3,gL3,gR3,wL3_sp,wR3_sp,crx3,cry3,trAX3,trAY3,trBX3,trBY3,trCX3,trCY3);
            }
            else{
                clearGraph();
            }
            isi_table(ui->tableWidget_3,orientasi,iterasi3,tmp);
        }

        emit pass_robotLiDAR(crx1,cry1,crx2,cry2,crx3,cry3);
        emit pass_trajectory(path);

        buff.clear();
        shadowBuff.clear();
        serial->flush();
    }
}

void MainWindow::plot_graph(QCustomPlot* plot1,QCustomPlot* plot2,QCustomPlot* plot3, QCustomPlot* plot4,QVector<double> time,QVector<double> leftgas,QVector<double> rightgas,QVector<double> leftpwm,QVector<double> rightpwm,QVector<double> LiDARX,QVector<double> LiDARY,QVector<double> trajectoryAX,QVector<double> trajectoryAY,QVector<double> trajectoryBX,QVector<double> trajectoryBY,QVector<double> trajectoryCX,QVector<double> trajectoryCY)
{
    std::array<QCustomPlot*,4> plt={plot1,plot2,plot3,plot4};
    std::array<QVector<double>,13> aray={time,leftgas,rightgas,leftpwm,rightpwm,LiDARX,LiDARY,trajectoryAX,trajectoryBX,trajectoryCX,trajectoryAY,trajectoryBY,trajectoryCY};

    for(int i=0;i<4;i++){
        switch(i){
        case 0:{
            for(int j=0;j<2;j++){
                plt[i]->graph(j)->setData(aray[0],aray[j+1]);
                plt[i]->graph(j)->rescaleAxes(true);
            }
        }
            break;
        case 1:{
            for(int j=0;j<2;j++){
                plt[i]->graph(j)->setData(aray[0],aray[j+3]);
                plt[i]->graph(j)->rescaleAxes(true);
            }
        }
            break;
        case 2:{
            plt[i]->graph(0)->setData(aray[5],aray[6]);
        }
            break;
        case 3:{
            for(int j=0;j<3;j++){
                plt[i]->graph(j)->setData(aray[j+7],aray[j+10]);
                plt[i]->graph(j)->rescaleAxes(true);
            }
        }
            break;
        }
        plt[i]->replot();
        plt[i]->update();
    }
}

void MainWindow::isi_table(QTableWidget* tabel,int orient,float itera,float* data)
{
    tabel->setItem(tabel->rowCount()-1,0,new QTableWidgetItem(QString().number(itera)));
    tabel->setItem(tabel->rowCount()-1,1,new QTableWidgetItem(QString().number(data[0])));
    tabel->setItem(tabel->rowCount()-1,2,new QTableWidgetItem(QString().number(data[1])));
    tabel->setItem(tabel->rowCount()-1,3,new QTableWidgetItem(QString().number(orient)));
    tabel->setItem(tabel->rowCount()-1,4,new QTableWidgetItem(QString().number(data[2])));
    tabel->setItem(tabel->rowCount()-1,5,new QTableWidgetItem(QString().number(data[3])));
    tabel->setItem(tabel->rowCount()-1,6,new QTableWidgetItem("("+QString().number(data[10]/10)+"<"+QString().number(data[11])+")"));
    tabel->setItem(tabel->rowCount()-1,7,new QTableWidgetItem("("+QString().number(data[12]/10)+"<"+QString().number(data[13])+")"));
    tabel->setItem(tabel->rowCount()-1,8,new QTableWidgetItem(QString().number(data[18])));
    tabel->setItem(tabel->rowCount()-1,9,new QTableWidgetItem("("+QString().number(data[15])+"<"+QString().number(data[14])+")"));
    tabel->setItem(tabel->rowCount()-1,10,new QTableWidgetItem("("+QString().number(data[16])+","+QString().number(data[17])+")"));
    tabel->setItem(tabel->rowCount()-1,11,new QTableWidgetItem("("+QString().number(data[4])+","+QString().number(data[5])+")"));
    tabel->setItem(tabel->rowCount()-1,12,new QTableWidgetItem(QString().number(data[25])));
    tabel->setItem(tabel->rowCount()-1,13,new QTableWidgetItem(QString().number(data[26])));
    tabel->setItem(tabel->rowCount()-1,14,new QTableWidgetItem(QString().number(data[27])));


    tabel->insertRow(tabel->rowCount());
    tabel->resizeColumnsToContents();
    tabel->resizeRowsToContents();
}

void MainWindow::clearGraph(){
    std::array<QCustomPlot*,12> chart={ui->plot0,ui->plot0L,ui->plot0R,ui->plot0V,ui->plot1,ui->plot1L,ui->plot1R,ui->plot1V,ui->plot2,ui->plot2L,ui->plot2R,ui->plot2V};
    for(int i=0;i<12;i=i+4){
        for(int j=0;j<4;j++){
            if(j<2){
                chart[i+j]->yAxis->setRange(0,5);
                chart[i+j]->xAxis->setRange(0,5);
                for(int k=0;k<2;k++){
                    chart[i+j]->graph(k)->data()->clear();
                }
            }
            else if(j==2){
                chart[i+j]->graph(0)->data()->clear();
            }
            else{
                chart[i+j]->xAxis->setRange(0,5);
                chart[i+j]->yAxis->setRange(0,5);
                for(int k=0;k<3;k++){
                    chart[i+j]->graph(k)->data()->clear();
                }
            }
            chart[i+j]->replot();
        }
    }
}
