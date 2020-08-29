#ifndef __MCN_SHARED_MEM_IF__
#define __MCN_SHARED_MEM_IF__
typedef struct _MCN_VID_FRAME_T
{
	int         width;			// In pixels
	int         height;			// In pixels
	int         stride;			// In pixels
	int         bitsperPixel;
	int         nFrameNumber;	// monotonically increasing
	int         fReady;			// Do not use it
	int         fInUse;			// Do not use it
	int         orientation;
} MCN_VID_FRAME_T;

#define VID_FRAME_SHARED_BUF_SIZE ((1920*1080*4) + sizeof(MCN_VID_FRAME_T))

#endif //__MCN_SHARED_MEM_IF__