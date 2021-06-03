#include "encrypt.h"

namespace Encrypt
{
	namespace DES
	{
		//���ܽ���
		enum
		{
			ENCRYPT = 0,	//! ����
			DECRYPT,			//! ����
		};

		//DES�㷨��ģʽ
		enum
		{
			ECB = 0,	//! ECBģʽ
			CBC				//! CBCģʽ
		};

		typedef bool(*PSubKey)[16][48];

		//Pad����ģʽ
		enum
		{
			PAD_ISO_1 = 0,	//! ISO_1��䣺���ݳ��Ȳ���8���صı�������0x00���㣬���Ϊ8���صı�������8��0x00
			PAD_ISO_2,		//! ISO_2��䣺���ݳ��Ȳ���8���صı�������0x80,0x00..���㣬���Ϊ8���صı�������0x80,0x00..0x00
			PAD_PKCS_7		//! PKCS7��䣺���ݳ��ȳ�8����Ϊn,��(8-n)����Ϊ8�ı���
		};

		// initial permutation IP
		const char IP_Table[64] = {
			58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
			62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
			57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
			61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
		};
		// final permutation IP^-1
		const char IPR_Table[64] = {
			40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
			38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
			36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
			34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
		};
		// expansion operation matrix
		const char E_Table[48] = {
			32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
			8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
			16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
			24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
		};
		// 32-bit permutation function P used on the output of the S-boxes
		const char P_Table[32] = {
			16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
			2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
		};
		// permuted choice table (key)
		const char PC1_Table[56] = {
			57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
			10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
			63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
			14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
		};
		// permuted choice key (table)
		const char PC2_Table[48] = {
			14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
			23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
			41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
			44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
		};
		// number left rotations of pc1
		const char LOOP_Table[16] = {
			1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
		};
		// The (in)famous S-boxes
		const char S_Box[8][4][16] = {
			// S1
			14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
			0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
			4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
			15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
			// S2
			15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
			3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
			0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
			13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
			// S3
			10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
			13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
			13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
			1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
			// S4
			7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
			13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
			10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
			3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
			// S5
			2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
			14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
			4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
			11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
			// S6
			12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
			10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
			9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
			4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
			// S7
			4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
			13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
			1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
			6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
			// S8
			13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
			1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
			7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
			2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
		};


		/*******************************************************************/
		/*
		�� �� �� ��:	ByteToBit
		�� �� �� ����	��BYTEת��ΪBit��
		�� �� ˵ ����	Out:	�����Bit��[in][out]
		In:		�����BYTE��[in]
		bits:	Bit���ĳ���[in]

		����ֵ ˵����	void
		/*******************************************************************/
		static void ByteToBit(bool *Out, const unsigned char *In, int bits)
		{
			for (int i = 0; i<bits; ++i)
				Out[i] = (In[i >> 3] >> (7 - i & 7)) & 1;
		}

		/*******************************************************************/
		/*
		�� �� �� ��:	BitToByte
		�� �� �� ����	��Bitת��ΪByte��
		�� �� ˵ ����	Out:	�����BYTE��[in][out]
		In:		�����Bit��[in]
		bits:	Bit���ĳ���[in]

		����ֵ ˵����	void
		/*******************************************************************/
		static void BitToByte(unsigned char *Out, const bool *In, int bits)
		{
			memset(Out, 0, bits >> 3);
			for (int i = 0; i<bits; ++i)
				Out[i >> 3] |= In[i] << (7 - i & 7);
		}
		/*******************************************************************/
		/*
		�� �� �� ��:	RotateL
		�� �� �� ����	��BIT����λ�������
		�� �� ˵ ����	In:		�����Bit��[in]
		len:	Bit���ĳ���[in]
		loop:	��������ĳ���

		����ֵ ˵����	void
		/*******************************************************************/
		static void RotateL(bool *In, int len, int loop)
		{
			bool Tmp[256];

			memcpy(Tmp, In, loop);
			memcpy(In, In + loop, len - loop);
			memcpy(In + len - loop, Tmp, loop);
		}

		/*******************************************************************/
		/*
		�� �� �� ��:	Xor
		�� �� �� ����	������Bit���������
		�� �� ˵ ����	InA:	�����Bit��[in][out]
		InB:	�����Bit��[in]
		loop:	Bit���ĳ���

		����ֵ ˵����	void
		/*******************************************************************/
		static void Xor(bool *InA, const bool *InB, int len)
		{
			for (int i = 0; i<len; ++i)
				InA[i] ^= InB[i];
		}

