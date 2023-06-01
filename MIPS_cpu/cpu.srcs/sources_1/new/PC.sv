// ��д��61821326���
`timescale 1ns / 1ps

// ���������PC+1*4��ѡ��һ��
// beq: op rs rt offset[15:0] �������ƫ����
// j:   op addr[25:0]         ��������λ��
// jr:  op rs 0 0 0 funct     ���Ĵ����õ�����λ��
// jal: op addr[25:0]         ��������λ�� �����浱ǰ��ַ+1*4

module PC(
    input clk,              // ʱ��

    input [31:0] offset,    // beq��ƫ��
    input [25:0] absoluteAddr,  // j(��jal)�ľ��Ե�ַ
    input [31:0] registerAddr,  // jr�����ԼĴ����ĵ�ַ

    input branch,           // ���� 1��ѡ��beq��֧(+4+offset*4) 0����һ��(+4)
    input jr,               // ���� 1��ѡ��jr, 0��ѡ��j(jal)
    input jump,             // ���� 1��ѡ��j(jal)��jr, 0��ѡ��beq��+4

    input reset,            // ��ֵ 1��PC���� 

    output reg [31:0] PC,   // ���
    output [31:0] PC_plus4  // PC+1*4 ��jal�洢��ǰλ����
    );

    assign PC_plus4 = PC + 4;    // �������
    wire [31:0] PC_beq = (offset << 2) + PC_plus4;   // beq
    wire [31:0] PC_j = {PC_plus4[31:28], absoluteAddr<<2}; // j(jal)

    wire [31:0] PC_1 = branch ? PC_beq : PC_plus4;
    wire [31:0] PC_2 = jr ? registerAddr : PC_j;
    wire [31:0] PC_final = jump ? PC_2 : PC_1;

    always @(negedge clk or posedge reset)
        PC <= reset ? 0 : PC_final;
endmodule