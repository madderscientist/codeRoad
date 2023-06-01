//`timescale 1ns / 1ps

//module TEST;
//    reg clk = 1;
//    always #10 clk = ~clk;
//    reg rf = 0;
//    reg pc = 0;
//    wire [31:0]PC;
//    wire [31:0] register[31:0];
//    MIPS m(clk,pc,rf,PC,register);
//    initial begin
//        #2 rf = 1; pc = 1;
//        #2 rf = 0; pc = 0;
//    end
//endmodule