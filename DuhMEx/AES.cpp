// AES.cpp: implementation of the CAES class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AES.h"
#include "Rijndael.h"
#include "Base64.h"

using std::string;
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int g_key_bits[] = {
   /* KEY_128 */ 128,
   /* KEY_192 */ 192,
   /* KEY_256 */ 256,
};

AES_CYPHER_T converKeyType(CAES::KEY_T keyType)
{
   switch(keyType)
   {
   case CAES::KEY_128:
      return AES_CYPHER_128;
   case CAES::KEY_192:
      return AES_CYPHER_192;
   case CAES::KEY_256:
      return AES_CYPHER_256;
   default:
      return AES_CYPHER_128;
   }
}

Duh::CAES::CAES(KEY_T KeyType /*= KEY_128*/,STRING_T ciphertextType /*= STRING_BASE64*/)
:m_keyType(KeyType)
,m_ciphertextType(ciphertextType)
{
 
}

Duh::CAES::~CAES()
{
   
}

std::string Duh::CAES::Encrypt_ecb(std::string strData,std::string strKey)
{
   string strResult;
   int nDataLen = 0;
   unsigned char *key =  NewKey(strKey);
   unsigned char *data =  NewData(strData,nDataLen);
   aes_encrypt_ecb(converKeyType(m_keyType),(uint8_t *)data,nDataLen,(uint8_t *)key);
   strResult = Encode(data,nDataLen);
   delete[] key;
   delete[] data;
   return strResult;
}

CString Duh::CAES::Encrypt_ecb(CString strData,CString strKey)
{
   return Encrypt_ecb(string(strData.GetBuffer(0)),string(strKey.GetBuffer(0))).c_str();
}

std::string Duh::CAES::Decrypt_ecb(std::string strData,std::string strKey)
{
   string strResult;
   int nDataLen = 0;
   unsigned char *key =  NewKey(strKey);
   nDataLen = DecLength(strData);
   unsigned char *data = new unsigned char[nDataLen];
   Decode(strData,data);
   aes_decrypt_ecb(converKeyType(m_keyType),(uint8_t *)data,nDataLen,(uint8_t *)key);
   strResult = (char*)data;
   delete[] key;
   delete[] data;
   return strResult;
}

CString Duh::CAES::Decrypt_ecb(CString strData,CString strKey)
{
   return Decrypt_ecb(string(strData.GetBuffer(0)),string(strKey.GetBuffer(0))).c_str();
}

std::string Duh::CAES::Encrypt_cbc(std::string strData,std::string strKey,std::string iv)
{
   string strResult;
   int nDataLen = 0;
   unsigned char *key =  NewKey(strKey);
   unsigned char *data =  NewData(strData,nDataLen);
   aes_encrypt_cbc(converKeyType(m_keyType),(uint8_t *)data,nDataLen,(uint8_t *)key,(uint8_t *)iv.c_str());
   strResult = Encode(data,nDataLen);
   delete[] key;
   delete[] data;
   return strResult;
}

CString Duh::CAES::Encrypt_cbc(CString strData,CString strKey,CString iv)
{
   return Encrypt_cbc(string(strData.GetBuffer(0)),string(strKey.GetBuffer(0)),string(iv.GetBuffer(0))).c_str();
}

std::string Duh::CAES::Decrypt_cbc(std::string strData,std::string strKey,std::string iv)
{
   string strResult;
   int nDataLen = 0;
   unsigned char *key =  NewKey(strKey);
   nDataLen = DecLength(strData);
   unsigned char *data = new unsigned char[nDataLen];
   Decode(strData,data);
   aes_decrypt_cbc(converKeyType(m_keyType),(uint8_t *)data,nDataLen,(uint8_t *)key,(uint8_t *)iv.c_str());
   strResult = (char*)data;
   delete[] key;
   delete[] data;
   return strResult;
}

CString Duh::CAES::Decrypt_cbc(CString strData,CString strKey,CString iv)
{
   return Decrypt_cbc(string(strData.GetBuffer(0)),string(strKey.GetBuffer(0)),string(iv.GetBuffer(0))).c_str();
}

unsigned char * Duh::CAES::NewKey(string strKey)
{
   unsigned char *key = new unsigned char[g_key_bits[m_keyType]];
   memset(key,0,g_key_bits[m_keyType]);
   memcpy(key,strKey.c_str(),min(strKey.length(),g_key_bits[m_keyType]));
   return key;
}

unsigned char * Duh::CAES::NewData(string strData,OUT int &len)
{
   int nLength = strData.length();
   int spaceLength = 16 - (nLength % 16);
   len = nLength + spaceLength;
   unsigned char* data = new unsigned char[len];
   memset(data,0, len);
   memcpy(data,strData.c_str(),nLength);
   return data;
}

std::string Duh::CAES::Encode(unsigned char *input,int nLength)
{
   switch(m_ciphertextType)
   {
   case STRING_HEX:
      return Encode_Hex(input,nLength);
   case STRING_BASE64:
      return Encode_Base64(input,nLength);
   default:
      return "";
      break;
   }
}

std::string Duh::CAES::Encode_Hex(unsigned char *input,int nLength)
{
   char* pOut = new char[2 * nLength +1];  //额外增加一个字节保存'\0'
   memset(pOut,0, 2*nLength + 1);
   for (int i= 0; i< nLength; ++i)
   {
      sprintf(pOut + i * 2,"%02X",input[i]);
   }
   return pOut;
}

std::string Duh::CAES::Encode_Base64(unsigned char *input,int nLength)
{
   int nOutLen = Base64::EncodedLength(nLength)+1; //额外增加一个字节保存'\0'
   char *out = new char[nOutLen];
   memset(out,0,nOutLen);
   Base64::Encode((char *)input,nLength,out,nOutLen);
   string strResult = out;
   delete[] out;
   return strResult;
}

int Duh::CAES::DecLength(std::string input)
{
   switch(m_ciphertextType)
   {
   case STRING_HEX:
      return input.length()/2;
   case STRING_BASE64:
      return Base64::DecodedLength(input);
   default:
      return 0;
   }
}

void Duh::CAES::Decode(std::string strText,unsigned char *output)
{
   switch(m_ciphertextType)
   {
   case STRING_HEX:
      Decode_Hex(strText,output);
      break;
   case STRING_BASE64:
      Decode_base64(strText,output);
      break;
   default:
      break;
   }
}

void Duh::CAES::Decode_Hex(std::string strText,unsigned char *output)
{
   int nLength = DecLength(strText);
   const char *src = strText.c_str();
   memset(output,0, nLength);
   for (int i=0; i<nLength; ++i)
   {
      output[i] = Char2Int(src[i * 2]) * 16 + Char2Int(src[i * 2 + 1]);
   }
}

void Duh::CAES::Decode_base64(std::string strText,unsigned char *output)
{
   Base64::Decode(strText.c_str(),strText.length(),(char *)output,DecLength(strText));
}

int Duh::CAES::Char2Int(char c) {
   if ('0' <= c && c <= '9') {
      return (c - '0');
   }
   else if ('a' <= c && c<= 'f') {
      return (c - 'a' + 10);
   }
   else if ('A' <= c && c<= 'F') {
      return (c - 'A' + 10);
   }
   return -1;
}
