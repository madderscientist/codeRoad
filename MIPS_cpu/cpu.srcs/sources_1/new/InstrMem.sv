// ��д��61821326���
`timescale 1ns / 1ps

// PC��32λ������16�ֽڵ�Ѱַ�ռ䣬ÿ4�ֽ�һ��λ������ֻҪ14����ַ��
module InstrMem (
	// Program ROM Pinouts
	input			clk,          // ROM clock
	input	[31:0]	PC,           // ��Դ��ȡָ��Ԫ��ȡָ��ַ��PC��
	output	[31:0]	Instruction   // ��ȡָ��Ԫ�Ķ��������ݣ�ָ�
);
	// ����64KB ROM, ������ʵ��ֻ�� 64KB ROM
    prgrom instmem(
        .clka(clk), // input wire clka
        .addra(PC[15:2]), // input wire [13 : 0] addra
        .douta(Instruction) // output wire [31 : 0] douta
    );
endmodule