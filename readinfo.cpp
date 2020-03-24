#include "readinfo.h"
using namespace std;
vector<string> ReadInfo(ifstream &info, vector<int> line, bool notail)
{
    int posi1,posi2;
    string info_get,info_store;
    vector<int> ask=line;
    vector<string> result;
    for(int counter=1;counter!=*(line.end()-1)+1;counter++)
    {
        if(counter==(*ask.begin()))   //Info needed
        {
            info_get.clear();
            getline(info,info_get);
            posi1=info_get.find(':',0);
            posi2=info_get.find_first_not_of(' ',posi1+1);
            if(notail)
                info_store=info_get.substr(posi2);
            else
            {
                posi1=info_get.find_first_of(' ',posi2);
                info_store=info_get.substr(posi2,posi1-posi2);
            }
            result.push_back(info_store);
            ask.erase(ask.begin());
        }
        else
            getline(info,info_get);
    }
    return result;
}

map<string,string> ReadInfobyName(ifstream &info, vector<string> line)
{
    string::size_type posi1,posi2;
    string info_get="1",info_store;
    vector<string> ask=line;
    map<string,string> result;  //the result must be as the same size as the ask
    vector<string>::iterator itor;
    while(!info_get.empty())
    {
        info_get.clear();
        info_store.clear();
        getline(info,info_get);
        for(itor=ask.begin();itor!=ask.end();itor++)
        {
            if(info_get.find(*itor,0)!=string::npos)   //Info needed
            {
                posi1=info_get.find(':',0);
                if((posi2=info_get.find_first_not_of('\t',posi1+1))==string::npos)
                    posi2=info_get.find_first_not_of(' ',posi1+1);
                info_store=info_get.substr(posi2);
                result[*itor]=info_store;
                ask.erase(itor);
                break;
            }
        }
    }
    for(itor=ask.begin();itor!=ask.end();itor++)
        result[*itor]="!!!";
    return result;
}

unsigned long str2uns(std::string str)
{
    unsigned long sum=0;
    string::iterator iter;
    int counter;
    for(counter=0,iter=str.end()-1;iter+1!=str.begin();iter--,counter++)
        sum+=(*iter-48)*pow(10,counter);
    return sum;
}
