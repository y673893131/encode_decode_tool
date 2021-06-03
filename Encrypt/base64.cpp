#include "encrypt.h"

namespace Encrypt
{
	namespace Base64
	{
		static const char alphabet64[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static const char pad = '=';
		static const char np = -1;
		static char table64vals[] =
		{
			62, np, np, np, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, np, np, np, np, np,
			np, np, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
			18, 19, 20, 21, 22, 23, 24, 25, np, np, np, np, np, np, 26, 27, 28, 29, 30, 31,
			32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
		};

		inline char table64(unsigned char c)
		{
			return (c < 43 || c > 122) ? np : table64vals[c - 43];
		}

		ENCODE_RET_TYPE encode64(const char* input, int len)
		{
			if (!input)
			{
				return ENCODE_RET_NULL;
			}

			char c;
			int length = len;
#ifndef ENCODE_RET_STL
			char* encoded = new char[length * 2 + 1];
			char* ret = encoded;
			memset(encoded, 0x00, length * 2 + 1);
#else
			std::string sData;
			sData.reserve(length * 2);
			char* encoded = (char*)sData.data();
#endif
			for (int i = 0; i < length; ++i)
			{
				c = (input[i] >> 2) & 0x3f;
				*encoded++ = alphabet64[c];

				c = (input[i] << 4) & 0x3f;
				if (++i < length)
					c = c | ((input[i] >> 4) & 0x0f);
				*encoded++ = alphabet64[c];

				if (i < length)
				{
					c = (input[i] << 2) & 0x3c;
					if (++i < length)
						c = c | ((input[i] >> 6) & 0x03);
					*encoded++ = alphabet64[c];
				}
				else
				{
					++i;
					*encoded++ = pad;
				}

				if (i < length)
				{
					c = input[i] & 0x3f;
					*encoded++ = alphabet64[c];
				}
				else
				{
					*encoded++ = pad;
				}
			}
#ifndef ENCODE_RET_STL
			return ret;
#else
			return std::move(sData);
#endif
		}

		ENCODE_RET_TYPE decode64(const char* input, int& len)
		{
			if (!input)
			{
				return ENCODE_RET_NULL;
			}

			char c, d;
			int length = strlen(input);
#ifndef ENCODE_RET_STL
			char* decoded = new char[length + 1];
			memset(decoded, 0x00, length + 1);
#else
			std::string sData;
			sData.reserve(length);
			char* decoded = (char*)sData.data();
#endif
			char* ret = decoded;
			for (int i = 0; i < length; ++i)
			{
				c = table64(input[i]);
				++i;
				d = table64(input[i]);
				c = (c << 2) | ((d >> 4) & 0x3);
				*decoded++ = c;
				if (++i < length)
				{
					c = input[i];
					if (pad == c)
						break;

					c = table64(input[i]);
					d =((d << 4) & 0xf0) | ((c >> 2) & 0xf);
					*decoded++ = d;
				}

				if (++i < length)
				{
					d = input[i];
					if (pad == d)
						break;

					d = table64(input[i]);
					c = ((c << 6) & 0xc0) | d;
					*decoded++ = c;
				}
			}

			len = decoded - ret;
#ifndef ENCODE_RET_STL
			return ret;
#else
			return std::move(sData);
#endif
		}
	}
}