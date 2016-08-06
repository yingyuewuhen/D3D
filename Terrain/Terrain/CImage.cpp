#include "CImage.h"
#include <fstream>

using namespace std;


unsigned char g_ucUTGAcompare[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char g_ucCTGAcompare[12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


CImage::CImage()
{

}

CImage::~CImage()
{

}


//创建一个图片内存区
bool CImage::create(unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP)
{
	m_uiHeight = uiHeight;
	m_uiWidth = uiWidth;
	m_uiBPP = uiBPP;

	m_pitch = m_uiWidth * (m_uiBPP / 8);

	//如果已经存在图片，释放当前图片
// 	if (m_ucpData)
// 		unload();

	m_ucpData = new unsigned char[uiHeight * uiWidth * (uiBPP / 8)];
	if (m_ucpData == 0)
		return false;
	//图片已经创建
	m_bIsLoaded = true;
	return true;
}


//从文件中加载数据
bool CImage::loadData(std::string fileName)
{
	ifstream inFIle(fileName, ios::binary);

	//打开文件失败
	if (!inFIle.good())
		return false;

	//计算文件大小
	inFIle.seekg(0, inFIle.end);
	int fileLength = static_cast<int>(inFIle.tellg());
	inFIle.seekg(0, inFIle.beg);

	//分配内存
	m_ucpData = new unsigned char[fileLength];
	if (m_ucpData == 0)
	{
		inFIle.close();
		return false;
	}

	//读取数据
	inFIle.read((char *)m_ucpData, fileLength);
	if (!inFIle)  //读取失败
	{
		if (m_ucpData)
		{
			delete[]m_ucpData;
			m_ucpData = 0;
		}
		return false;
	}

	inFIle.close();

	//判断文件的类型
	if (memcmp(m_ucpData, "BM", 2) == 0)  //BMP文件
	{
		if (!loadBMP())
		{
			if (m_ucpData)
			{
				delete[] m_ucpData;
				m_ucpData = 0;
			}
			return false;
		}
	}
	else if (memcmp(m_ucpData, g_ucUTGAcompare, 12) == 0)
	{
		if (!loadUncompressedTGA(fileName))
		{
			if (m_ucpData)
			{
				delete[] m_ucpData;
				m_ucpData = 0;
			}
			return false;
		}
	}
	else if (memcmp(m_ucpData, g_ucCTGAcompare, 12) == 0)
	{
		if (!loadCompressedTGA(fileName))
		{
			if (m_ucpData)
			{
				delete[] m_ucpData;
				m_ucpData = 0;
			}
			return false;
		}
	}
	else
	{
		if (m_ucpData)
		{
			delete[] m_ucpData;
			m_ucpData = 0;
		}
		return false;
	}
	m_bIsLoaded = true;
	return true;
}


bool CImage::load(std::string fileName, float fMinFilter, float fMaxFilter, bool bMipmap /*= false*/)
{
	return false;
}

void CImage::unload()
{
	if (m_ucpData)
	{
		delete[]m_ucpData;
		m_ucpData = 0;
		m_uiBPP = 0;
		m_uiHeight = 0;
		m_uiWidth = 0;

		m_bIsLoaded = false;
	}
}


//将当前的图片数据保存
bool CImage::save(std::string fileName)
{
	if (!saveBMP(fileName))
	{
		return false;
	}
	return true;
}

bool CImage::loadBMP()
{
	BMPFileHeader* fileHeader;
	BMPInfoHeader* infoHeader;
	unsigned int iImageSize;
	unsigned char uiTempRGB;
	unsigned int i;
	unsigned char* ucpFile = m_ucpData;

	//load in the file header
	fileHeader = (BMPFileHeader*)ucpFile;

	//advance the buffer, and load in the file information header
	ucpFile += sizeof(BMPFileHeader);
	infoHeader = (BMPInfoHeader*)ucpFile;

	//advance the buffer to load int he actual image data
	ucpFile += sizeof(BMPInfoHeader);
	ucpFile += fileHeader->uiOffBits;

	//initialize the image memory
	iImageSize = fileHeader->uiSize - fileHeader->uiOffBits;
	create(infoHeader->lWidth, infoHeader->lWidth, infoHeader->usBitCount);

	//copy the data to the class's data buffer
	memcpy(m_ucpData, ucpFile, iImageSize);

	//swap the R and B values to get RGB since the bitmap color format is in BGR
	for (i = 0; i < infoHeader->uiSizeImage; i += 3)
	{
		uiTempRGB = m_ucpData[i];
		m_ucpData[i] = m_ucpData[i + 2];
		m_ucpData[i + 2] = uiTempRGB;
	}

	//the BMP has been successfully loaded
	return true;
}

bool CImage::saveBMP(std::string saveFileName)
{
	FILE*		   pFile;
	BMPFileHeader bitmapFileHeader;
	BMPInfoHeader bitmapInfoHeader;
	unsigned char  ucTempRGB;
	unsigned int   i;

	//open a file that we can write to
	fopen_s(&pFile, saveFileName.c_str(), "wb");
	if (!pFile)
	{
		return false;
	}

	//define the BMP file header
	bitmapFileHeader.uiSize = m_uiWidth*m_uiHeight * 3 + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
	bitmapFileHeader.usType = BITMAP_ID;
	bitmapFileHeader.usReserved1 = 0;
	bitmapFileHeader.usReserved2 = 0;
	bitmapFileHeader.uiOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

	//write the .bmp file header to the file
	fwrite(&bitmapFileHeader, 1, sizeof(BMPFileHeader), pFile);

	//define the BMP information header
	bitmapInfoHeader.uiSize = sizeof(BMPInfoHeader);
	bitmapInfoHeader.usPlanes = 1;
	bitmapInfoHeader.usBitCount = 24;
	bitmapInfoHeader.uiCompression = 0;
	bitmapInfoHeader.uiSizeImage = m_uiWidth*m_uiHeight * 3;
	bitmapInfoHeader.lXPelsPerMeter = 0;
	bitmapInfoHeader.lYPelsPerMeter = 0;
	bitmapInfoHeader.uiClrUsed = 0;
	bitmapInfoHeader.uiClrImportant = 0;
	bitmapInfoHeader.lWidth = m_uiWidth;
	bitmapInfoHeader.lHeight = m_uiHeight;

	//write the .bmp file information header to the file
	fwrite(&bitmapInfoHeader, 1, sizeof(BMPInfoHeader), pFile);

	//swap the image bit order (RGB to BGR)
	for (i = 0; i < bitmapInfoHeader.uiSizeImage; i += 3)
	{
		ucTempRGB = m_ucpData[i];
		m_ucpData[i] = m_ucpData[i + 2];
		m_ucpData[i + 2] = ucTempRGB;
	}

	//now write the actual image data
	fwrite(m_ucpData, 1, bitmapInfoHeader.uiSizeImage, pFile);

	//the file has been successfully saved
	fclose(pFile);
	return true;
}

bool CImage::loadCompressedTGA(std::string fileName)
{
	TGAInformationHeader pTGAinfo;
	unsigned char* ucpFile;
	unsigned char* ucpColorBuffer;
	unsigned char  ucChunkHeader;
	unsigned int   uiPixelCount;
	unsigned int   uiCurrentPixel;
	unsigned int   uiCurrentByte;
	short		   i;

	ucpFile = m_ucpData;

	//skip past the image header
	ucpFile += 12;

	memcpy(pTGAinfo.m_ucHeader, ucpFile, sizeof(unsigned char[6]));

	//allocate memory for the image data buffer
	create(pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0],
		pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2],
		pTGAinfo.m_ucHeader[4]);

	//set the class's member variables
	pTGAinfo.m_uiBPP = m_uiBPP;
	pTGAinfo.m_uiHeight = m_uiHeight;
	pTGAinfo.m_uiWidth = m_uiWidth;

	//advance the file buffer
	ucpFile += 6;

	//make sure that the image's dimensions are supported by this loaded
	if ((m_uiWidth <= 0) || (m_uiHeight <= 0) || ((m_uiBPP != 24) && (m_uiBPP != 32)))
		return false;

	pTGAinfo.m_uiBytesPerPixel = pTGAinfo.m_uiBPP / 8;
	pTGAinfo.m_uiImageSize = (pTGAinfo.m_uiBytesPerPixel*pTGAinfo.m_uiWidth*pTGAinfo.m_uiHeight);

	uiPixelCount = pTGAinfo.m_uiHeight * pTGAinfo.m_uiWidth;
	uiCurrentPixel = 0;
	uiCurrentByte = 0;
	ucpColorBuffer = new unsigned char[pTGAinfo.m_uiBytesPerPixel];

	do
	{
		ucChunkHeader = 0;

		ucChunkHeader = *(unsigned char*)ucpFile;
		ucpFile++;

		if (ucChunkHeader < 128)
		{
			ucChunkHeader++;

			//read RAW color values
			for (i = 0; i < ucChunkHeader; i++)
			{
				memcpy(ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel);
				ucpFile += pTGAinfo.m_uiBytesPerPixel;

				//flip R and B color values around
				m_ucpData[uiCurrentByte] = ucpColorBuffer[2];
				m_ucpData[uiCurrentByte + 1] = ucpColorBuffer[1];
				m_ucpData[uiCurrentByte + 2] = ucpColorBuffer[0];

				if (pTGAinfo.m_uiBytesPerPixel == 4)
					m_ucpData[uiCurrentByte + 3] = ucpColorBuffer[3];

				uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
				uiCurrentPixel++;

				//make sure too many pixels have not been read in
				if (uiCurrentPixel > uiPixelCount)
				{
					if (ucpColorBuffer != NULL)
						delete[] ucpColorBuffer;
					if (m_ucpData != NULL)
						delete[] m_ucpData;

					return false;
				}
			}
		}

		//the chunk header is greater than 128 RLE data
		else
		{
			ucChunkHeader -= 127;

			memcpy(ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel);
			ucpFile += pTGAinfo.m_uiBytesPerPixel;

			//copy the color into the image data as many times as needed
			for (i = 0; i < ucChunkHeader; i++)
			{
				//switch R and B bytes around while copying
				m_ucpData[uiCurrentByte] = ucpColorBuffer[2];
				m_ucpData[uiCurrentByte + 1] = ucpColorBuffer[1];
				m_ucpData[uiCurrentByte + 2] = ucpColorBuffer[0];

				if (pTGAinfo.m_uiBytesPerPixel == 4)
					m_ucpData[uiCurrentByte + 3] = ucpColorBuffer[3];

				uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
				uiCurrentPixel++;

				//make sure that we have not written too many pixels
				if (uiCurrentPixel > uiPixelCount)
				{
					if (ucpColorBuffer != NULL)
						delete[] ucpColorBuffer;
					if (m_ucpData != NULL)
						delete[] m_ucpData;

					return false;
				}
			}
		}
	}

	//loop while there are still pixels left
	while (uiCurrentPixel < uiPixelCount);

	//the compressed TGA has been successfully loaded
	return true;
}

