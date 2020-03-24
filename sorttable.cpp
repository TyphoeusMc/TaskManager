#include "sorttable.h"

SortTable::SortTable()
{

}

SortTable::~SortTable(){}

bool SortTable::operator<(const QTableWidgetItem & other) const
{
    unsigned pos1,pos2;
    std::string a=this->text().toStdString();
    std::string b=other.text().toStdString();
    pos=a.find('.',0);
    pos2=b.find('.',0);
    unsigned aa=str2uns(a.substr(0,pos1-1));
    unsigned bb=str2uns(b.substr(0,pos2-1));
    return (aa < bb);
}
