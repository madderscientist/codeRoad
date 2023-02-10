`timescale 1ns/ 1ps
// 时钟
module clock_core(
        input clk,
        input enable,   // 1使能, 0禁用
        input clr,      // 1清零
        input mode,     // 高电平显示时分, 低电平显示分秒
        input madd,     // 分钟加1
        input madd_sig, // 是否正在加一
        input hadd,     // 小时加1
        input hadd_sig, // 是否正在加一
        input hourMode, // 小时输出模式 1为12制
        // 仿真中展示每个计数器的输出 可删除
        output [7:0] osecond,
        output [7:0] ominute,
        output [7:0] ohour,
        // 四个LED
        output [6:0] Led1,
        output [6:0] Led2,
        output [6:0] Led3,
        output [6:0] Led4
    );
    // 计数器维护
    wire [3:0] cout [5:0];  // 每个计数器的bcd输出
    wire carry [4:0];       // 计数器进位
    wire hourclr = (cout[4][2]&cout[5][1])|clr; // 小时清零(满24清零
    counter #(10)   s1(clk&enable, clr, cout[0], carry[0]);
    counter #(6)    s2(carry[0], clr, cout[1], carry[1]);
    counter #(10)   m1(madd_sig?madd:carry[1], clr, cout[2], carry[2]);
    counter #(6)    m2(carry[2], clr, cout[3], carry[3]);
    counter #(10)   h1(hadd_sig?hadd:carry[3], hourclr, cout[4], carry[4]);
    counter #(10)   h2(carry[4], hourclr, cout[5]);

    // 小时模式
    wire [7:0] HOUR;
    hourShow S(hourMode,{cout[5],cout[4]},HOUR);
    
    // LED输出
    wire [3:0] decodeIn [3:0];
    decoder led1(decodeIn[0], Led1);
    decoder led2(decodeIn[1], Led2);
    decoder led3(decodeIn[2], Led3);
    decoder led4(decodeIn[3], Led4);

    // 选择时-分 或 分-秒
    assign decodeIn[0] = mode?cout[2]:cout[0];
    assign decodeIn[1] = mode?cout[3]:cout[1];
    assign decodeIn[2] = mode?HOUR[3:0]:cout[2];
    assign decodeIn[3] = mode?HOUR[7:4]:cout[3];
    
    // 仿真用 可删除
    assign osecond = {cout[1],cout[0]};
    assign ominute = {cout[3],cout[2]};
    assign ohour = {HOUR[7:4],HOUR[3:0]};
endmodule