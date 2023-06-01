# MIPS_CPU
2023东南大学数字逻辑与计算机体系结构（含实验）II期末实验<br>
环境：VIVADO 2020.1

## 总述
基于书上的数据通路增加了多条指令。因为语法不够灵活中途从verilog改为system verilog。是小组作业但我一人2023年5月31日周三写了一天。关于IP的使用参考了这篇[文章](https://comp2008.gitee.io/lab2/mem/)。时钟IP没用到。更多见报告，就是根目录那个pdf。


## 支持的指令
### R类型
[31:26] [25:21] [20:16] [15:11] [10:6]  [5:0]
000000  |rs     |rt     |rd     |00000  |xxxxxx

- add   加
- sub   减
- and   与
- or    或
- nor   同或
- xor   异或
- sll   逻辑左移
- srl   逻辑右移
- sra   算数右移
- jr    寄存器跳转

### I类型
[31:26] [25:21] [20:16] [15:0]
op      |rs     |rt     |imm

- addi  加
- andi  与
- ori   或
- xori  异或
- lw    取字
- sw    存字
- beq   分支跳转

### J类型
[31:26] [25:0]
op      |imm

- j     跳转
- jal   跳转并存地址


## 写rom的方法：
在根目录找到mipscode.txt，编写MIPS汇编代码。运行“mipscode.py”，获得二进制机器码。把内容粘贴到rom.coe文件合适的位置。建议开头留一行000...000，因为cpu开头有reset操作，可能因此跳过第一跳指令（其实我也不知道会不会跳过）。这个脚本是网上搜的，加了点内容，不保证没有bug。

## 更换rom的方法：
VIVADO中文件树中，MIPS->im:InstrMem->instmem:pgrom，双击这个ip，选择Other Options，选择Coe File，选择根目录下的rom.coe，OK，Generate。注意，只修改rom的内容不会使仿真结果因此改变，必须重新generate。