# MidiReader 读取Midi并播放

- 创建项目时间：2022年10月16日，18:28:17
- 学习目标：[midi格式学习](https://zhuanlan.zhihu.com/p/464166848)、调用windowsApi中的midi输出
- 关键代码：

```
#include <Windows.h>
#pragma comment(lib,"winmm.lib")
	HMIDIOUT handle;
	midiOutOpen(&handle, 0, 0, 0, 0);
	midiOutShortMsg(handle, 力度音符乐器信息);  // 发送midi事件
	midiOutClose(handle);
```

“力度音符乐器信息”传参举例：<br>
0x00403C90
1. **0x40**: 音量
2. **0x3C**: 音符
3. **0x90**: 乐器

- 思路：把二进制解码为事件，分音轨存入链表，完成读取；播放前，将各音轨合并，事件按时间排好；播放时，发送事件并sleep

- 使用：cmd里面输入 "MidiReader.exe 要播放的mid文件名"

- 成果：加强了对二进制文件读取的能力

