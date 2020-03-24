#ifndef RAMSTATS_H
#define RAMSTATS_H

#include <QWidget>
#include "qcustomplot.h"
#include <QTimer>
#include "readinfo.h"
#include <QVector>

namespace Ui {
class ramstats;
}

class ramstats : public QWidget
{
    Q_OBJECT

public:
    explicit ramstats(QWidget *parent = 0);
    void InitializeWindow();
    QTimer *timer;
    ~ramstats();

public slots:
    void UpdateWindow();

private:
    Ui::ramstats *ui;
};

#endif // RAMSTATS_H
