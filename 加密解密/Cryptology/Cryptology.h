#pragma once

#include <fstream>
#include <string>
#include <sstream>

namespace Cryptology_ {
	using std::ifstream;
	using std::ofstream;
	using std::istream;
	using std::ostream;

	typedef unsigned long long ULL;
	typedef unsigned int UI;
	typedef unsigned short US;
	typedef unsigned char UB;

	unsigned long long getLen(std::istream& fin);
	char* sstrToCstr(std::ostringstream& osstr);

	bool fileDelete(const char* const file);
	//删除文件，请自行使用。
}

// i/oPath : 地址（以'\\'结尾），如		C:\Program Files\
// i/oFile : 文件名，如					C:\Program Files\a.txt
// char* 不加说明是C-string（以'\0'结尾），是char数组的会特殊说明

namespace AES {
	//AES (对称)
	//Key: 128 bits
	//E: 128 bits/block -> 128 bits/block

	//加密
	bool Encrypt(const char* const iFile, const char* const oFile);
	//解密
	bool Decipher(const char* const iFile, const char* const oPath);
	//失败返回false
	
	//加密
	char* Encrypt(const char* const str);
	//解密
	char* Decipher(const char* const str);
	//失败返回nullptr,记得释放
	
	//秘钥获取（秘钥改变的时候运行一次就可以了，不是一次性的）
	void keyLoad(const char* const key); //key=char[16]

	//优化中。。。现有接口不会变，但可能增加调参数的接口
}

namespace SM3 {
	//SM3 (散列)
	//512 bits/block -> 256 bits

	char* HashFile(const char* const iFile);
	char* HashStr(const char* const str);
	//均返回char[32]
}

namespace RSA {
	//RSA （非对称，1920位秘钥）
	//还没想好，暂时用不到
}
