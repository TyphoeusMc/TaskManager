#include "inputexec.h"
#include "ui_inputexec.h"

InputExec::InputExec(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputExec)
{
    ui->setupUi(this);
    connect(ui->pushButton_2,SIGNAL(pressed()),this,SLOT(ExeNewProcess()));
    connect(ui->pushButton,SIGNAL(pressed()),this,SLOT(close()));
}

InputExec::~InputExec()
{
    delete ui;
}

void InputExec::ExeNewProcess()
{
    if(ui->lineEdit->text().toStdString().empty())
        ui->label_2->setText("输入不能为空!");
    else
    {
        emit path(ui->lineEdit->text());
        ui->lineEdit->clear();
        this->close();
    }
}
