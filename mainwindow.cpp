#include "mainwindow.h"
#include "ui_mainwindow.h"

int INTERVAL=500;
//sysinfo
using namespace std;
ifstream meminfo,cpuinfo,diskinfo,etc,ver,cpustat;
ifstream proc_stat,proc_status;
//plot
vector <double> CPU_info(7,0);
vector <double> CPU_info_old(7,0);
unsigned long cpu_total2,cpu_total1;
unsigned long disk_iotime=0,disk_iotime_old=0;
unsigned long ram_util;
QVector <double> t_util(10,0);      //Store cpu utilization value for the graph
QVector <double> r_util(10,0);
QVector <double> d_util(10,0);
QVector <double> x_axis={1,2,3,4,5,6,7,8,9,10};
vector <int> ram_read_line={1,2,4,5};
vector <string> ram_read;
QBrush BGC_cpu(QColor(219,242,255));
QBrush BGC_ram(QColor(255,235,201));
QBrush BGC_disk(QColor(211,255,191));
QPen GC_cpu(QColor(39,170,226),1);
QPen GC_ram(QColor(245,121,0),1);
QPen GC_disk(QColor(115,210,22),1);
string info;
//MdiArea
QMdiSubWindow *cpu_sub;
QMdiSubWindow *ram_sub;
QMdiSubWindow *disk_sub;
//ProcessManage
map <string,string> proc_read;
unsigned processnum=0,threads=0;
map <string,unsigned long> pid_old_cpu;
unsigned pid_cur;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("资源管理");
    //***********************Sys_info_part***********************//
    GetSysInfo();
    PlotOutlookInit();
    cpuwindow=new cpustats;
    ramwindow=new ramstats;
    diskwindow=new diskstats;
    procpath=new InputExec;
    procpath->setModal(true);
    cpu_sub=ui->mdiArea->addSubWindow(cpuwindow);
    ram_sub=ui->mdiArea->addSubWindow(ramwindow);
    disk_sub=ui->mdiArea->addSubWindow(diskwindow);

    connect(ui->cpu_u_plot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(CPU_click()));
    connect(ui->ram_u_plot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(RAM_click()));
    connect(ui->disk_u_plot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(DISK_click()));
    //Performance info initialize
    ui->mdiArea->setActiveSubWindow(disk_sub);
    ui->mdiArea->activeSubWindow()->setWindowFlags(Qt::FramelessWindowHint);
    ui->mdiArea->activeSubWindow()->showMaximized();
    ui->mdiArea->setActiveSubWindow(ram_sub);
    ui->mdiArea->activeSubWindow()->setWindowFlags(Qt::FramelessWindowHint);
    ui->mdiArea->activeSubWindow()->showMaximized();
    ui->mdiArea->setActiveSubWindow(cpu_sub);
    ui->mdiArea->activeSubWindow()->setWindowFlags(Qt::FramelessWindowHint);
    ui->mdiArea->activeSubWindow()->showMaximized();
    //Process table initialize
    QFont TableHeader=ui->tableWidget->font();
    TableHeader.setBold(true);
    ui->tableWidget->setFont(TableHeader);
    InitializeProcessTable();

    //Timer and menu action initialize
    timer=new QTimer(this);
    timer->setInterval(INTERVAL);
    connect(timer,SIGNAL(timeout()),this,SLOT(UpdatePlotOutlook()));
    connect(ui->CloseWindow,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->action0_5s_2,&QAction::triggered,[=](){timer->setInterval(500);
                                                       cpuwindow->timer->setInterval(500);
                                                       ramwindow->timer->setInterval((500));});  //use lambda expression to pass the time
    connect(ui->action1s_2,&QAction::triggered,[=](){timer->setInterval(1000);
                                                     cpuwindow->timer->setInterval(1000);
                                                     ramwindow->timer->setInterval((1000));});
    connect(ui->action2s_2,&QAction::triggered,[=](){timer->setInterval(2000);
                                                     cpuwindow->timer->setInterval(2000);
                                                     ramwindow->timer->setInterval((2000));});
    connect(ui->action_4,SIGNAL(triggered(bool)),this,SLOT(ShutDown()));
    connect(timer,SIGNAL(timeout()),this,SLOT(InitializeProcessTable()));
    connect(this,SIGNAL(PT_Update(uint,uint)),cpuwindow,SLOT(UpdatePT(uint,uint)));
    connect(ui->kill,SIGNAL(pressed()),this,SLOT(KillProcess()));
    connect(ui->action,SIGNAL(triggered(bool)),this,SLOT(GetProcessPath()));
    connect(procpath,SIGNAL(path(QString)),this,SLOT(AcceptPath(QString)));
    timer->start();
}

