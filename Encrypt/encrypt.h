#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

//#define ENCODE_RET_STL
#ifndef ENCODE_RET_STL
#include <string.h>
#define ENCODE_RET_TYPE char*
#define ENCODE_RET_NULL nullptr

#else
#include <string>
#define ENCODE_RET_TYPE std::string
#define ENCODE_RET_NULL ""
#endif

namespace Encrypt
{
	enum Data_Type
	{
		DT_Hex,
		DT_Base64
	};

	namespace Data
	{
		ENCODE_RET_TYPE encodeByType(const unsigned char* vByte, int vLen, Data_Type type);
		ENCODE_RET_TYPE decodeByType(const char* vByte, int& len, Data_Type type);
	}

	namespace Base64
	{
		ENCODE_RET_TYPE encode64(const char* input, int len);
		ENCODE_RET_TYPE decode64(const char* input, int& len);
	}

	namespace Hex
	{
		ENCODE_RET_TYPE hexToByte(const char* szHex, int& len);
		ENCODE_RET_TYPE byteToHex(const unsigned char* vByte, int vLen);
	}

	namespace RC4
	{
		ENCODE_RET_TYPE encode(const char* szSource, const char* szPassWord, Data_Type type = DT_Hex);
		ENCODE_RET_TYPE decode(const char* szSource, const char* szPassWord, Data_Type type = DT_Hex);
		char* encodeBin(const char* szSource, int len, const char* szPassWord, int passLen, int* outLen);
		char* decodeBin(const char* szSource, int len, const char* szPassWord, int passLen, int* outLen);
	}

	namespace DES
	{
		ENCODE_RET_TYPE encode(const char* src, const char* key, Data_Type type = DT_Hex);
		ENCODE_RET_TYPE decode(const char* src, const char* key, Data_Type type = DT_Hex);
	}

	namespace AES
	{
		ENCODE_RET_TYPE encode(const char* src, const char* key, Data_Type type = DT_Hex);
		ENCODE_RET_TYPE decode(const char* src, const char* key, Data_Type type = DT_Hex);
	}
}

#endif /*_ENCRYPT_H_*/