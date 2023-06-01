// 编写：61821326李睿刚
`timescale 1ns / 1ps

// PC是32位，但是16字节的寻址空间，每4字节一单位，所以只要14根地址线
module InstrMem (
	// Program ROM Pinouts
	input			clk,          // ROM clock
	input	[31:0]	PC,           // 来源于取指单元的取指地址（PC）
	output	[31:0]	Instruction   // 给取指单元的读出的数据（指令）
);
	// 分配64KB ROM, 编译器实际只用 64KB ROM
    prgrom instmem(
        .clka(clk), // input wire clka
        .addra(PC[15:2]), // input wire [13 : 0] addra
        .douta(Instruction) // output wire [31 : 0] douta
    );
endmodule