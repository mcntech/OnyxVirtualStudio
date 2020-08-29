#ifndef __PLUGIN_UTIL_H__
#define __PLUGIN_UTIL_H__

#include <string>
#include <vector>

#define foreach(type,lst) for (std::vector<type>::iterator it=lst.begin();it != lst.end(); it++)
#define foreach_l(type,lst) for (std::list<type>::iterator it=lst.begin();it != lst.end(); it++)
#define foreach_r(type,lst) for (std::vector<type>::reverse_iterator it=lst.rbegin();it != lst.rend(); it++)
#define foreach_in_map(type,list) for (std::map<std::string,type>::iterator it=list.begin();it != list.end(); it++)

class PluginUtil
{
public:
	static int nextPow2(int x)
	{
		int y;
		for (y=1;y<x;y*=2);
		return y;
	}
};

#endif // __PLUGIN_UTIL_H__