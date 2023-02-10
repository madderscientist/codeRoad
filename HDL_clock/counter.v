`timescale 1ns/ 1ps
// bcd计数器
module counter
    #(parameter max = 10)
    (
        input clk,
        input clr,
        output reg [3:0] count,
        output reg c        // 进位
    );

    initial begin   // 初始化置零
        count<=0;c<=0;
    end
    always @(posedge clk or posedge clr) begin
        if (clr) begin
            count<=0;c<=0;
        end
        else begin
            if (count >= max-1) begin
                count<=0;c<=1;
            end
            else begin
                count<=count+1;c<=0;
            end
        end
    end
endmodule