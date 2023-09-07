`timescale 1ns/1ps
// 分频器 输出频率fo和输入频率fi的关系是：
// fo = fi / (2*MAX)
// 传参要求：2^SIZE >= MAX
// 板子提供100M的CLK 要分为100Hz应该传参(19, 500000)
module DIVIDER #(parameter SIZE = 3, MAX = 5) (
    input CLK,
    output reg DIV
);
    reg [SIZE-1: 0] counter;
    always @(posedge CLK) begin
        if(counter == MAX-1) begin
            counter <= 0;
            DIV <= ~DIV;
        end
        else counter <= counter + 1;
    end

    // 仿真时用 综合时被忽略
    initial begin
        DIV = 0;
        counter = 0;
    end
endmodule