#include "Cryptology.h"
#include <windows.h>
using namespace std;
string Utf8ToGbk(const char *src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
char* HSKL(const char* const str,bool mode=true) {
	char* a=SM3::HashStr(str);
	char i=0;
	if(mode){
		char* key=new char[16];
		char index[16]={1, 3, 4, 5, 7, 11, 12, 14, 16, 17, 19, 22, 25, 26, 28, 31};
		for(;i<16;i++){
			key[i]=a[index[i]];
		}
		AES::keyLoad(key);
		delete []key;
		i=0;
	}
	char index2[8]={8, 9, 10, 13, 18, 23, 24, 27};
	char* userkey=new char[9]; 
	for(;i<8;i++){
		userkey[i]=(a[index2[i]]==0)?(i+1):a[index2[i]];
	}
	userkey[8]='\0'; 
	delete []a;
	return userkey;
}
bool EncryptUTF(const char* const iFile, const char* const oFile){
	return AES::Encrypt(Utf8ToGbk(iFile).data(),Utf8ToGbk(oFile).data());
}
bool DecipherUTF(const char* const iFile, const char* const oPath){
	return AES::Decipher(Utf8ToGbk(iFile).data(),Utf8ToGbk(oPath).data());
}
bool fileDeleteUTF(const char* const file){
	return Cryptology_::fileDelete(Utf8ToGbk(file).data());
}

