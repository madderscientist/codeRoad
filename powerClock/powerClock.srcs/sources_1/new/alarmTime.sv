`timescale 1ns/1ps
// 报警时间控制。FSM会输出一个电平信号表示是否正在报警，同时接收一个停止的脉冲以置零此信号
// 本模块的作用是让电平信号持续一段事件自动停止，同时输出需要的时间波形

// 收到TRIG的上升沿后，5s高电平，10秒低电平，5秒高电平，停止
module ALARM_TIME_1 (
    input CLK,      // 100Hz
    input TRIG,     // 电平信号
    output CTRL,    // 控制信号
    output STOP     // 脉冲信号，使FSM将TRIG置零
);
    reg [10:0] cnt;     // 共2048个时钟周期，20.48秒
    always @(posedge CLK) begin
        if (TRIG) begin
            cnt <= cnt + 1;
        end else begin
            cnt <= 11'b0;
        end
    end
    assign CTRL = (cnt>1500)||(cnt>0&&cnt<500);
    assign STOP = cnt == 11'd2002;
endmodule

// 收到TRIG的上升沿后，5s高电平，停止
module ALARM_TIME_2 (
    input CLK,      // 100Hz
    input TRIG,     // 电平信号
    output CTRL,    // 控制信号
    output STOP     // 脉冲信号，使FSM将TRIG置零
);
    reg [8:0] cnt;     // 共512个时钟周期，5.12秒
    always @(posedge CLK) begin
        if (TRIG) begin
            cnt <= cnt + 1;
        end else begin
            cnt <= 11'b0;
        end
    end
    assign CTRL = cnt != 9'b0;
    assign STOP = cnt == 9'd502;
endmodule