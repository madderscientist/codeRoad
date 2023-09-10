`timescale 1ns / 1ps

module main(
    input CLK,
    input [15:0] SWITCH,    // 最右侧的拨码开关为0
    input [4:0] BUTTON,     // 012345左上右下中
    output [7:0] EN,
    output [7:0] TUBE,
    output [15:0] LED,      // 16个LED
    output RING,            // 耳机口 有滤波器：A11 无滤波器：J8
    // 串口
    input UART_RX,
    input UART_STATE,
    output UART_TX,
    // OLED
    inout OLED_SDA,
    output OLED_CLK,
    // VGA
    output [3:0] VGA_R,
    output [3:0] VGA_G,
    output [3:0] VGA_B,
    output VGA_HS,
    output VGA_VS
);
    // 时钟配置
    wire clk_1k;    // 数码管扫描
    DIVIDER #(16,50000) divider_1k(CLK,clk_1k);
    wire clk_100;
    DIVIDER #(19,500000) divider_100(CLK,clk_100);
    wire clk_ring;  // 声音784Hz
    DIVIDER #(16,63375) divider_ring(CLK,clk_ring);
    wire clk_5;    // 蓝牙发送间隔
    DIVIDER #(4,10) divider_10(clk_100,clk_5);
    // 按钮配置 输出是短脉冲
    wire [4:0] btn;
    BUTTON_loop btn0(clk_100, BUTTON[0], btn[0]);   // 左移
    BUTTON_loop btn1(clk_100, BUTTON[1], btn[1]);   // +1
    BUTTON_loop btn2(clk_100, BUTTON[2], btn[2]);   // 各异
    BUTTON_loop btn3(clk_100, BUTTON[3], btn[3]);   // 换界面
    BUTTON_loop btn4(clk_100, BUTTON[4], btn[4]);   // 设置模式
    // 按钮对应功能 可能多路信号控制一个信号，故设此赋值
    wire [4:0] blueToothBtn;
    wire _left       = btn[0] | blueToothBtn[0];
    wire _add        = btn[1] | blueToothBtn[1];
    wire _functional = btn[2] | blueToothBtn[2];
    wire _next       = btn[3] | blueToothBtn[3];
    wire _setting    = btn[4] | blueToothBtn[4];
    // 信号定义
    wire [31:0] timer_out;
    wire [31:0] date_out;
    wire [31:0] alarm1_out;
    wire [31:0] alarm2_out;
    wire [31:0] alarm3_out;
    wire [31:0] countdown_out;

    // 状态机
    wire [2:0] select;      // 选择哪路信号显示
    wire [7:0] set;         // 设置哪一个
    wire setmode = set!=8'b0;   // 是否在设置模式 1是0否
    wire hourmode;          // 小时12/24进制
    wire [2:0] alarm_en_fsm;// 来自fsm的闹钟使能
    wire [3:0] alarms;      // 闹钟和倒计时的脉冲
    wire [3:0] alarming;
    wire [3:0] alarms_stop;
    wire coundowning;   // 是电平
    wire cd_mode;       // 是脉冲
    FSM fsm(
        clk_100, _next, _left, _setting, _functional,
        alarms, alarms_stop,
        select, set,
        hourmode, alarm_en_fsm, alarming, coundowning, cd_mode
    );

    // 选择界面
    wire [3:0] data_out[0:7];   // 选择后的输出
    wire [31:0] data_out_all = {data_out[7], data_out[6], data_out[5], data_out[4], data_out[3], data_out[2], data_out[1], data_out[0]};
    SELECTOR selector(
        select, timer_out, date_out, alarm1_out, alarm2_out, alarm3_out, countdown_out,
        data_out_all
    );
    // 设置时闪烁一位 报警时全屏闪烁
    wire [7:0] en_spark;
    SPARKLE sparkle(clk_100, alarming==4'b0 ? set : 8'b11111111, en_spark);
    // 数码管驱动
    TUBES tube(clk_1k, en_spark,
        data_out[0], data_out[1], data_out[2], data_out[3], data_out[4], data_out[5], data_out[6], data_out[7],
        select==3'b001 ? 8'b00010100 : 8'b01010100, EN, TUBE
    );
    // LED连接
    LED_DECODER ledDecoder(select, LED[15:10]);
    assign LED[0] = alarm_en_fsm[2];
    assign LED[1] = alarm_en_fsm[1];
    assign LED[2] = alarm_en_fsm[0];

    //== 时钟功能 ==//
    wire timer_carry;   // 向日期的进位
    wire [7:0] hour;    // 设置模式下用24进制
    wire [31:0] timer_out_origin = {hour, timer_out[23:0]};
    HOURMODE hourMode(setmode ? 0 : hourmode, hour[7:4], hour[3:0], timer_out[31:28], timer_out[27:24]);
    // 界面不是时钟时不连接set信号
    TIMER timer(clk_100, select==3'b0 ? set : 8'b0, _add, timer_carry, timer_out_origin);
    //== 日期功能 ==//
    CALENDAR calendar(timer_carry, clk_100, select==3'b001 ? set : 8'b0, _add, date_out);
    //== 闹钟1 ==//
    wire alarm1_enable = alarm_en_fsm[0] && !((select==3'b0 || select==3'b010) && setmode);    // 时钟或闹钟都在设置时，不使能
    ALARM alarm1(clk_100, select==3'b010 ? set : 8'b0, _add, alarm1_enable, timer_out_origin, alarm1_out, alarms[0]);
    wire alarm1_time_ctrl;  // 5s高电平，10s低电平，5s高电平
    ALARM_TIME_1 alarm_time_1(clk_100, alarming[0], alarm1_time_ctrl, alarms_stop[0]);
    //== 闹钟2 ==//
    wire alarm2_enable = alarm_en_fsm[1] && !((select==3'b0 || select==3'b011) && setmode);
    ALARM alarm2(clk_100, select==3'b011 ? set : 8'b0, _add, alarm2_enable, timer_out_origin, alarm2_out, alarms[1]);
    wire alarm2_time_ctrl;
    ALARM_TIME_1 alarm_time_2(clk_100, alarming[1], alarm2_time_ctrl, alarms_stop[1]);
    //== 闹钟3 ==//
    wire alarm3_enable = alarm_en_fsm[2] && !((select==3'b0 || select==3'b100) && setmode);
    ALARM alarm3(clk_100, select==3'b100 ? set : 8'b0, _add, alarm3_enable, timer_out_origin, alarm3_out, alarms[2]);
    wire alarm3_time_ctrl;
    ALARM_TIME_1 alarm_time_3(clk_100, alarming[2], alarm3_time_ctrl, alarms_stop[2]);
    //== 倒计时 ==//
    wire cd_time_ctrl;
    ALARM_TIME_2 cd_time(clk_100, alarming[3], cd_time_ctrl, alarms_stop[3]);
    COUNTDOWN cd(clk_100, _add, select==3'b101 ? set : 8'b0, cd_mode, coundowning, countdown_out, alarms[3]);
    // 耳机输出
    assign RING = clk_ring & (alarm1_time_ctrl | alarm2_time_ctrl | alarm3_time_ctrl | cd_time_ctrl);

    // OLED显示
    reg [2:0] refreshOLED1;
    reg [2:0] refreshOLED2;
    wire refreshOLED = refreshOLED1 != refreshOLED2;
    always @(negedge clk_100) begin
        refreshOLED1 <= select;
        refreshOLED2 <= refreshOLED1;
    end
    oled_main(CLK, ~refreshOLED, select, data_out_all, OLED_CLK, OLED_SDA);
    //== 蓝牙功能 ==//
    SENDER sender(  // 10ms更新一次
        CLK, data_out_all,
        {setmode, select, alarming},
        clk_5 & UART_STATE, UART_STATE, UART_TX
    );
    RECEIVER receiver(CLK, clk_100, UART_RX, UART_STATE, blueToothBtn);
endmodule
// 标题 目的 任务 原理图 状态机流程图 输入情况波形情况(文字解释) 实物结果(拍照) 总结 分工 附录(源程序)
// 功能说明书