bool CImage::loadUncompressedTGA(std::string fileName)
{
	TGAInformationHeader pTGAinfo;
	unsigned int uiCSwap;
	unsigned char* ucpFile = m_ucpData;

	//skip past the TGA header in the data buffer
	ucpFile += 12;

	//copy the header data
	memcpy(pTGAinfo.m_ucHeader, ucpFile, sizeof(unsigned char[6]));

	//allocate memory for the image's data buffer
	create(pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0],
		pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2],
		pTGAinfo.m_ucHeader[4]);

	//set the class's member variables
	pTGAinfo.m_uiBPP = m_uiBPP;
	pTGAinfo.m_uiWidth = m_uiWidth;
	pTGAinfo.m_uiHeight = m_uiHeight;

	ucpFile += 6;

	if ((m_uiWidth <= 0) || (m_uiHeight <= 0) || ((m_uiBPP != 24) && (m_uiBPP != 32)))
		return false;

	pTGAinfo.m_uiBytesPerPixel = m_uiBPP / 8;
	pTGAinfo.m_uiImageSize = (pTGAinfo.m_uiBytesPerPixel*m_uiWidth*m_uiHeight);

	//copy the image data
	memcpy(m_ucpData, ucpFile, pTGAinfo.m_uiImageSize);

	//byte swapping ( optimized by Steve Thomas )
	for (uiCSwap = 0; uiCSwap < (int)pTGAinfo.m_uiImageSize; uiCSwap += pTGAinfo.m_uiBytesPerPixel)
	{
		m_ucpData[uiCSwap] ^= m_ucpData[uiCSwap + 2] ^=
			m_ucpData[uiCSwap] ^= m_ucpData[uiCSwap + 2];
	}

	//the uncompressed TGA has been successfully loaded
	return true;
}

unsigned int CImage::getPitch()
{
	return m_pitch;
}



