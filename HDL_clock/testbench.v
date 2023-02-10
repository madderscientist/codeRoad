`timescale 1us / 1ns
module testbench();
    reg CLK_1M=0,enable=1,clr=0,mode=0,hourMode=0,madd=0,hadd=0;
    wire [6:0] LEDS0,LEDS1,LEDS2,LEDS3;
    clock c(CLK_1M,enable,clr,mode,hourMode,madd,hadd,
        LEDS0,LEDS1,LEDS2,LEDS3);
    always #0.5 CLK_1M=~CLK_1M;
endmodule



//module testbench();
//    reg clk=0;	 reg enable=1;
//    reg clr=0; 	 reg mode=0;
//reg madd=0;	 reg hadd=0;		// 分钟加1·小时加1
//reg hourMode=0;
//    wire [7:0] osecond;
//    wire [7:0] ominute;
//    wire [7:0] ohour;
//clock_core c(clk,enable,clr,mode,madd,hadd,hourMode,osecond,ominute,ohour);
//initial begin
//// enable、调时间 功能测试
//        #5 enable = 0;
//        #5.1 madd = 1;
//        #1.1 madd = 0;
//        #1.1 hadd = 1;
//        #1 hadd = 0;
        
//        #2 enable = 1;
//        #5 madd = 1;
//        #1 madd = 0;
//        #1 hadd = 1;
//        #1 hadd = 0;
//        // clear功能测试
//        #5 clr = 1;
//        #3 clr = 0;
//    end
//    always #0.5 clk<=~clk;
//endmodule

//module testbench();
//    wire out=0;
//    reg clk = 0;
//    reg sig=0;
//    btn b(clk,sig,out);
//    initial begin
//        #1 sig=1;
//        #1 sig=0;
//        #1 sig=1;
//        #2 sig=0;
//        #1 sig=1;
//        #150 sig = 0;
//        #1 sig = 1;
//        #150000 sig = 0;
//        #1 sig = 1;
//    end
//    always #0.5 clk<=~clk;
//endmodule