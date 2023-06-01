// 编写：61821326李睿刚
`timescale 1ns / 1ps

// 正常情况：PC+1*4，选下一条
// beq: op rs rt offset[15:0] 给出相对偏移量
// j:   op addr[25:0]         给出绝对位置
// jr:  op rs 0 0 0 funct     读寄存器得到绝对位置
// jal: op addr[25:0]         给出绝对位置 并保存当前地址+1*4

module PC(
    input clk,              // 时钟

    input [31:0] offset,    // beq的偏置
    input [25:0] absoluteAddr,  // j(或jal)的绝对地址
    input [31:0] registerAddr,  // jr的来自寄存器的地址

    input branch,           // 控制 1则选择beq分支(+4+offset*4) 0则下一条(+4)
    input jr,               // 控制 1则选择jr, 0则选择j(jal)
    input jump,             // 控制 1则选择j(jal)或jr, 0则选择beq或+4

    input reset,            // 初值 1则PC清零 

    output reg [31:0] PC,   // 输出
    output [31:0] PC_plus4  // PC+1*4 给jal存储当前位置用
    );

    assign PC_plus4 = PC + 4;    // 正常情况
    wire [31:0] PC_beq = (offset << 2) + PC_plus4;   // beq
    wire [31:0] PC_j = {PC_plus4[31:28], absoluteAddr<<2}; // j(jal)

    wire [31:0] PC_1 = branch ? PC_beq : PC_plus4;
    wire [31:0] PC_2 = jr ? registerAddr : PC_j;
    wire [31:0] PC_final = jump ? PC_2 : PC_1;

    always @(negedge clk or posedge reset)
        PC <= reset ? 0 : PC_final;
endmodule