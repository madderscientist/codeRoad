`timescale 1ns / 1ps
// 分频器
// 1M Hz一个上升沿，即计数500000次要翻转一次
module freq(
        clk_in,
        clk_out
    );
    input clk_in;
    output reg clk_out=0;
    reg [18:0] c = 0;   // 2^19 > 500000
    always @(posedge clk_in) begin
        if(c>=19'b1111010000100011111) begin    // 499999
            c<=0;
            clk_out<=~clk_out;
        end
        else c<=c+1;
    end
endmodule
