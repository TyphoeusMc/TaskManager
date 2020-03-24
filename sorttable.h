#ifndef SORTTABLE_H
#define SORTTABLE_H
#include <QTableWidgetItem>
#include <string>
#include "readinfo.h"

class SortTable:public QTableWidgetItem
{
public:
    SortTable();
    ~SortTable();
    bool operator<(const QTreeWidgetItem & other) const;
};

#endif // SORTTABLE_H
