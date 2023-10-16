`timescale 1ns / 1ps

// 倒计时功能测试
module TESTBENCH();
  reg clk;
  reg add;
  reg [7:0]set;
  reg mode;
  reg suspend;
  wire [31:0]count;
  wire pulse;
  
  always #5 clk = ~clk;
  
  initial begin
        clk = 0;
        set = 0;
        add = 1'b0;
        suspend = 1'b1;
        mode = 1'b0;


        #15 set = 8'b00000110;
        add = 1;
        #5 add = 0;
        #5 set = 0;
        #10 mode = 1'b1;
        #10 mode = 1'b0;
        #60 suspend = 1'b0;
        #60 suspend = 1'b1;
        #40 mode = 1'b1;
        #10 mode = 1'b0;
        #40 mode = 1'b1;
        #10 mode = 1'b0;
  end
  
  COUNTDOWN counter(
       .CLK(clk),
       .ADD(add),
       .SET(set),
       .MODE(mode),
       .SUSPEND(suspend),
       .COUNT(count),
       .PULSE(pulse)
  );
  
endmodule
