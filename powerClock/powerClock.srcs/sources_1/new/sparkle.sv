`timescale 1ns/1ps
// 生成使能信号 选择某几个闪烁
module SPARKLE(
    input CLK,  // 100Hz
    input [7:0] SELECT,     // 1对应位闪烁
    output [7:0] EN
);
    reg [5:0] counter;
    always @(posedge CLK) begin
        counter <= counter + 1;
    end
    assign EN = counter > 45 ? 8'b11111111 ^ SELECT : 8'b11111111;

    initial begin
        counter = 0;
    end
endmodule