		/*******************************************************************/
		/*
		�� �� �� ��:	Transform
		�� �� �� ����	������Bit���������λת��
		�� �� ˵ ����	Out:	�����Bit��[out]
		In:		�����Bit��[in]
		Table:	ת����Ҫ�ı�ָ��
		len:	ת����ĳ���

		����ֵ ˵����	void
		/*******************************************************************/
		static void Transform(bool *Out, bool *In, const char *Table, int len)
		{
			bool Tmp[256];

			for (int i = 0; i<len; ++i)
				Tmp[i] = In[Table[i] - 1];
			memcpy(Out, Tmp, len);
		}

		/*******************************************************************/
		/*
		�� �� �� ��:	S_func
		�� �� �� ����	ʵ�����ݼ���S BOXģ��
		�� �� ˵ ����	Out:	�����32Bit[out]
		In:		�����48Bit[in]

		����ֵ ˵����	void
		/*******************************************************************/
		static void S_func(bool Out[32], const bool In[48])
		{
			for (char i = 0, j, k; i<8; ++i, In += 6, Out += 4)
			{
				j = (In[0] << 1) + In[5];
				k = (In[1] << 3) + (In[2] << 2) + (In[3] << 1) + In[4];	//��֯SID�±�

				for (int l = 0; l<4; ++l)								//����Ӧ4bit��ֵ
					Out[l] = (S_Box[i][j][k] >> (3 - l)) & 1;
			}
		}

		/*******************************************************************/
		/*
		�� �� �� ��:	F_func
		�� �� �� ����	ʵ�����ݼ��ܵ����P
		�� �� ˵ ����	Out:	�����32Bit[out]
		In:		�����48Bit[in]

		����ֵ ˵����	void
		/*******************************************************************/
		void F_func(bool In[32], const bool Ki[48])
		{
			bool MR[48];
			Transform(MR, In, E_Table, 48);
			Xor(MR, Ki, 48);
			S_func(In, MR);
			Transform(In, In, P_Table, 32);
		}

		//���㲢�������Կ��SubKey������
		void SetSubKey(PSubKey pSubKey, const unsigned char Key[8])
		{
			bool K[64], *KL = &K[0], *KR = &K[28];
			ByteToBit(K, Key, 64);
			Transform(K, K, PC1_Table, 56);
			for (int i = 0; i<16; ++i) {
				RotateL(KL, 28, LOOP_Table[i]);
				RotateL(KR, 28, LOOP_Table[i]);
				Transform((*pSubKey)[i], K, PC2_Table, 48);
			}
		}

		//DES��Ԫ����
		void DES(unsigned char Out[8], const unsigned char In[8], const PSubKey pSubKey, bool Type)
		{
			bool M[64], tmp[32], *Li = &M[0], *Ri = &M[32];
			ByteToBit(M, In, 64);
			Transform(M, M, IP_Table, 64);
			if (Type == ENCRYPT)
			{
				for (int i = 0; i<16; ++i)
				{
					memcpy(tmp, Ri, 32);		//Ri[i-1] ����
					F_func(Ri, (*pSubKey)[i]);	//Ri[i-1]����ת����SBox���ΪP
					Xor(Ri, Li, 32);			//Ri[i] = P XOR Li[i-1]
					memcpy(Li, tmp, 32);		//Li[i] = Ri[i-1]
				}
			}
			else
			{
				for (int i = 15; i >= 0; --i)
				{
					memcpy(tmp, Ri, 32);		//Ri[i-1] ����
					F_func(Ri, (*pSubKey)[i]);	//Ri[i-1]����ת����SBox���ΪP
					Xor(Ri, Li, 32);			//Ri[i] = P XOR Li[i-1]
					memcpy(Li, tmp, 32);		//Li[i] = Ri[i-1]
				}
			}
			RotateL(M, 64, 32);					//Ri��Li��λ����M
			Transform(M, M, IPR_Table, 64);		//���������ת��
			BitToByte(Out, M, 64);				//��֯���ַ�
		}

		/*
		* @Description   ������Э��Լ���ǰ�����ݽ������
		* @param bType	�����ͣ�PAD����
		* @param In		�����ݴ�ָ��
		* @param Out	����������ָ��
		* @param datalen�����ݵĳ���
		* @param padlen	��(in,out)���buffer�ĳ��ȣ�����ĳ���
		* @return    	��true--�ɹ���false--ʧ�ܣ�
		*/
		bool RunPad(int nType, const unsigned char* In,
			unsigned datalen, unsigned char* Out, unsigned& padlen)
		{
			if (nType < PAD_ISO_1 || nType > PAD_PKCS_7)
				return false;

			if (In == NULL || datalen < 0 || Out == NULL)
				return false;
			int res = (datalen & 0x00000007);

			if (res == 0)
			{
				padlen = datalen;
				memcpy(Out, In, datalen);
				return true;
			}

			padlen = (datalen + 8 - res);
			memcpy(Out, In, datalen);

			if (nType == PAD_ISO_1)
			{
				memset(Out + datalen, 0x00, 8 - res);
			}
			else
			{
				if (nType == PAD_ISO_2)
				{
					memset(Out + datalen, 0x80, 1);
					memset(Out + datalen, 0x00, 7 - res);
				}
				else
				{
					if (nType == PAD_PKCS_7)
					{
						memset(Out + datalen, 0x00, 8 - res);
					}
					else
					{
						return false;
					}
				}
			}

			return true;
		}

