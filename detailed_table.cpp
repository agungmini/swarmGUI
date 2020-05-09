#include "detailed_table.h"
#include "ui_detailed_table.h"

#define PI (180/3.14)

char sh_id;
int sh_gasL,sh_gasR,sh_orientasi,sh_cartsY,sh_cartsT,sh_cartX,sh_cartY,sh_pid0,sh_pid1,sh_cartgX,sh_cartgY,sh_teta1,sh_pos1,sh_teta2,sh_pos2;
int fn_0,fn_1,fn_2;
float time1,time2,time3;
int angle;

detailed_table::detailed_table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detailed_table)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
    table_cfg();
    //ui->checkBox->setCheckState(Qt::CheckState(true));
    ui->checkBox->setTristate(false);
    ui->checkBox->setChecked(true);
}

detailed_table::~detailed_table()
{
    delete ui;
}

void detailed_table::get_tableContent(float time,char *charBuff){
    sscanf(charBuff,"%c;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",&sh_id,&sh_gasL,&sh_gasR,&sh_orientasi,&sh_cartsY,&sh_cartsT,&sh_cartX,&sh_cartY,&sh_pid0,&sh_pid1,&sh_cartgX,&sh_cartgY,&sh_teta1,&sh_pos1,&sh_teta2,&sh_pos2,&fn_0,&fn_1,&fn_2,&angle);
    float tmp[28];
    tmp[0]=(float)sh_gasL/10;  //ppm gas kiri
    tmp[1]=(float)sh_gasR/10;  //ppm gas kanan
    tmp[2]=(float)sh_pid0/10;  //pwm kiri
    tmp[3]=(float)sh_pid1/10;  //pwm kanan
    tmp[4]=(float)sh_cartgX/10;    //self global position dalam cartesian X (A)
    tmp[5]=(float)sh_cartgY/10;    //self global position dalam cartesian Y (A)
    tmp[6]=((float)(sh_pos1/10)*qSin((sh_teta1-sh_orientasi)/PI))+tmp[4]; //global position dalam cartesian X (B)
    tmp[7]=((float)(sh_pos1/10)*qCos((sh_teta1-sh_orientasi)/PI))+tmp[5]; //global position dalam cartesian X (B)
    tmp[8]=((float)(sh_pos2/10)*qSin((sh_teta2-sh_orientasi)/PI))+tmp[4]; //global position dalam cartesian X (C)
    tmp[9]=((float)(sh_pos2/10)*qCos((sh_teta2-sh_orientasi)/PI))+tmp[5]; //global position dalam cartesian X (C)
    tmp[10]=sh_pos1;   //B dist
    tmp[11]=sh_teta1;  //B angle
    tmp[12]=sh_pos2;   //C dist
    tmp[13]=sh_teta2;  //B angle

    tmp[14]=(float)sh_cartsT;   //set point orientasi
    tmp[15]=(float)sh_cartsY;   //set point jarak Y (cm)
    tmp[16]=(float)sh_cartX/10; //self step dalam cartesian X
    tmp[17]=(float)sh_cartY; //self step dalam cartesian Y
    tmp[18]= angle;
    tmp[19]=0;   //self pos dalam LiDAR
    tmp[20]=0;   //self pos dalam LiDAR
    tmp[21]=(float)(sh_pos1/10)*qSin(sh_teta1/PI);    //LiDAR posisi robot lain dalam X
    tmp[22]=(float)(sh_pos1/10)*qCos(sh_teta1/PI);    //LiDAR posisi robot lain dalam Y
    tmp[23]=(float)(sh_pos2/10)*qSin(sh_teta2/PI);    //LiDAR posisi robot lain 1 dalam X
    tmp[24]=(float)(sh_pos2/10)*qCos(sh_teta2/PI);    //LiDAR posisi robot lain 2 dalam Y

    tmp[25]= fn_0;
    tmp[26]= fn_1;
    tmp[27]= fn_2;

    if(sh_id=='a'){
        time1= time;
        isi_table(ui->tableWidget,sh_orientasi,time1,tmp,ui->checkBox);
    }
    else if(sh_id=='b'){
        time2= time;
        isi_table(ui->tableWidget_2,sh_orientasi,time2,tmp,ui->checkBox);
    }
    else if(sh_id=='c'){
        time3= time;
        isi_table(ui->tableWidget_3,sh_orientasi,time3,tmp,ui->checkBox);
    }
}


void detailed_table::table_cfg()
{
    std::array<QTableWidget*,3> tbl={ui->tableWidget,ui->tableWidget_2,ui->tableWidget_3};
    QFont tblFont= font();
    tblFont.setPointSize(9);
    tableHeader<<"time(s)"<<"Left Gas(ppm)"<<"Right Gas(ppm)"<<"Orientation"<<"Left PWM(%)"<<"Right PWM(%)"<<"Polar Coordinate"<<"Polar Coordinate"<<"diff angle"<<"SetPoint Step"<<"Actual Step"<<"Actual Pos"<<"My fitness value"<<"other fitness 1"<<"other fitness 2";
    for(int i=0;i<3;i++){
        tbl[i]->setColumnCount(15);
        tbl[i]->setRowCount(1);
        tbl[i]->setFont(tblFont);
        tbl[i]->horizontalHeader()->setStretchLastSection(true);
        tbl[i]->setHorizontalHeaderLabels(tableHeader);
    }
}

void detailed_table::isi_table(QTableWidget* tabel,int orient,float itera,float* data,QCheckBox *check_box)
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
    if(check_box->isChecked()==true){
        tabel->scrollToBottom();
    }
}
