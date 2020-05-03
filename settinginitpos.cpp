#include "settinginitpos.h"
#include "ui_settinginitpos.h"

int cart[6];

settingInitPos::settingInitPos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingInitPos)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
    openFile("./cfg/pos");
    startUp();
}

settingInitPos::~settingInitPos()
{
    delete ui;
}

void settingInitPos::startUp(){
    std::array<QLabel*,5> lbel={ui->label,ui->label_2,ui->label_3,ui->label_4,ui->label_5};
    QString teks[5]= {"Robot A","Robot B","Robot C","X Axis (cm)","Y Axis (cm)"};

    for(int i=0;i<5;i++){
        lbel[i]->setText(teks[i]);
    }

    std::array<QPushButton*,3> btn={ui->pushButton_3,ui->pushButton_2,ui->pushButton};
    QString teks1[3]= {"Cancel","Apply","OK"};
    for(int i=0;i<3;i++){
        btn[i]->setText(teks1[i]);
    }

    std::array<QSpinBox*,6> spinbx={ui->spinBox,ui->spinBox_2,ui->spinBox_3,ui->spinBox_4,ui->spinBox_5,ui->spinBox_6};
    for(int i=0;i<6;i++){
        spinbx[i]->setRange(-200,200);
        spinbx[i]->setSingleStep(1);
        spinbx[i]->setValue(cart[i]);
    }
}

void settingInitPos::openFile(char *dir){
    QFile file(dir);
    if(file.open(QFile::ReadWrite)){
        QTextStream in(&file);
        int i=0;
        while(!in.atEnd()){
            QString line= in.readLine();
            cart[i]= line.toInt();
            i++;
        }
    }

    file.close();
}

void settingInitPos::on_pushButton_3_clicked()
{
    this->close();
}

void settingInitPos::on_pushButton_2_clicked()
{
    saveFile("./cfg/pos");
}

void settingInitPos::on_pushButton_clicked()
{
    saveFile("./cfg/pos");
    this->close();
}

void settingInitPos::saveFile(char *dir){
    QFile file(dir);
    std::array<QSpinBox*,6> spinbx={ui->spinBox,ui->spinBox_2,ui->spinBox_3,ui->spinBox_4,ui->spinBox_5,ui->spinBox_6};
    if(file.open(QFile::ReadWrite)){
        file.resize(0);
        QTextStream out(&file);
        int i=0;
        for(int i=0;i<6;i++){
            out<< spinbx[i]->value()<<"\n";
        }
    }
    file.close();
}
