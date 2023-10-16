`timescale 1ns / 1ps

// 时钟功能测试
module TESTBENCH3();
   reg clk;
   wire [31:0]timer_out_origin;
   
   always #5 clk = ~clk;
   initial begin
      clk = 0;
      
   end

   TIMER timer(clk, 0, 0, timer_carry, timer_out_origin);


endmodule
