#ifndef BASE64_H
#define BASE64_H

#include <string>
class Base64 {
public:
   static bool Encode(const std::string &in, std::string *out);
   static bool Encode(const char *input, size_t input_length, char *out, size_t out_length);
   static bool Decode(const std::string &in, std::string *out);
   static bool Decode(const char *input, size_t input_length, char *out, size_t out_length);
   static int DecodedLength(const char *in, size_t in_length);
   static int DecodedLength(const std::string &in);
   static int EncodedLength(size_t length);
   static int EncodedLength(const std::string &in);
   static void StripPadding(std::string *in);
private:
   static void a3_to_a4(unsigned char * a4, unsigned char * a3);
   static void a4_to_a3(unsigned char * a3, unsigned char * a4);
   static unsigned char b64_lookup(unsigned char c);
};



#endif // BASE64_H
