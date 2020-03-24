#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCodec>
#include <QVector>
#include <QTimer>
#include <QMessageBox>
#include <unistd.h>
#include "qcustomplot.h"
//#include "clickablewidget.h"
#include "cpustats.h"
#include "ramstats.h"
#include "diskstats.h"
#include "readinfo.h"
#include "inputexec.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cctype>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string>
#include <map>
#include <errno.h>
#include <signal.h>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void GetSysInfo();
    void PlotOutlookInit();
    //void InitializeProcessTable();
    ~MainWindow();

public slots:
    void UpdatePlotOutlook();
    void RAM_click();
    void DISK_click();
    void CPU_click();
    void ShutDown();
    //void UpdateProcessTable();
    void InitializeProcessTable();
    void KillProcess();
    void GetProcessPath();
    void AcceptPath(QString path);

signals:
    void SendData(int value);
    void PT_Update(unsigned proc,unsigned threads);


private:
    Ui::MainWindow *ui;
    QTimer *timer;
    cpustats *cpuwindow;
    ramstats *ramwindow;
    diskstats *diskwindow;
    InputExec *procpath;
};

#endif // MAINWINDOW_H
