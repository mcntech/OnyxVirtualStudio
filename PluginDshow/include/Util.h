#ifndef _Util_h
#define _Util_h

#include <string>
#include <vector>

#define foreach(type,lst) for (std::vector<type>::iterator it=lst.begin();it != lst.end(); it++)
#define foreach_l(type,lst) for (std::list<type>::iterator it=lst.begin();it != lst.end(); it++)
#define foreach_r(type,lst) for (std::vector<type>::reverse_iterator it=lst.rbegin();it != lst.rend(); it++)
#define foreach_in_map(type,list) for (std::map<std::string,type>::iterator it=list.begin();it != list.end(); it++)

#define th_writelog(x) VideoManager::getSingleton().logMessage(x)

std::string str(int i);
std::string strf(float i);
void _psleep(int milliseconds);
int _nextPow2(int x);

#endif