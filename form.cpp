#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    QWidget::setWindowTitle("About us");
    this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
    //ui->label->setPixmap(QPixmap());
    //ui->label_2->setPixmap(QPixmap("graph/ristek.png"));
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    this->close();
}
