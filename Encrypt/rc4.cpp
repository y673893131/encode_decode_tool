#include "encrypt.h"

namespace Encrypt
{
	namespace RC4{
#define BOX_LEN 256
		void swap_byte(unsigned char* a, unsigned char* b)
		{
			unsigned char swapByte;
			swapByte = *a;
			*a = *b;
			*b = swapByte;
		}

		bool GetKey(const unsigned char* pass, int pass_len, unsigned char* out)
		{
			if (pass == nullptr || out == nullptr)
				return false;

			int i;

			for (i = 0; i < BOX_LEN; i++)
				out[i] = i;

			int j = 0;
			for (i = 0; i < BOX_LEN; i++)
			{
				j = (pass[i % pass_len] + out[i] + j) % BOX_LEN;
				swap_byte(&out[i], &out[j]);
			}

			return true;
		}

		bool RC4(const unsigned char* data, int data_len, const unsigned char* key, int key_len, unsigned char* out, int* out_len)
		{
			if (data == nullptr || key == nullptr || out == nullptr)
				return false;

			unsigned char* mBox = new unsigned char[BOX_LEN];

			if (!GetKey(key, key_len, mBox))
				return false;

			int i = 0;
			int x = 0;
			int y = 0;

			for (int k = 0; k < data_len; k++)
			{
				x = (x + 1) % BOX_LEN;
				y = (mBox[x] + y) % BOX_LEN;
				swap_byte(&mBox[x], &mBox[y]);
				out[k] = data[k] ^ mBox[(mBox[x] + mBox[y]) % BOX_LEN];
			}

			*out_len = data_len;
			delete[] mBox;
			return true;
		}

		ENCODE_RET_TYPE encode(const char* szSource, const char* szPassWord, Data_Type type)
		{
			if (szSource == nullptr || szPassWord == nullptr) 
				return ENCODE_RET_NULL;
			
			unsigned char* ret = new unsigned char[strlen(szSource)];
			int ret_len = 0;
			if (!RC4((unsigned char*)szSource, strlen(szSource), (unsigned char*)szPassWord, strlen(szPassWord), ret, &ret_len))
				return ENCODE_RET_NULL;

			auto sData = Data::encodeByType(ret, ret_len, type);
			delete[] ret;

#ifndef ENCODE_RET_STL
			return sData;
#else
			return std::move(sData);
#endif
		}

		ENCODE_RET_TYPE decode(const char* szSource, const char* szPassWord, Data_Type type)
		{
			if (szSource == nullptr || (strlen(szSource) % 2 != 0) || szPassWord == nullptr)
				return ENCODE_RET_NULL;

			int len = 0;
			auto src = Data::decodeByType(szSource, len, type);
			int ret_len = 0;

#ifndef ENCODE_RET_STL
			unsigned char* ret = new unsigned char[len + 1];
			memset(ret, 0, len + 1);
			if (RC4((unsigned char*)src, len, (unsigned char*)szPassWord, strlen(szPassWord), ret, &ret_len))
			{
				ret[ret_len] = 0;
				if (src) delete[]src;
				return (char*)ret;
			}

			if (src) delete[]src;
			if (ret) delete[]ret;
#else
			std::string sData;
			sData.resize(len);
			if (RC4((unsigned char*)src.c_str(), len, (unsigned char*)szPassWord, strlen(szPassWord), (unsigned char*)sData.data(), &ret_len))
			{
				return std::move(sData);
			}
#endif

			return ENCODE_RET_NULL;
		}

		char* encodeBin(const char* szSource, int len, const char* szPassWord, int passLen, int* outLen)
		{
			if (szPassWord == nullptr) return nullptr;
			unsigned char* ret = new unsigned char[len];
			try
			{
				if (!RC4((unsigned char*)szSource, len, (unsigned char*)szPassWord,	passLen, ret, outLen))
					return nullptr;
			}
			catch (...)
			{
				delete[] ret;
				return nullptr;
			}

			return (char*)ret;
		}

		char* decodeBin(const char* szSource, int len, const char* szPassWord, int passLen, int* outLen)
		{
			if (szPassWord == nullptr)
				return nullptr;
			unsigned char* src = (unsigned char*)szSource;
			unsigned char* ret = new unsigned char[len + 1];
			memset(ret, 0x00, len + 1);
			try
			{
				if (!RC4(src, len, (unsigned char*)szPassWord, passLen, ret, outLen))
				{
					delete[] ret;
					return nullptr;
				}
			}
			catch (...)
			{
				delete[] ret;
				return nullptr;
			}

			ret[*outLen] = 0;
			return (char*)ret;
		}
	}
}