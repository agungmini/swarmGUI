#ifndef DETAILED_TABLE_H
#define DETAILED_TABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QDebug>
#include <qmath.h>

namespace Ui {
class detailed_table;
}

class detailed_table : public QWidget
{
    Q_OBJECT

public:
    explicit detailed_table(QWidget *parent = nullptr);
    ~detailed_table();

    void table_cfg();
    void isi_table(QTableWidget* tabel,int orient,float itera,float* data,QCheckBox *check_box);

public slots:
    void get_tableContent(float time,char *charBuff);

private:
    Ui::detailed_table *ui;
    QStringList tableHeader;

private slots:
};

#endif // DETAILED_TABLE_H
