// AES.h: interface for the CAES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AES_H__5FA10C71_835B_4620_BA64_0DBA167A4305__INCLUDED_)
#define AFX_AES_H__5FA10C71_835B_4620_BA64_0DBA167A4305__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <STRING>
namespace Duh
{
   class AFX_EXT_CLASS CAES  
   {
   public:
      typedef enum {
         KEY_128 = 0,
         KEY_192,
         KEY_256,
      } KEY_T;
      typedef enum {
         STRING_HEX,
         STRING_BASE64,
      } STRING_T;
   public:
      CAES(KEY_T keyType = KEY_128,STRING_T ciphertextType = STRING_BASE64);
      virtual ~CAES();  
      std::string Encrypt_ecb(std::string strData,std::string strKey);
      CString     Encrypt_ecb(CString strData,CString strKey);
      std::string Decrypt_ecb(std::string strData,std::string strKey);
      CString     Decrypt_ecb(CString strData,CString strKey);
      std::string Encrypt_cbc(std::string strData,std::string strKey,std::string iv);
      CString     Encrypt_cbc(CString strData,CString strKey,CString iv);
      std::string Decrypt_cbc(std::string strData,std::string strKey,std::string iv);
      CString     Decrypt_cbc(CString strData,CString strKey,CString iv);
   protected:
      KEY_T m_keyType;
      STRING_T m_ciphertextType;
   protected:
      unsigned char *NewKey(std::string strKey);
      unsigned char *NewData(std::string strData,OUT int &len);
      //密文编码
      std::string Encode(unsigned char *input,int nLength);
      std::string Encode_Hex(unsigned char *input,int nLength);
      std::string Encode_Base64(unsigned char *input,int nLength);
      //密文解码
      int DecLength(std::string input);  //返回解码后长度
      void Decode(std::string strText,unsigned char *output);
      void Decode_Hex(std::string strText,unsigned char *output);
      void Decode_base64(std::string strText,unsigned char *output);

      int Char2Int(char c);
   };
}


#endif // !defined(AFX_AES_H__5FA10C71_835B_4620_BA64_0DBA167A4305__INCLUDED_)
