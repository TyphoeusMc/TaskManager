#ifndef DISKSTATS_H
#define DISKSTATS_H

#include <QWidget>

namespace Ui {
class diskstats;
}

class diskstats : public QWidget
{
    Q_OBJECT

public:
    explicit diskstats(QWidget *parent = 0);
    ~diskstats();

private:
    Ui::diskstats *ui;
};

#endif // DISKSTATS_H
