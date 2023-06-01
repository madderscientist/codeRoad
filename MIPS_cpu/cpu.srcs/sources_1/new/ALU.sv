// 编写：61821326李睿刚
`timescale 1ns / 1ps

module ALU(
    input [3:0] ALUOp,           // ALU操作控制
    input [31:0] A,              // 输入1
    input [31:0] B,              // 输入2
    input [4:0]  C,              // 输入3（移位）
    output reg zero,             // 运算结果result的标志，result为0输出1，否则输出0
    output reg [31:0] result     // ALU运算结果
    );
    always@(*)
     begin
        case (ALUOp)
            4'b0000: result = B << C;   // 逻辑左移
            4'b0001: result = $signed(B) >>> C;   // 算数右移
            4'b0010: result = B >> C;   // 逻辑右移
            4'b0011: result = A * B;    // 无符号乘法
            4'b0100: result = A / B;    // 无符号除法
            4'b0101: result = A + B;    // 无符号加法
            4'b0110: result = A - B;    // 无符号减法
            4'b0111: result = A & B;    // 按位与
            4'b1000: result = A | B;    // 按位或
            4'b1001: result = A ^ B;    // 按位异或
            4'b1010: result = ~(A | B); // 按位或非
            4'b1011: result = ($signed(A) < $signed(B)) ? 1 : 0; // 有符号比较
            4'b1100: result = (A < B) ? 1 : 0; // 无符号比较
            default : result = 0;
        endcase
        // 设置zero
        if (result)  zero = 0;
        else  zero = 1;
     end
endmodule