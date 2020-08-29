
#include <memory.h>
#include "VideoFrame.h"
#include "VideoClip.h"

void GenerateAlphaForChromaKey(
		unsigned long      *pulData,
		int                nWidth,
		int                nHeight,
		int                nStride,
		unsigned long      ulColorkeyValue,
		unsigned long      ulColorkeyThresh
		);

void VideoFrame::CopyToHwTextureARGB(img_plane* yuv,unsigned char* out,int destStride)
{
	unsigned char *ysrc;
	unsigned char *ydest = out;
	int nUpsideDown = (mParent->mSrcUpsidedown != mParent->mDstUpsidedown);
	if (nUpsideDown){
		ysrc = yuv->data + (yuv->height - 1 - mParent->mYStart) * yuv->stride * 4;
	} else {
		ysrc = yuv->data + mParent->mYStart * yuv->stride * 4;
	}

	int offsetLine = mParent->mXStart * 4;
	int lenLine = yuv->stride * 4;
	int lenCpy = mWidth * 4;
	for (int y = 0; y < mHeight; y += 1) 	{
		memcpy(ydest, ysrc + offsetLine, lenCpy);
		ydest += destStride * 4;
		if (nUpsideDown){			
			ysrc -= lenLine;
		} else {
			ysrc += lenLine;
		}
	}
}



// --------------------------------------------------------------
VideoFrame::VideoFrame(VideoClip* parent)
{
	mReady=mInUse=false;
	mParent=parent;
	mIteration=0;

	mWidth = mParent->getCropWidth();
	mStride = mParent->getStride();
	mHeight = mParent->getCropHeight();

	int destSize = mStride * mHeight * 4;
	int size = mStride * mHeight * 4;
	// Intermediate buffer. Should be same as dest size
	mBuffer=new unsigned char[destSize];

	mBufferYuv.width = mParent->getWidth();
	mBufferYuv.height = mParent->getHeight();
	mBufferYuv.stride = mParent->getWidth();	// Get it form DS
	int srcSize = mBufferYuv.stride * mBufferYuv.height * 4;
	// Intermediate buffer. Should be same as source size
	mBufferYuv.data = new unsigned char[srcSize];
	memset(mBufferYuv.data,255,srcSize);
	memset(mBuffer,0x00,destSize);
}

VideoFrame::~VideoFrame()
{
	if (mBuffer) delete [] mBuffer;
	if (mBufferYuv.data) delete [] mBufferYuv.data;
}

int VideoFrame::getWidth()
{
	return mWidth;
}

int VideoFrame::getStride()
{
	return mStride;
}

int VideoFrame::getHeight()
{
	return mHeight;
}

unsigned char* VideoFrame::getBuffer()
{
	return mBuffer;
}

img_plane VideoFrame::getBufferYuv()
{
	return mBufferYuv;
}

void VideoFrame::decode(void* yuv)
{
	img_plane* pImg = (img_plane*)yuv;
	if(mParent->mColorKeyEnable) {
		GenerateAlphaForChromaKey((unsigned long *)pImg->data, pImg->width, pImg->height,  pImg->stride, mParent->mColorKeyRatio, mParent->mColorKeyThresh);
	}
	CopyToHwTextureARGB((img_plane*) yuv,mBuffer,mParent->mStride);
	mReady=true;
}

void VideoFrame::clear()
{
	mInUse=mReady=false;
}

typedef struct _PIX_ARGB_T
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
} PIX_ARGB_T;

void GenerateAlphaForChromaKey(
		unsigned long      *pulData,
		int                nWidth,
		int                nHeight,
		int                nStride,
		unsigned long      ulColorkeyRatio,
		unsigned long      ulColorkeyThresh
		) 
 { 
	using namespace Ogre; 
	using std::fabs; 
	
	//PIX_ARGB_T *pColorKey = (PIX_ARGB_T *)&ulColorkeyValue;
	PIX_ARGB_T *pThreshold = (PIX_ARGB_T *)&ulColorkeyThresh;
	unsigned char threshG = pThreshold->g;
	//double ratioG =  (double)pColorKey->g / ((double)pColorKey->r + (double)pColorKey->g + (double)pColorKey->b);
	double ratioG = (double)ulColorkeyRatio / (double)100;

	for(int j = 0; j < nHeight; j++)  { 
		for(int i = 0; i < nWidth; i++)  { 
			int nIndx = j * nStride + i;
			PIX_ARGB_T *Pix =  (PIX_ARGB_T *)(pulData + nIndx);

			double sum = Pix->r + Pix->g + Pix->b;
			Pix->a =  (Pix->g > threshG && Pix->g > (sum * ratioG)) ? 0 : 0xFF;
		} 
	}
}