//SLOTS to control sub-windows
void MainWindow::CPU_click()
{
    ui->mdiArea->setActiveSubWindow(cpu_sub);
}

void MainWindow::RAM_click()
{
    ui->mdiArea->setActiveSubWindow(ram_sub);
}

void MainWindow::DISK_click()
{
    ui->mdiArea->setActiveSubWindow(disk_sub);
}

void MainWindow::UpdatePlotOutlook()
{
    meminfo.open("/proc/meminfo");
    cpustat.open("/proc/stat");
    diskinfo.open("/proc/diskstats");
    //Draw cpu graph
    cpustat>>info;
    for(int counter=0; counter<=6; counter++)
    {
        cpustat>>info;
        CPU_info.push_back(str2uns(info));
        info.clear();
    }
    cpu_total2=CPU_info[0]+CPU_info[1]+CPU_info[2]+CPU_info[3]+CPU_info[4]+CPU_info[5]+CPU_info[6];
    cpu_total1=CPU_info_old[0]+CPU_info_old[1]+CPU_info_old[2]+CPU_info_old[3]+CPU_info_old[4]+CPU_info_old[5]+CPU_info_old[6];
    for(int counter=0;counter<=8;counter++)
        t_util[counter]=t_util[counter+1];
    t_util[9]=100*double(((CPU_info[0]-CPU_info_old[0])+(CPU_info[2]-CPU_info_old[2])))/(cpu_total2-cpu_total1);
    ui->cpu_u->setText(QString::number(t_util[9],'f',2)+"%");
    ui->cpu_u_plot->graph(0)->setData(x_axis,t_util);
    ui->cpu_u_plot->replot();
    CPU_info_old.clear();
    CPU_info_old=CPU_info;
    CPU_info.clear();
    //Draw ram graph
    ram_read=ReadInfo(meminfo,ram_read_line,false);
    for(int counter=0;counter<=8;counter++)
        r_util[counter]=r_util[counter+1];
    r_util[9]=100*double(str2uns(ram_read[0])-str2uns(ram_read[1])-str2uns(ram_read[2])-str2uns(ram_read[3]))/str2uns(ram_read[0]);
    ui->ram_u->setText(QString::number(r_util[9],'f',2)+"%");
    ui->ram_u_plot->graph(0)->setData(x_axis,r_util);
    ui->ram_u_plot->replot();
    //Draw disk graph
    for(int counter=0; counter<=8; counter++)
    {
        getline(diskinfo,info);
        info.clear();
    }
    for(int counter=0; counter<=11; counter++)
    {
        diskinfo>>info;
        info.clear();
    }
    diskinfo>>info;
    //cout<<info<<endl;
    for(int counter=0;counter<=8;counter++)
        d_util[counter]=d_util[counter+1];
    disk_iotime=str2uns(info);
    d_util[9]=100*double(disk_iotime-disk_iotime_old)/INTERVAL;
    disk_iotime_old=disk_iotime;
    ui->disk_u->setText(QString::number(d_util[9],'f',2)+"%");
    ui->disk_u_plot->graph(0)->setData(x_axis,d_util);
    ui->disk_u_plot->replot();
    meminfo.close();
    cpustat.close();
    diskinfo.close();
}