		/*
		* @Description    ��ִ��DES�㷨���ı��ӽ���
		* @param bType   �����ͣ�����ENCRYPT������DECRYPT
		* @param bMode   ��ģʽ��ECB,CBC
		* @param In      �������ܴ�ָ��
		* @param Out     ���������ָ��
		* @param datalen �������ܴ��ĳ��ȣ�ͬʱOut�Ļ�������СӦ���ڻ��ߵ���datalen
		* @param Key     ����Կ(��Ϊ8λ,16λ,24λ)֧��3��Կ
		* @param keylen  ����Կ���ȣ����24λ���ֽ����Զ��ü�
		* @return        ��true--�ɹ���false--ʧ�ܣ�
		*/
		bool RunDes(bool bType, bool bMode, const unsigned char* In, unsigned Inlen,
			unsigned char* Out, unsigned& OutLen, const unsigned char* Key, unsigned keylen)
		{
			//�ж�����Ϸ���
			if (!(In && Out && Key && Inlen && keylen >= 0))
				return false;

			unsigned char* OutTop = Out;
			unsigned char* newKey = nullptr;
			if (keylen < 8)
			{
				newKey = new unsigned char[9];
				memcpy(newKey, Key, keylen);
				strncpy_s((char*)newKey + keylen, 9 - keylen, "00000000", 8 - keylen);
				keylen = 8;
				newKey[8] = 0;
			}
			else
			{
				newKey = new unsigned char[keylen + 1];
				memcpy(newKey, Key, keylen);
				newKey[keylen] = 0;
			}

			unsigned char* inbuf = new unsigned char[Inlen + 8];
			memset(inbuf, 0x00, sizeof(inbuf));
			memcpy(inbuf, In, Inlen);
			unsigned char* tempBuf;
			unsigned padlen = Inlen;
			//ֻ����8�������������㳤���Լ����
			if (Inlen & 0x00000007)
			{
				if (!RunPad(PAD_PKCS_7, In, Inlen, inbuf, padlen))
				{
					delete[] newKey;
					delete[] inbuf;
					return false;
				}
				tempBuf = inbuf;
			}
			else
			{
				tempBuf = inbuf;
			}

			bool				m_SubKey[3][16][48];		//��Կ
			//���첢����SubKeys
			unsigned char nKey = (keylen >> 3) >= 3 ? 3 : (keylen >> 3);
			for (int i = 0; i<nKey; i++)
			{
				SetSubKey(&m_SubKey[i], &Key[i << 3]);
			}

			if (bMode == ECB)	//ECBģʽ
			{
				if (nKey == 1)	//��Key
				{
					int j = padlen >> 3;
					for (int i = 0, j = padlen >> 3; i<j; ++i, Out += 8, tempBuf += 8)
					{
						DES(Out, tempBuf, &m_SubKey[0], bType);
					}
				}
				else
				{
					if (nKey == 2)	//3DES 2Key
					{
						for (int i = 0, j = padlen >> 3; i < j; ++i, Out += 8, tempBuf += 8)
						{
							DES(Out, tempBuf, &m_SubKey[0], bType);
							DES(Out, Out, &m_SubKey[1], !bType);
							DES(Out, Out, &m_SubKey[0], bType);
						}
					}
					else			//3DES 3Key
					{
						for (int i = 0, j = padlen >> 3; i < j; ++i, Out += 8, tempBuf += 8)
						{
							DES(Out, tempBuf, &m_SubKey[bType ? 2 : 0], bType);
							DES(Out, Out, &m_SubKey[1], !bType);
							DES(Out, Out, &m_SubKey[bType ? 0 : 2], bType);
						}
					}
				}
			}
			else				//CBCģʽ
			{
				unsigned char	cvec[8] = "";	//Ťת����
				unsigned char	cvin[8] = ""; //�м����

				if (nKey == 1)	//��Key
				{
					for (int i = 0, j = padlen >> 3; i<j; ++i, Out += 8, tempBuf += 8)
					{
						if (bType == ENCRYPT)
						{
							for (int j = 0; j<8; ++j)		//��������Ťת�������
							{
								cvin[j] = tempBuf[j] ^ cvec[j];
							}
						}
						else
						{
							memcpy(cvin, tempBuf, 8);
						}

						DES(Out, cvin, &m_SubKey[0], bType);

						if (bType == ENCRYPT)
						{
							memcpy(cvec, Out, 8);			//������趨ΪŤת����
						}
						else
						{
							for (int j = 0; j<8; ++j)		//�������Ťת�������
							{
								Out[j] = Out[j] ^ cvec[j];
							}
							memcpy(cvec, cvin, 8);			//�������趨ΪŤת����
						}
					}
				}
				else
				{
					if (nKey == 2)	//3DES CBC 2Key
					{
						for (int i = 0, j = padlen >> 3; i < j; ++i, Out += 8, tempBuf += 8)
						{
							if (bType == ENCRYPT)
							{
								for (int j = 0; j < 8; ++j)		//��������Ťת�������
								{
									cvin[j] = tempBuf[j] ^ cvec[j];
								}
							}
							else
							{
								memcpy(cvin, tempBuf, 8);
							}

							DES(Out, cvin, &m_SubKey[0], bType);
							DES(Out, Out, &m_SubKey[1], !bType);
							DES(Out, Out, &m_SubKey[0], bType);

							if (bType == ENCRYPT)
							{
								memcpy(cvec, Out, 8);			//������趨ΪŤת����
							}
							else
							{
								for (int j = 0; j < 8; ++j)		//�������Ťת�������
								{
									Out[j] = Out[j] ^ cvec[j];
								}
								memcpy(cvec, cvin, 8);			//�������趨ΪŤת����
							}
						}
					}
					else			//3DES CBC 3Key
					{
						for (int i = 0, j = padlen >> 3; i < j; ++i, Out += 8, tempBuf += 8)
						{
							if (bType == ENCRYPT)
							{
								for (int j = 0; j < 8; ++j)		//��������Ťת�������
								{
									cvin[j] = tempBuf[j] ^ cvec[j];
								}
							}
							else
							{
								memcpy(cvin, tempBuf, 8);
							}

							DES(Out, cvin, &m_SubKey[bType ? 2 : 0], bType);
							DES(Out, Out, &m_SubKey[1], !bType);
							DES(Out, Out, &m_SubKey[bType ? 0 : 2], bType);

							if (bType == ENCRYPT)
							{
								memcpy(cvec, Out, 8);			//������趨ΪŤת����
							}
							else
							{
								for (int j = 0; j < 8; ++j)		//�������Ťת�������
								{
									Out[j] = Out[j] ^ cvec[j];
								}
								memcpy(cvec, cvin, 8);			//�������趨ΪŤת����
							}
						}
					}
				}
			}

			delete[] newKey;
			delete[] inbuf;
			OutLen = Out - OutTop;
			return true;
		}

