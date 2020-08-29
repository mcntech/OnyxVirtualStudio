#ifndef __CHROMA_KEY_H__
#define __CHROMA_KEY_H__

class CChromaKey
{
public:
typedef struct _PIX_ARGB_T
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
} PIX_ARGB_T;

static void GenerateAlphaForChromaKey(
		unsigned long      *pulData,
		int                nWidth,
		int                nHeight,
		int                nStride,
		unsigned long      ulColorkeyRatio,
		unsigned long      ulColorkeyThresh
		) 
 { 
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

};

#endif // __CHROMA_KEY_H__