void MainWindow::PlotOutlookInit()
{
    //initialize label
    ui->label_cpu_u->setText("CPU:");
    ui->label_ram_u->setText("内存:");
    ui->label_disk_u->setText("磁盘活动时间:");
    ui->cpu_u->setText("0%");
    ui->ram_u->setText("0%");
    ui->disk_u->setText("0%");
    //initialize plots
    ui->cpu_u_plot->addGraph();
    ui->cpu_u_plot->graph(0)->setData(x_axis,t_util);
    ui->ram_u_plot->addGraph();
    ui->ram_u_plot->graph(0)->setData(x_axis,r_util);
    ui->disk_u_plot->addGraph();
    ui->disk_u_plot->graph(0)->setData(x_axis,d_util);
    ui->cpu_u_plot->setBackground(BGC_cpu);
    ui->ram_u_plot->setBackground(BGC_ram);
    ui->disk_u_plot->setBackground(BGC_disk);
    ui->cpu_u_plot->graph(0)->setPen(GC_cpu);
    ui->ram_u_plot->graph(0)->setPen(GC_ram);
    ui->disk_u_plot->graph(0)->setPen(GC_disk);

    ui->cpu_u_plot->xAxis->setTicks(false);
    ui->cpu_u_plot->xAxis->grid()->setVisible(false);
    ui->cpu_u_plot->xAxis->setBasePen(GC_cpu);
    ui->cpu_u_plot->yAxis->setVisible(false);
    ui->cpu_u_plot->xAxis->setRange(1,10);
    ui->cpu_u_plot->yAxis->setRange(0,100);

    ui->ram_u_plot->xAxis->setTicks(false);
    ui->ram_u_plot->xAxis->grid()->setVisible(false);
    ui->ram_u_plot->xAxis->setBasePen(GC_ram);
    ui->ram_u_plot->yAxis->setVisible(false);
    ui->ram_u_plot->xAxis->setRange(1,10);
    ui->ram_u_plot->yAxis->setRange(0,100);

    ui->disk_u_plot->xAxis->setTicks(false);
    ui->disk_u_plot->xAxis->grid()->setVisible(false);
    ui->disk_u_plot->xAxis->setBasePen(GC_disk);
    ui->disk_u_plot->yAxis->setVisible(false);
    ui->disk_u_plot->xAxis->setRange(1,10);
    ui->disk_u_plot->yAxis->setRange(0,100);
}

void MainWindow::KillProcess()
{
    pid_t pid=str2uns(ui->tableWidget->item(ui->tableWidget->currentRow(),4)->text().toStdString());
    if(kill(pid,SIGKILL)==-1)
        QMessageBox::warning(NULL,"Oops!","中止进程出错,您可能没有足够的权限",QMessageBox::Yes);
}

