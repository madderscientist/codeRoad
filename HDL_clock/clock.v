`timescale 1ns / 1ps
// ���˷�Ƶ����clock_core
// ����Ҫ1M Hz
module clock(
        CLK_1M,enable,clr,mode,hourMode,madd,hadd,
        LEDS0,LEDS1,LEDS2,LEDS3
    );
    wire clk, minute_add, hour_add;
    input CLK_1M,enable,clr,mode,hourMode,madd,hadd;
    output [6:0] LEDS0,LEDS1,LEDS2,LEDS3;
    // �������н��ڷ����в����ã�����ɾ����ͬʱӦɾ��clock_core�е��������
    wire [7:0] osecond;wire [7:0] ominute;wire [7:0] ohour;
    btn m(CLK_1M,madd,minute_add);
    btn h(CLK_1M,hadd,hour_add);
    freq f(CLK_1M,clk);
    clock_core c(clk,enable,clr,mode,minute_add,madd,hour_add,hadd,hourMode,osecond,ominute,ohour,LEDS0,LEDS1,LEDS2,LEDS3);
endmodule
