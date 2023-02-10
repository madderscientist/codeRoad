`timescale 1ns / 1ps
// ��Ƶ��
// 1M Hzһ�������أ�������500000��Ҫ��תһ��
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
