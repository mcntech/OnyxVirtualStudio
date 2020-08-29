#ifndef _VideoExport_H
#define _VideoExport_H

#ifdef _WIN32
	#ifdef VIDEO_STATIC
		#define PluginExport
	#else
		#ifdef VIDEO_EXPORTS
			#define PluginExport __declspec(dllexport)
		#else
			#define PluginExport __declspec(dllimport)
		#endif
    #endif
#else
	#define PluginExport __attribute__ ((visibility("default")))
#endif

#endif

