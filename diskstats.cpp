#include "diskstats.h"
#include "ui_diskstats.h"

diskstats::diskstats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::diskstats)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

diskstats::~diskstats()
{
    delete ui;
}

