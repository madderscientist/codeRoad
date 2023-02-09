#include <fstream>
#include <string>
#include <Windows.h>
#include "linklist.h"
#pragma comment(lib,"winmm.lib")
using namespace std;
// 一个事件 只有音符的按下和松开
struct event{
	unsigned int at = 0;		// 时间点
	unsigned char note = 0;		// 音符序号, 0~127
	unsigned char force = 0;	// 力度 0表示不响
	event(unsigned int AT = 0, unsigned char NOTE = 0, unsigned char FORCE = 0):at(AT),note(NOTE),force(FORCE){}
};
// read函数中读取字符数超过文件字符数，超过的全部读到零
// 一个midi文件
class MidiReader{
public:
	char miditype = 1;		// midi文件类型, 1为同步多音轨, 0为单音轨, 2为不同步多音轨
	int tick = 480;			// 一个四份音符的tick数
	double t = 50000;	// 一个四分音符的微秒数
	linklist<event*> mtrk[16];	// 最多16个音轨, 用链表组织, 事件中只存储音符的开闭, 速度变化等不存
	
	MidiReader(string);
	~MidiReader(){};
	void play(int which = 0xFFFF, bool harmonicaMode = false);
};

MidiReader::MidiReader(string midifile){
	ifstream fin(midifile);
	unsigned char buffer[10];	//如果不是unsigned则会出现负数
	// 判断是不是mid文件
	fin.read((char*)&buffer, 4);
	if(buffer[0]!='M' || buffer[1]!='T' || buffer[2]!='h' || buffer[3]!='d') return;
	// 是mid, 开始读取文件头, 固定大小10字节
	fin.read((char*)&buffer, 10);
	this->tick = buffer[9] + (buffer[8]<<8);
	int mtrkNum = buffer[7] + (buffer[6]<<8);

	// 读mtrk音轨
	for(int i=0; i<mtrkNum; i++){
		// 判断是否为MTrk音轨
		fin.read((char*)&buffer, 4);
		if(buffer[0]!='M' || buffer[1]!='T' || buffer[2]!='r' || buffer[3]!='k') {i--; continue;}
		// 获取数据长度, 读取一个音轨
		fin.read((char*)&buffer, 4);
		unsigned int Size = buffer[3]+ (buffer[2]<<8) + (buffer[1]<<16) + (buffer[0]<<24);
		unsigned char* Buffer = new unsigned char[Size];
		fin.read((char*)Buffer, Size);
		// 分析其中内容
		unsigned int Time = 0;		// 总时间(tick)
		unsigned char lastType = 0xC0;	// 上一个midi事件类型
		for(unsigned int k=0; k<Size-5; k++){	// 最后四个是结束符
			{	// 时间间隔(tick)
				unsigned int derta = 0;
				while(Buffer[k]>127) derta = (derta<<7) + Buffer[k++]-128;
				Time += (derta<<7) + Buffer[k++];
			}
			{	// 事件类型
				unsigned char type = Buffer[k] & 0xF0;
				unsigned char channel = Buffer[k++] - type;
				bool flag;
				do{
					flag = false;
					switch (type) {		// switch结束后k应该指向本事件的最后一个数据
					case 0x90:	// 按下音符
						this->mtrk[channel].insert(0, new event(Time, Buffer[k++], Buffer[k]));
						break;
					case 0x80:	// 松开音符
						this->mtrk[channel].insert(0, new event(Time, Buffer[k++], 0));
						break;
					case 0xF0:	// 系统码和其他格式
						if(channel == 0xF){
							switch (Buffer[k++]) {
							case 0x51:	// 一个四分音符的微秒 没有处理中途变速的情况
								this->t = (Buffer[++k]<<16) + (Buffer[++k]<<8) + (Buffer[++k]);
							default:
								k += Buffer[k];
								break;
							}
						}else if(!channel){	// 系统码
							unsigned int derta = 0;
							while(Buffer[k]>127) derta = (derta<<7) + Buffer[k++];
							k += derta;
						}
						break;
					case 0xB0:	// 控制器
						k ++;
						break;
					case 0xC0:	// 改变乐器
						break;
					case 0xD0:	// 触后通道
						break;
					case 0xE0:	// 滑音
						k ++;
						break;
					case 0xA0:	// 触后音符
						k ++;
						break;
					default:
						type = lastType;
						flag = true;
						break;
					}
				} while (flag);
				lastType = type;
			}
		}
		delete[]Buffer;
	}

	fin.close();
}
void MidiReader::play(int which, bool harmonicaMode){
	// 获取时序数据
	int len = 0;
	Node<event*>* points[16];
	for (int i = 0; i < 16; i++) {
		if (which & 1 << i) {
			len += this->mtrk[i].length;
			points[i] = this->mtrk[i].head->next;
		}
		else points[i] = nullptr;
	}
	linklist<event*> music;
	
	// 添加音符同时按时间排序
	for (int j=0; j < 16; j++) {
		while (points[j]) {
			Node<event*>* p = music.head;
			for (; p->next; p = p->next) {// 找到位置并插入
				if (p->next->data->at > points[j]->data->at) break;
				else if (p->next->data->at == points[j]->data->at && p->next->data->force > points[j]->data->force) break;
			}
			p->next = new Node<event*>(points[j]->data, p->next);
			points[j] = points[j]->next;
		}
	}

	HMIDIOUT handle;
	midiOutOpen(&handle, 0, 0, 0, 0);
	// 开始播放
	for (Node<event*>* p = music.head->next; p; p=p->next) {
		printf("time: %d\tnote: %d\tforce: %d\n", p->data->at, p->data->note, p->data->force);
		midiOutShortMsg(handle, (p->data->force << 16) + (p->data->note << 8) + 0x90);
		if (p->next) {
			int x = p->next->data->at - p->data->at;
			double y = (double)x / this->tick * this->t / 15000;
			if (x > 0) {
				printf("delay:%d\n", (int)y);
				Sleep((int)y);
			}
		}
	}
	midiOutClose(handle);
	music.clear();
}
