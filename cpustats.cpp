#include "cpustats.h"
#include "ui_cpustats.h"

using namespace std;

extern int INTERVAL;
ifstream c_info,c_stat,uptime;
vector <unsigned> CPUinfo(7,0);
vector <unsigned> CPUinfo_old(7,0);
vector <string> cpu_info_read;
unsigned cputotal2,cputotal1;
QVector <double> util(30,0);      //Store cpu utilization value for the graph
QVector <double> freq(30,0);
QVector <double> draw(30,0);
QVector <double> xaxis={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                        21,22,23,24,25,26,27,28,29,30};
QBrush BGCcpu(QColor(230,242,255));
QBrush FillGraph(QColor(255,242,255));
QPen GCcpu(QColor(200,200,255),2);
QPen GCdraw(QColor(255,242,255),0);
string info_in;

cpustats::cpustats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cpustats)
{
    ui->setupUi(this);
    InitializeWindow();
    timer=new QTimer(this);
    timer->setInterval(INTERVAL);
    connect(timer,SIGNAL(timeout()),this,SLOT(UpdateWindow()));
    timer->start();
}

void cpustats::UpdatePT(unsigned proc,unsigned thread)
{
    ui->process->setText(QString::number(proc,10));
    ui->thread->setText(QString::number(thread,10));
}

void cpustats::UpdateWindow()
{
    uptime.open("/proc/uptime");
    c_info.open("/proc/cpuinfo");
    c_stat.open("/proc/stat");
    //Draw cpu graph
    c_stat>>info_in;
    info_in.clear();
    for(int counter=0; counter<=6; counter++)
    {
        c_stat>>info_in;
        CPUinfo.push_back(str2uns(info_in));
        info_in.clear();
    }
    cputotal2=CPUinfo[0]+CPUinfo[1]+CPUinfo[2]+CPUinfo[3]+CPUinfo[4]+CPUinfo[5]+CPUinfo[6];
    cputotal1=CPUinfo_old[0]+CPUinfo_old[1]+CPUinfo_old[2]+CPUinfo_old[3]+CPUinfo_old[4]+CPUinfo_old[5]+CPUinfo_old[6];
    for(int counter=0;counter<=28;counter++)
        util[counter]=util[counter+1];
    //cout<<cputotal1<<"   "<<cputotal2<<endl;
    util[29]=100*double(((CPUinfo[0]-CPUinfo_old[0])+(CPUinfo[2]-CPUinfo_old[2])))/(cputotal2-cputotal1);
    ui->util->setText(QString::number(util[29],'f',2)+"%");
    //ui->cpu_u_plot->addGraph(0);
    ui->util_plot->graph(0)->setData(xaxis,util);
    ui->util_plot->replot();
    ui->util_plot->graph(0)->setChannelFillGraph(ui->util_plot->graph(1));
    //ui->progressBar->setSizeIncrement(260,10);
    CPUinfo_old.clear();
    CPUinfo_old=CPUinfo;
    CPUinfo.clear();
    //Draw freq graph
    cpu_info_read=ReadInfo(c_info,{8},true);
    for(int counter=0;counter<=28;counter++)
        freq[counter]=freq[counter+1];
    ui->freq->setText(QString::fromStdString(cpu_info_read[0])+"MHz");
    cpu_info_read[0].erase(cpu_info_read[0].find_first_of(".",0));
    freq[29]=double(str2uns(cpu_info_read[0]))/1000;
    ui->freq_des->graph(0)->setData(xaxis,freq);
    ui->freq_des->replot();
    ui->freq_des->graph(0)->setChannelFillGraph(ui->freq_des->graph(1));
    uptime>>info_in;
    info_in.erase(info_in.find_first_of(".",0));
    ui->time->setText(QString::number(str2uns(info_in)/3600,10)
                      +":"+QString::number(str2uns(info_in)%3600/60,10)
                      +":"+QString::number(str2uns(info_in)%60,10));
    uptime.close();
    c_info.close();
    c_stat.close();
}

void cpustats::InitializeWindow()
{
    uptime.open("/proc/uptime");
    c_info.open("/proc/cpuinfo");
    //util
    ui->util_plot->addGraph();
    ui->util_plot->addGraph();
    ui->util_plot->graph(0)->setData(xaxis,util);
    ui->util_plot->graph(1)->setData(xaxis,draw);
    ui->util_plot->setBackground(BGCcpu);
    ui->util_plot->graph(0)->setPen(GCcpu);
    ui->util_plot->graph(1)->setPen(GCdraw);
    ui->util_plot->graph(0)->setBrush(FillGraph);
    ui->util_plot->xAxis->setTicks(false);
    ui->util_plot->xAxis->setBasePen(GCcpu);
    ui->util_plot->yAxis->setVisible(false);
    ui->util_plot->xAxis->setRange(1,30);
    ui->util_plot->yAxis->setRange(0,100);
    ui->util_plot->xAxis->setPadding(1);
    ui->util_plot->yAxis->setPadding(1);
    //freq
    ui->freq_des->addGraph();
    ui->freq_des->addGraph();
    ui->freq_des->graph(0)->setData(xaxis,freq);
    ui->freq_des->graph(1)->setData(xaxis,draw);
    ui->freq_des->setBackground(BGCcpu);
    ui->freq_des->graph(0)->setPen(GCcpu);
    ui->freq_des->graph(1)->setPen(GCdraw);
    ui->freq_des->graph(0)->setBrush(FillGraph);
    ui->freq_des->xAxis->setTicks(false);
    ui->freq_des->xAxis->setBasePen(GCcpu);
    ui->freq_des->yAxis->setVisible(false);
    ui->freq_des->xAxis->setRange(1,30);
    ui->freq_des->yAxis->setRange(0,4);
    ui->freq_des->xAxis->setPadding(1);
    ui->freq_des->yAxis->setPadding(1);
    //other
    cpu_info_read=ReadInfo(c_info,{5,8,9,11,13},true);
    ui->CPU_name->setText(QString::fromStdString(cpu_info_read[0]));
    ui->freq->setText(QString::fromStdString(cpu_info_read[1])+"MHz");
    ui->cache->setText(QString::fromStdString(cpu_info_read[2]));
    ui->cores->setText(QString::number((unsigned(str2uns(cpu_info_read[3]))*unsigned(str2uns(cpu_info_read[4]))),10));
    uptime>>info_in;
    info_in.erase(info_in.find_first_of(".",0));
    ui->time->setText(QString::number(str2uns(info_in)/3600,10)
                      +":"+QString::number(str2uns(info_in)%3600/60,10)
                      +":"+QString::number(str2uns(info_in)%60,10));
    c_info.close();
    uptime.close();
}

cpustats::~cpustats()
{
    delete ui;
}
