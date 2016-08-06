#ifndef __CIMAGE_H__
#define __CIMAGE_H__

#include <string>

#define BITMAP_ID 0x4D42

struct TGAInformationHeader
{
	unsigned char m_ucHeader[6];
	unsigned int  m_uiBytesPerPixel;
	unsigned int  m_uiImageSize;
	unsigned int  m_uiTemp;
	unsigned int  m_uiType;
	unsigned int  m_uiHeight;
	unsigned int  m_uiWidth;
	unsigned int  m_uiBPP;
};

#pragma pack( push, 1 )

struct BMPFileHeader
{
	unsigned short usType;
	unsigned int uiSize;
	unsigned short usReserved1;
	unsigned short usReserved2;
	unsigned int uiOffBits;
};

#pragma pack( pop )

struct BMPInfoHeader
{
	unsigned int  uiSize;
	long   lWidth;
	long   lHeight;
	unsigned short usPlanes;
	unsigned short usBitCount;
	unsigned int uiCompression;
	unsigned int uiSizeImage;
	long lXPelsPerMeter;
	long lYPelsPerMeter;
	unsigned int uiClrUsed;
	unsigned int uiClrImportant;
};


class CImage
{

public:
	CImage();
	~CImage();

	//根据长度，宽度，深度，创建image
	bool create(unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP);

	//加载图片数据
	bool loadData(std::string fileName);

	//加载一个图片
	bool load(std::string fileName, float fMinFilter, float fMaxFilter, bool bMipmap = false);

	//释放加载的图片
	void unload();  

	//保存当前图片
	bool save(std::string fileName);


	unsigned int getPitch();

	//获取某个像素的RGB值
	inline void getColor(unsigned int x, unsigned int y,
		unsigned char *pRed, unsigned char *pGreen, unsigned char *pBlue)
	{
		//图片的像素字节数
		unsigned int uiBPP = m_uiBPP / 8;

		//满足条件，获取像素值
		if (x < m_uiWidth && y < m_uiHeight)
		{
			*pRed = m_ucpData[((y * m_uiHeight) + x) * uiBPP];
			*pGreen = m_ucpData[((y * m_uiHeight) + x) * uiBPP + 1];
			*pBlue = m_ucpData[((y * m_uiHeight) + x) * uiBPP + 2];
		}
	}


	//设置某点的颜色
	inline void setColor(unsigned int x, unsigned int y,
		unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
	{
		unsigned int uiBPP = m_uiBPP / 8;

		if (x < m_uiWidth && y < m_uiHeight)
		{
			m_ucpData[((y * m_uiHeight) + x) * uiBPP] = ucRed;
			m_ucpData[((y * m_uiHeight) + x) * uiBPP + 1] = ucGreen;
			m_ucpData[((y * m_uiHeight) + x) * uiBPP + 2] = ucBlue;
		}
	}

	inline unsigned char *getData()
	{
		return m_ucpData;
	}

	inline unsigned int getWidth()
	{
		return m_uiWidth;
	}

	inline unsigned int getHeight()
	{
		return m_uiHeight;
	}

	inline unsigned int getBPP()
	{
		return m_uiBPP;
	}

	inline unsigned int getID()
	{
		return m_ID;
	}

	inline void setID(unsigned int id)
	{
		m_ID = id;
	}

	inline bool isLoad()
	{
		return m_bIsLoaded;
	}

private:
	unsigned char *m_ucpData;  //图片的数据
	unsigned int m_uiWidth;  //宽度
	unsigned int m_uiHeight;  //高度
	unsigned int m_uiBPP;  //深度
	unsigned int m_ID; //id号

	unsigned int m_pitch;

	bool m_bIsLoaded;

	bool loadBMP();
	bool saveBMP(std::string saveFileName);

	bool loadCompressedTGA(std::string fileName);
	bool loadUncompressedTGA(std::string fileName);




};
#endif