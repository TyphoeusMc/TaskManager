#ifndef SYSINFO_H
#define SYSINFO_H

#endif // SYSINFO_H
#include "mainwindow.h"


void MainWindow::GetSysInfo()
{
    string info_get,info_use;
    int posi1,posi2,mem_mb;
    vector<int> cpu_info_line={5,11,13};
    vector<string> cpu_info_read;
    getline(etc,info_get);
    posi1=info_get.find_first_of('\\',0);
    ui->label_publish_ver->setText(QString::fromStdString(info_get.substr(0,posi1-1)));   //publish version
    getline(ver,info_get);
    posi1=info_get.find_first_of('(',0);
    ui->label_ker_version->setText(QString::fromStdString(info_get.substr(0,posi1-1)));   //kernel version
    cpu_info_read=ReadInfo(cpuinfo,cpu_info_line);
    ui->label_cpu_model->setText(QString::fromStdString(cpu_info_read[0]));
    ui->label_CT->setText(QString::fromStdString(cpu_info_read[1]+" core(s) "+cpu_info_read[2]+" thread(s) "));
    getline(meminfo,info_get);
    posi1=info_get.find_first_of(' ',0);
    posi2=info_get.find_first_not_of(' ',posi1);
    posi1=info_get.find_first_of(' ',posi2);
    mem_mb=str2uns(info_get.substr(posi2,posi1-posi2))/1024;
    ui->label_ram->setText(QString::number(mem_mb)+"MB");
}
