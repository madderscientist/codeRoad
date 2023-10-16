`timescale 1ns / 1ps
// 设置模式测试
module TESTBENCH2();
   reg clk;
   reg [2:0] button;
   wire add;
   wire setmode;
   wire left;
   wire [7:0] set;
  // wire [7:0] en_spark;
   wire [31:0] timer_out_origin;
   always #5 clk = ~clk;

   initial begin
      clk = 0;
      button =0;
      //set = 0;
      //en_spark = 0;
      //timer_out_origin = 0;
      
      #20
      button[1] = 1;
      #20
      button[1] = 0;
      #20
      button[2] = 1;
      #20
      button[2] = 0;
      
      #20
      button[2] = 1;
      #20
      button[2] = 0;
      
      #20
      button[2] = 1;
      #20
      button[2] = 0;
      
      #20
      button[2] = 1;
      #20
      button[2] = 0; //3
      
      #15 button[0] = 1;
      #15 button[0] = 0; 
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      
      
      #20
      button[2] = 1; //4
      #20
      button[2] = 0;
      
      #15 button[0] = 1;
      #15 button[0] = 0; 
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;
      #50 button[0] = 1;
      #15 button[0] = 0;

      
      
      
      
      
      #20
      button[2] = 1;
      #20
      button[2] = 0;
   end
   
   BUTTON_loop btn1(clk, button[0], add);   // +1
   BUTTON_loop btn4(clk, button[1], setmode);   // 设置模式
   BUTTON_loop btn0(clk, button[2], left);   // 左移
   FSM fsm(
        clk, 0, left, setmode, 0,
        0, 0,
        select, set,
        hourmode, alarm_en_fsm, alarming, coundowning, cd_mode
    );
   
   TIMER timer(clk,  set, add, timer_carry, timer_out_origin);
   
   
endmodule
