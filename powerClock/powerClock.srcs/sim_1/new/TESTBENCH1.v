`timescale 1ns / 1ps
// 界面切换测试
//为了方便仿真，对main函数的端口做了调整。
module TESTBENCH1();
   reg clk;
   reg [4:0] button;
   wire [5:0] led;
   wire [2:0] select;
   wire next;
   always #5 clk = ~clk;
   
   initial begin
   clk =0;
   button = 0;
   
   #20
   button[3] = 1;
   #20 
   button[3] =0; 
   #20 
   
   button[3] = 1;
   #20 
   button[3] =0; 
   #20 
   button[3] = 1;
   #20 
   button[3] =0; 
   #20
   button[3] = 1;
   #20 
   button[3] =0; 
   #20
   button[3] = 1;
   #20 
   button[3] =0; 
   #20
   button[3] = 1;
   #20 
   button[3] =0; 
   #20
   button[3] = 1;
   #20 
   button[3] =0; 
  
   end
   BUTTON_loop btn3(clk, button[3], next);
   FSM fsm(
        clk, next, 0, 0, 0,
        0, 0,
        select, set,
        hourmode, alarm_en_fsm, alarming, coundowning, cd_mode
    );
   LED_DECODER ledDecoder(select, led[5:0]);

endmodule
