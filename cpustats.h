#ifndef CPUSTATS_H
#define CPUSTATS_H

#include <QWidget>
#include "qcustomplot.h"
#include <QTimer>
#include "readinfo.h"
#include <QVector>

namespace Ui {
class cpustats;
}

class cpustats : public QWidget
{
    Q_OBJECT

public:
    explicit cpustats(QWidget *parent = 0);
    void InitializeWindow();
    ~cpustats();
    QTimer *timer;

public slots:
    void UpdateWindow();
    void UpdatePT(unsigned proc,unsigned thread);

private:
    Ui::cpustats *ui;
};

#endif // CPUSTATS_H
