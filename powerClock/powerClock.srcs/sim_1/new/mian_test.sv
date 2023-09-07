`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/08/30 10:38:32
// Design Name: 
// Module Name: mian_test
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module main_test();

    reg CLK;      // 来自TIMER的进位
    reg REFRESH;  // 100Hz
    reg [7:0] SET;
    reg ADD;
    wire [31:0] DATE;
    wire [3:0] D2 = DATE[3:0];
    wire [3:0] D1 = DATE[7:4];
    wire [3:0] M2 = DATE[11:8];
    wire [3:0] M1 = DATE[15:12];

    always #5 REFRESH = ~REFRESH;


    reg [7:0] set;
    reg ifcarry;    // 此信号必须在此同步，不然在切换的时候会"泄露"时钟，导致最后一位+1
    // 同步SET信号
    always @(posedge REFRESH) begin
        set <= SET;
        ifcarry <= SET==8'b00000000;
    end

    // 进位信号处理
    wire [6:0] c;       // 从计数器取到的宽电平
    wire [6:0] carry;   // 脉冲化后的进位
    POSEDGE_TO_PULSE p01(REFRESH, c[0], carry[0]);
    POSEDGE_TO_PULSE p12(REFRESH, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(REFRESH, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(REFRESH, c[3], carry[3]);
    POSEDGE_TO_PULSE p45(REFRESH, c[4], carry[4]);
    POSEDGE_TO_PULSE p56(REFRESH, c[5], carry[5]);
    POSEDGE_TO_PULSE p67(REFRESH, c[6], carry[6]);

    /*-- 日期、月份的限制 组合逻辑--*/
    wire leapyear;      // 判断是否为闰年(1-是，0-否)
    LEAPYEAR _leapyear(DATE[31:28], DATE[27:24], DATE[23:20], DATE[19:16], leapyear);
    reg day31;
    reg [3:0] d2_max;
    reg [3:0] d1_max;
    // 判断是否为大月
    always @(*) begin
        case ({DATE[15:8]})
            8'b0000_0001, 8'b0000_0011, 8'b0000_0101, 8'b0000_0111,
            8'b0000_1000, 8'b0001_0000, 8'b0001_0010: day31 = 1;
            default: day31 = 0;
        endcase
    end
    // 日最大值
    always @(*) begin
        if ({DATE[15:8]}==8'b0000_0010) begin   // 如果是2月
            d1_max = 4'b0010;
            if (DATE[7:4]==8'b0010) begin   // 如果十位为2
                if (leapyear) d2_max = 4'b1001;     // 如果是闰年
                else d2_max = 4'b1000;              // 如果是平年
            end else begin                  // 十位不为2
                d2_max = 4'b1001;
            end
        end else begin                  // 不是2月
            d1_max = 4'b0011;
            if (DATE[7:4]==8'b0011) begin   // 如果十位为3
                if (day31) d2_max = 4'b0001;        // 如果是大月
                else d2_max = 4'b0000;              // 如果是小月
            end else begin                  // 十位不为3
                d2_max = 4'b1001;
            end 
        end
    end
    // 月最大值
    wire [3:0] m2_max = DATE[15:12]==4'b0 ? 4'b1001 : 4'b0010;
    // 最小值 需要考虑进位和设置
    wire [3:0] d2_min = DATE[7:4]==d1_max||{DATE[7:0]}==8'b0 ? 4'b0001 : 4'b0000;
    wire [3:0] m2_min = DATE[15:12]==4'b0001 ? 4'b0001 : 4'b0000;
    // 刷新时机
    // 日期和前面所有位都有关系，而月的个位之和十位有关系
    wire d2_refresh = set[1] | set[2] | set[3] | set[4] | set[5] | set[6] | set[7];
    wire d1_refresh = set[2] | set[3] | set[4] | set[5] | set[6] | set[7];
    wire m2_refresh = set[3];

    /*-- 计数器 --*/
    COUNTER_ADJUST_MAX_MIN d2(REFRESH, ifcarry ? CLK      : 0, set[0], ADD, d2_refresh, d2_max, d2_min, c[0], DATE[3:0]);
    COUNTER_ADJUST d1(REFRESH, carry[0],    set[1], ADD, d1_refresh, d1_max, c[1], DATE[7:4]);
    COUNTER_ADJUST_MAX_MIN m2(REFRESH, ifcarry ? carry[1] : 0, set[2], ADD, m2_refresh, m2_max, m2_min, c[2], DATE[11:8]);
    COUNTER #(1) m1(          carry[2],     set[3], ADD, c[3], DATE[15:12]);
    COUNTER #(9) y4(ifcarry ? carry[3] : 0, set[4], ADD, c[4], DATE[19:16]);
    COUNTER #(9) y3(          carry[4],     set[5], ADD, c[5], DATE[23:20]);
    COUNTER #(9) y2(          carry[5],     set[6], ADD, c[6], DATE[27:24]);
    COUNTER #(9) y1(    // 没有进位输出，所以不传
        .CLK(carry[6]),
        .SET(set[7]),
        .ADD(ADD),
        .COUNT(DATE[31:28])
    );

    integer i;
    initial begin
        SET = 8'b00000001;
        set = 0;
        CLK = 0;
        REFRESH = 0;
        ADD = 0;
        #15;
        for (i = 0; i<28 ; i=i+1) begin
            #10 ADD = 1;
            #10 ADD = 0;
        end
        #10 SET = 8'b00000010;
        #20 SET = 8'b00000100;
        #20 SET = 8'b00000001;
        #20 SET = 8'b00000010;
        #20 SET = 8'b00000100;
    end
endmodule

// hour_mode
// module main_test();
//     reg [3:0] h1, h2;
//     wire [3:0] ho1, ho2;
//     reg mode;
//     HOURMODE HM(mode, h1, h2, ho1, ho2);
//     initial begin
//         mode = 0;
//         h1 = 0;
//         h2 = 0;
//         #10 h2 = 1;
//         #10 h2 = 2;
//         #10 h2 = 3;
//         #10 h2 = 4;
//         #10 h1 = 1;
//         #10 h2 = 5;
//     end
// endmodule

// FSM测试
// module main_test();
//     reg clk;
//     reg [3:0]alarm;
//     reg [3:0]stop;
//     wire [3:0] alarming;
//     FSM fsm(
//         .CLK(clk),
//         .ALARM(alarm),
//         .STOP(stop),
//         .ALARMING(alarming)
//     );
//     always #5 clk = ~clk;
//     initial begin
//         clk = 0;
//         alarm = 0;
//         stop = 0;
//         #15 alarm[0] = 1;
//         #10 alarm[0] = 0;
//         #100 stop[0] = 1;
//         #10 stop[0] = 0; 
//     end
// endmodule

// COUNTER_ADJUST测试
// module main_test();
//     reg clk;
//     DIVIDER #(3,5) d(clk, CLK);
//     reg ifrefresh;
//     reg [3:0] max;
//     wire carry;
//     wire [3:0] count;
//     reg set;
//     reg add;
//     COUNTER_ADJUST x(clk, CLK, set, add, ifrefresh, max, carry, count);
//     always #1 clk = ~clk;
//     initial begin
//         set = 0;
//         add = 0;
//         ifrefresh = 0;
//         max = 9;
//         clk = 0;
//         #506 ifrefresh = 1;
//         max = 3;
//         #99 ifrefresh = 0;
//         max = 7;
//         #10 set = 1;
//         #20 add = 1; #10 add = 0;
//         #20 add = 1; #10 add = 0;
//         #20 add = 1; #10 add = 0;
//         #20 add = 1; #10 add = 0;
//         #10 set = 0;
//     end
// endmodule