void MainWindow::InitializeProcessTable()
{
    processnum=0;
    threads=0;
    int row=0,column=0;
    int counter;
    bool ispid=true;
    DIR *catalog_open=opendir("/proc");
    QString user_name;
    string read_proc_time,pid;
    dirent *dirinfo;
    unsigned long utilization=0,thread_num=0;
    double util_per=0,mem_use=0;
    struct stat processInfo;
    bool CREATED;
    while(NULL!=(dirinfo=readdir(catalog_open)))
    {
        for(counter=0;dirinfo->d_name[counter]!='\0';counter++)
        {
            if(!isdigit(dirinfo->d_name[counter]))
                ispid=false;
        }
        if(ispid)  //if current file is a process file, fill the table
        {
            CREATED=true;
            processnum++;
            if(processnum+1>unsigned(ui->tableWidget->rowCount())) //this is crucial, if not, the program will corrupt
                ui->tableWidget->setRowCount(processnum);
            //processtimeinitial
            pid=string(dirinfo->d_name);
            if(pid_old_cpu.find(pid)==pid_old_cpu.end())
                CREATED=false;
            proc_stat.open("/proc/"+pid+"/stat");
            if(proc_stat.is_open()==false)
                break;
            while(read_proc_time.find(')',0)==string::npos)
                proc_stat>>read_proc_time;
            for(counter=0;counter<=11;counter++)
                proc_stat>>read_proc_time;
            for(counter=0;counter<=3;counter++)
            {
                proc_stat>>read_proc_time;
                utilization+=str2uns(read_proc_time);
            }
            //名称
            proc_status.open("/proc/"+pid+"/status");
            if(proc_status.is_open()==false)
                break;
            proc_read=ReadInfobyName(proc_status,{"Name","Thread"});
            if(!CREATED)
                ui->tableWidget->setItem(row,column++,new QTableWidgetItem(QString::fromStdString(proc_read["Name"])));
            else
                ui->tableWidget->item(row,column++)->setText(QString::fromStdString(proc_read["Name"]));
            //CPU使用
            if(!CREATED)
            {

                ui->tableWidget->setItem(row,column,new QTableWidgetItem(QString("X")));
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 241, 225));
                pid_old_cpu[pid]=utilization;
            }
            else
            {
                util_per=100*double(utilization-pid_old_cpu[pid])/(cpu_total2-cpu_total1);
                pid_old_cpu[pid]=utilization;
                utilization=0;
                ui->tableWidget->item(row,column)->setText(QString::number(util_per,'f',2)+"%");
                if(util_per>0&&util_per<=5)
                    ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 208, 141));
                else if(util_per>5)
                    ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 170, 70));
                else
                    ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 241, 225));
            }
            ui->tableWidget->item(row,column++)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //RAM使用
            for(counter=0;counter<=5;counter++)
                proc_stat>>read_proc_time;
            mem_use=double(str2uns(read_proc_time)*4)/1024;
            if(!CREATED)
                ui->tableWidget->setItem(row,column,new QTableWidgetItem(QString::number(mem_use,'f',2)+"MB"));
            else
                ui->tableWidget->item(row,column)->setText(QString::number(mem_use,'f',2)+"MB");
            if(mem_use==0)
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 241, 225));
            else if(0<mem_use&&mem_use<=100)
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 208, 141));
            else
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 170, 70));
            ui->tableWidget->item(row,column++)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //线程
            if(!CREATED)
                ui->tableWidget->setItem(row,column,new QTableWidgetItem(QString::fromStdString(proc_read["Thread"])));
            else
                ui->tableWidget->item(row,column)->setText(QString::fromStdString(proc_read["Thread"]));
            thread_num=str2uns(proc_read["Thread"]);
            threads+=thread_num;
            if(thread_num==1)
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 241, 225));
            else if(thread_num>1&&thread_num<=5)
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 208, 141));
            else
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 170, 70));
            ui->tableWidget->item(row,column++)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //pid
            if(!CREATED)
                ui->tableWidget->setItem(row,column,new QTableWidgetItem(QString(dirinfo->d_name)));
            else
                ui->tableWidget->item(row,column)->setText(QString(dirinfo->d_name));
            ui->tableWidget->item(row,column++)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //创建时间
            string path="/proc/"+pid;
            lstat(path.c_str(),&processInfo);
            struct tm *proc_time;
            proc_time=localtime(&processInfo.st_mtim.tv_sec);
            if(!CREATED)
                ui->tableWidget->setItem(row,column,new QTableWidgetItem(QString::number(proc_time->tm_hour,10)+":"+QString::number(proc_time->tm_min,10)));
            else
                ui->tableWidget->item(row,column)->setText(QString::number(proc_time->tm_hour,10)+":"+QString::number(proc_time->tm_min,10));
            ui->tableWidget->item(row,column++)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            //用户组
            user_name=QString(getpwuid(processInfo.st_uid)->pw_name);
            if(!CREATED)
                ui->tableWidget->setItem(row,column,new QTableWidgetItem(user_name));
            else
                ui->tableWidget->item(row,column)->setText(user_name);
            if(user_name=="root")
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 208, 141));
            else
                ui->tableWidget->item(row,column)->setBackgroundColor(QColor(255, 241, 225));
            ui->tableWidget->item(row++,column)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            user_name.clear();
            column=0;
            proc_read.clear();
            proc_status.close();
            proc_stat.close();
        }
        ispid=true;
    }
    ui->tableWidget->setRowCount(processnum);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->setShowGrid(false);
    emit PT_Update(processnum,threads);
    closedir(catalog_open);
}

void MainWindow::ShutDown()
{
    execv("/home/typhoeus/OS_Design/OS_Design_T4/Manager/shutdown",NULL);
}

void MainWindow::GetSysInfo()
{
    meminfo.open("/proc/meminfo");
    cpuinfo.open("/proc/cpuinfo");
    etc.open("/etc/issue");
    ver.open("/proc/version");
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
    cpu_info_read=ReadInfo(cpuinfo,cpu_info_line,true);
    ui->label_cpu_model->setText(QString::fromStdString(cpu_info_read[0]));
    ui->label_CT->setText(QString::fromStdString(cpu_info_read[1]+" core(s) "+cpu_info_read[2]+" thread(s) "));
    getline(meminfo,info_get);
    posi1=info_get.find_first_of(' ',0);
    posi2=info_get.find_first_not_of(' ',posi1);
    posi1=info_get.find_first_of(' ',posi2);
    mem_mb=str2uns(info_get.substr(posi2,posi1-posi2))/1024;
    ui->label_ram->setText(QString::number(mem_mb)+"MB");
    meminfo.close();
    cpuinfo.close();
    etc.close();
    ver.close();
}

void MainWindow::GetProcessPath()
{
    procpath->show();
}

void MainWindow::AcceptPath(QString path)
{
    QProcess newproc;
    if(newproc.startDetached(path)==false)
        QMessageBox::warning(NULL,"Oops!","开启任务失败,请检查路径:"+path+"是否正确.");
}

MainWindow::~MainWindow()
{
    delete ui;
}