		ENCODE_RET_TYPE encode(const char* src, const char* key, Data_Type type)
		{
			auto srcLen = strlen(src);
			char* strEncode = new char[srcLen * 2];
			memset(strEncode, 0x00, sizeof(char) * (srcLen * 2));
			unsigned int outLen = 0;
			if (RunDes(true, true, (unsigned char*)src, srcLen, (unsigned char*)strEncode, outLen, (unsigned char*)key, strlen(key)))
			{
				auto sData = Data::encodeByType((unsigned char*)strEncode, outLen, type);
				delete[] strEncode;
#ifndef ENCODE_RET_STL
				return sData;
#else
				return std::move(sData);
#endif
			}

			delete[] strEncode;
			return ENCODE_RET_NULL;
		}

		ENCODE_RET_TYPE decode(const char* src, const char* key, Data_Type type)
		{
			int srcLen = 0;
			auto sEncode = Data::decodeByType(src, srcLen, type);
			unsigned int outLen = 0;

#ifndef ENCODE_RET_STL
			char* sData = new char[srcLen * 2];
			memset(sData, 0x00, sizeof(char) * (srcLen * 2));
			if (RunDes(false, true, (unsigned char*)sEncode, srcLen, (unsigned char*)sData, outLen, (unsigned char*)key, strlen(key)))
			{
				if (sEncode)
				{
					delete[] sEncode;
				}

				return sData;
			}

			if (sEncode)
			{
				delete[] sEncode;
			}

			delete[] sData;
#else
			std::string sData;
			sData.resize(srcLen * 2);
			if (RunDes(false, true, (unsigned char*)sEncode.c_str(), srcLen, (unsigned char*)sData.data(), outLen, (unsigned char*)key, strlen(key)))
			{
				return std::move(sData);
			}
#endif

			return ENCODE_RET_NULL;
		}
	}
}