`timescale 1ns/ 1ps
// 按钮除颤 按超过0.1s才发出信号 长按每0.1s发出信号
module btn(
        clk,
        signal,
        out
    );
    input clk,signal;
    output reg out;
    wire clr = ~signal;
    reg [16:0] count=0;
    initial begin
        count<=0;out<=0;
    end
    always @(posedge clk or posedge clr) begin
        if (clr) begin
            count<=0;out<=0;
        end
        else begin
            if (count >= 17'b11000011010011111) begin
                count<=0;out<=1;
            end
            else begin
                count<=count+1;out<=0;
            end
        end
    end
endmodule
