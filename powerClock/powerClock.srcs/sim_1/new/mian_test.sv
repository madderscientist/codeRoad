`timescale 1ns / 1ps

// COUNTER_ADJUST测试
module main_test();
    reg clk;
    DIVIDER #(3,5) d(clk, CLK);
    reg ifrefresh;
    reg [3:0] max;
    wire carry;
    wire [3:0] count;
    reg set;
    reg add;
    COUNTER_ADJUST x(clk, CLK, set, add, ifrefresh, max, carry, count);
    always #1 clk = ~clk;
    initial begin
        set = 0;
        add = 0;
        ifrefresh = 0;
        max = 9;
        clk = 0;
        #506 ifrefresh = 1;
        max = 3;
        #99 ifrefresh = 0;
        max = 7;
        #10 set = 1;
        #20 add = 1; #10 add = 0;
        #20 add = 1; #10 add = 0;
        #20 add = 1; #10 add = 0;
        #20 add = 1; #10 add = 0;
        #10 set = 0;
    end
endmodule
