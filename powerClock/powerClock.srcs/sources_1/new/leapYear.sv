`timescale 1ns/1ps
// 根据4位BCD码判断是否为闰年
// 闰年的年份可以被4整除，但是不能被100整除，或者可以被400整除
// 输入十进制四位年份的BCD二进制码，共16位，表示为ABCD。
module LEAPYEAR(
    input [3:0] A,
    input [3:0] B,
    input [3:0] C,
    input [3:0] D,
    output reg leapyear
);
    // AB是否被4整除：即A*10+B能否被4整除，即[(A*10)%4+B%4]的低两位是否为零
    // 根据观察，当A为奇数则A*10%4=2，否则为0；而B%4的结果为B的低两位
    wire[1:0] ABmod4_temp = ({A[0],1'b0} + B[1:0]);
    wire ABmod4 = ABmod4_temp == 2'b00;     // 为1则是4的倍数
    // CD是否被4整除同理
    wire[1:0] CDmod4_temp = ({C[0],1'b0} + D[1:0]);
    wire CDmod4 = CDmod4_temp == 2'b00;
    // 组合逻辑判断是否为闰年
    always @(*) begin
        // 能被400整除：CD全为0，且AB能被4整除。
        if ({C,D} == 8'b0) begin
            if(ABmod4) leapyear = 1'b1;
            else leapyear = 1'b0;
        // 可以被4整除且不能被100整除：CD不全为0，且CD能被4整除，所以只要判断两位十进制数AB或CD能否被4整除：
        end else if (CDmod4) leapyear = 1'b1;
        else leapyear = 1'b0;
    end
endmodule