#include "encrypt.h"

namespace Encrypt
{
	namespace Hex
	{
		ENCODE_RET_TYPE hexToByte(const char* szHex, int& len)
		{
			if (!szHex)
			{
				return ENCODE_RET_NULL;
			}

			int iLen = strlen(szHex);
			if (iLen <= 0 || 0 != iLen % 2)
			{
				return ENCODE_RET_NULL;
			}

			len = iLen / 2;
#ifndef ENCODE_RET_STL
			char* pbBuf = new char[iLen / 2 + 1];
#else
			std::string sData;
			sData.reserve(iLen / 2);
			char* pbBuf = (char*)sData.data();
#endif
			int tmp1, tmp2;
			for (int i = 0; i < iLen / 2; i++)
			{
				tmp1 = (int)szHex[i * 2] - (((int)szHex[i * 2] >= 'A') ? 'A' - 10 : '0');
				if (tmp1 >= 16)
				{
					return ENCODE_RET_NULL;
				}

				tmp2 = (int)szHex[i * 2 + 1] - (((int)szHex[i * 2 + 1] >= 'A') ? 'A' - 10 : '0');
				if (tmp2 >= 16)
				{
					return ENCODE_RET_NULL;
				}

				pbBuf[i] = (tmp1 * 16 + tmp2);
			}

#ifndef ENCODE_RET_STL
			pbBuf[iLen / 2] = 0;
			return pbBuf;
#else
			return std::move(sData);
#endif
		}

		ENCODE_RET_TYPE byteToHex(const unsigned char* vByte, int vLen)
		{
			if (!vByte)
			{
				return ENCODE_RET_NULL;
			}
#ifndef ENCODE_RET_STL
			char* pbBuf = new char[vLen * 2 + 2];
#else
			std::string sData;
			sData.resize(vLen * 2);
			char* pbBuf = (char*)sData.data();
#endif
			int tmp;
			for (int i = 0; i < vLen; i++)
			{
				tmp = (int)(vByte[i]) / 16;
				pbBuf[i * 2] = (char)(tmp + ((tmp>9) ? 'A' - 10 : '0'));
				tmp = (int)(vByte[i]) % 16;
				pbBuf[i * 2 + 1] = (char)(tmp + ((tmp > 9) ? 'A' - 10 : '0'));
			}

#ifndef ENCODE_RET_STL
			pbBuf[vLen * 2] = 0;
			pbBuf[vLen * 2 + 1] = 0;
			return pbBuf;
#else
			return std::move(sData);
#endif
		}
	}
}