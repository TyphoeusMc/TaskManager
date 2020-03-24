#include "ramstats.h"
#include "ui_ramstats.h"

using namespace std;

extern int INTERVAL;
ifstream mem_info;
vector <string> mem_info_read;
QVector <double> used(30,0);      //Store cpu utilization value for the graph
QVector <double> buffer(30,0);
QVector <double> swap_use(30,0);
QVector <double> drawline(30,0);
QVector <double> xaxis1={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                        21,22,23,24,25,26,27,28,29,30};
QBrush BGC(QColor(255,235,201));
QBrush used_buffer_fill(QColor(255,254,143));
QBrush swap_fill(QColor(255,176,176));
QPen swap_used_color(QColor(245,121,0),2);
QPen buffer_color(QColor(241,85,85),2);
QPen GCdrawline(QColor(255,255,255),0);



ramstats::ramstats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ramstats)
{
    ui->setupUi(this);
    timer=new QTimer(this);
    timer->setInterval(INTERVAL);
    connect(timer,SIGNAL(timeout()),this,SLOT(UpdateWindow()));
    InitializeWindow();
    timer->start();
}

void ramstats::InitializeWindow()
{
    mem_info.open("/proc/meminfo");
    //read mem info
    mem_info_read=ReadInfo(mem_info,{1,2,3,4,5,15,16},0);
    //used and buffer
    ui->ram_plot->addGraph();
    ui->ram_plot->addGraph();
    ui->ram_plot->addGraph();
    ui->ram_plot->graph(0)->setData(xaxis1,used);
    ui->ram_plot->graph(1)->setData(xaxis1,buffer);
    ui->ram_plot->graph(2)->setData(xaxis1,drawline);
    ui->ram_plot->setBackground(BGC);
    ui->ram_plot->graph(0)->setPen(swap_used_color);
    ui->ram_plot->graph(1)->setPen(buffer_color);
    ui->ram_plot->graph(2)->setPen(GCdrawline);
    ui->ram_plot->graph(0)->setBrush(used_buffer_fill);
    ui->ram_plot->graph(1)->setBrush(swap_fill);
    ui->ram_plot->xAxis->setTicks(false);
    ui->ram_plot->xAxis->setBasePen(swap_used_color);
    ui->ram_plot->yAxis->setVisible(false);
    ui->ram_plot->xAxis->setRange(1,30);
    ui->ram_plot->yAxis->setRange(0,str2uns(mem_info_read[0])/1000000);
    ui->ram_plot->xAxis->setPadding(1);
    ui->ram_plot->yAxis->setPadding(1);
    //swap
    ui->swap_plot->addGraph();
    ui->swap_plot->addGraph();
    ui->swap_plot->graph(0)->setData(xaxis1,swap_use);
    ui->swap_plot->graph(1)->setData(xaxis1,drawline);
    ui->swap_plot->setBackground(BGC);
    ui->swap_plot->graph(0)->setPen(swap_used_color);
    ui->swap_plot->graph(1)->setPen(GCdrawline);
    ui->swap_plot->graph(0)->setBrush(swap_fill);
    ui->swap_plot->xAxis->setTicks(false);
    ui->swap_plot->xAxis->setBasePen(swap_used_color);
    ui->swap_plot->yAxis->setVisible(false);
    ui->swap_plot->xAxis->setRange(1,30);
    ui->swap_plot->yAxis->setRange(0,str2uns(mem_info_read[5])/1000000);
    ui->swap_plot->xAxis->setPadding(1);
    ui->swap_plot->yAxis->setPadding(1);
    //other
    ui->ram_total->setText(QString::number(double(str2uns(mem_info_read[0]))/1000000,'f',2)+"GB");
    ui->swap->setText(QString::number(double(str2uns(mem_info_read[5]))/1000000,'f',2)+"GB");
    mem_info.close();
}

void ramstats::UpdateWindow()
{
    mem_info.open("/proc/meminfo");
    mem_info_read.clear();
    mem_info_read=ReadInfo(mem_info,{1,2,3,4,5,15,16},0);
    //Draw ram graph
    for(int counter=0;counter<=28;counter++)
        used[counter]=used[counter+1];
    used[29]=double(str2uns(mem_info_read[0])-str2uns(mem_info_read[1]))/1000000;
    ui->memused->setText(QString::number(used[29],'f',2)+"GB");
    ui->ram_plot->graph(0)->setData(xaxis1,used);
    ui->ram_plot->replot();
    ui->ram_plot->graph(0)->setChannelFillGraph(ui->ram_plot->graph(1));
    //Draw buffer graph
    for(int counter=0;counter<=28;counter++)
        buffer[counter]=buffer[counter+1];
    buffer[29]=double(str2uns(mem_info_read[3])+str2uns(mem_info_read[4]))/1000000;
    ui->buff->setText(QString::number(buffer[29],'f',2)+"GB");
    ui->ram_plot->graph(1)->setData(xaxis1,buffer);
    ui->ram_plot->replot();
    ui->ram_plot->graph(1)->setChannelFillGraph(ui->ram_plot->graph(2));
    //Draw swap graph
    for(int counter=0;counter<=28;counter++)
        swap_use[counter]=swap_use[counter+1];
    swap_use[29]=double(str2uns(mem_info_read[5])-str2uns(mem_info_read[6]))/1000000;
    ui->swap->setText(QString::number(swap_use[29],'f',2)+"GB");
    ui->swap_num->setText(QString::number(swap_use[29],'f',2)+"GB");
    ui->swap_plot->graph(0)->setData(xaxis1,swap_use);
    ui->swap_plot->replot();
    ui->swap_plot->graph(0)->setChannelFillGraph(ui->swap_plot->graph(1));
    //Others
    ui->memunused->setText(QString::number(double(str2uns(mem_info_read[1]))/1000,'f',2)+"MB");
    ui->memavai->setText(QString::number(double(str2uns(mem_info_read[2]))/1000,'f',2)+"MB");
    mem_info.close();
}

ramstats::~ramstats()
{
    delete ui;
}
