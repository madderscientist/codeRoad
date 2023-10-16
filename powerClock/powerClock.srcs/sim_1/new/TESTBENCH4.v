`timescale 1ns / 1ps

// 日期功能测试
module TESTBENCH4();
   reg clk;
   reg timer_carry;
   reg [7:0] set;
   reg add;
   wire [31:0] date_out;
   
   always #5 clk = ~clk;
   always #15 timer_carry = ~timer_carry;

   initial begin
      //date_out = 0;
      clk = 0;
      timer_carry = 0;
      add = 0;
      set = 8'b10000000;
      #20
      add = 1;
      #10
      add = 0;
      #10
      add = 1;
      #10
      add = 0;
      
      #20
      set = 8'b00010000;
      #20
      add = 1;
      #10
      add = 0;
      #10
      add = 1;
      #10
      add = 0;
      #10
      add = 1;
      #10
      add = 0;
      #10
      add = 1;
      #10
      add = 0;
      #20
      set = 8'b00000100;
      #20
      add = 1;
      #10
      add = 0;
      
      
      set = 8'b00000000;
   end
   
   CALENDAR calendar(timer_carry, clk,  set, add, date_out);

endmodule
