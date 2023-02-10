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
	//ɾ���ļ���������ʹ�á�
}

// i/oPath : ��ַ����'\\'��β������		C:\Program Files\
// i/oFile : �ļ�������					C:\Program Files\a.txt
// char* ����˵����C-string����'\0'��β������char����Ļ�����˵��

namespace AES {
	//AES (�Գ�)
	//Key: 128 bits
	//E: 128 bits/block -> 128 bits/block

	//����
	bool Encrypt(const char* const iFile, const char* const oFile);
	//����
	bool Decipher(const char* const iFile, const char* const oPath);
	//ʧ�ܷ���false
	
	//����
	char* Encrypt(const char* const str);
	//����
	char* Decipher(const char* const str);
	//ʧ�ܷ���nullptr,�ǵ��ͷ�
	
	//��Կ��ȡ����Կ�ı��ʱ������һ�ξͿ����ˣ�����һ���Եģ�
	void keyLoad(const char* const key); //key=char[16]

	//�Ż��С��������нӿڲ���䣬���������ӵ������Ľӿ�
}

namespace SM3 {
	//SM3 (ɢ��)
	//512 bits/block -> 256 bits

	char* HashFile(const char* const iFile);
	char* HashStr(const char* const str);
	//������char[32]
}

namespace RSA {
	//RSA ���ǶԳƣ�1920λ��Կ��
	//��û��ã���ʱ�ò���
}
