#ifndef INPUTEXEC_H
#define INPUTEXEC_H

#include <QDialog>
#include <QString>

using namespace std;

namespace Ui {
class InputExec;
}

class InputExec : public QDialog
{
    Q_OBJECT

public:
    explicit InputExec(QWidget *parent = 0);
    ~InputExec();

public slots:
    void ExeNewProcess();

signals:
    void path(QString p);

private:
    Ui::InputExec *ui;
};

#endif // INPUTEXEC_H
