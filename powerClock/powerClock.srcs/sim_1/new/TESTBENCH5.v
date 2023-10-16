`timescale 1ns / 1ps

// 闹钟功能测试
module TESTBENCH5();
    reg clk;
    reg [7:0] set;
    reg add;
    wire [31:0] timer_out_origin;
    wire [31:0] alarm_out;
    wire alarms;
    reg enable;
    
    
    always #5 clk = ~clk;
    
    initial begin
       clk = 0;
       add = 0;
       enable =0;
       set = 8'b00000010;
       #10
       add = 1;
       #10
       add = 0;
       #10
       add = 1;
       #10
       add = 0;
       set = 0;
       
    end
    TIMER timer(clk, 0, 0, timer_carry, timer_out_origin);
    ALARM alarm1(clk ,set , add, enable, timer_out_origin, alarm_out, alarms);


endmodule


