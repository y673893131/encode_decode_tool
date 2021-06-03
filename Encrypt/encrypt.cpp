#include "encrypt.h"

namespace Encrypt
{
	namespace Data
	{
		ENCODE_RET_TYPE encodeByType(const unsigned char* vByte, int vLen, Data_Type type)
		{
			ENCODE_RET_TYPE sData = ENCODE_RET_NULL;
			switch (type)
			{
			case Encrypt::DT_Hex:
				sData = Hex::byteToHex(vByte, vLen);
				break;
			case Encrypt::DT_Base64:
				sData = Base64::encode64((char*)vByte, vLen);
				break;
			default:
				break;
			}

#ifndef ENCODE_RET_STL
			return sData;
#else
			return std::move(sData);
#endif
		}

		ENCODE_RET_TYPE decodeByType(const char* vByte, int& vLen, Data_Type type)
		{
			ENCODE_RET_TYPE sData = ENCODE_RET_NULL;
			switch (type)
			{
			case Encrypt::DT_Hex:
				sData = Hex::hexToByte(vByte, vLen);
				break;
			case Encrypt::DT_Base64:
				sData = Base64::decode64((char*)vByte, vLen);
				break;
			default:
				break;
			}

#ifndef ENCODE_RET_STL
			return sData;
#else
			return std::move(sData);
#endif
		}
	}
}
