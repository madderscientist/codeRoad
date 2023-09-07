// 长脉冲转短脉冲
`timescale 1ns/1ps
// 最完美的版本 对SIGNAL长度无限制
// 上升沿异步步，下降沿同步
module POSEDGE_TO_PULSE (
    input CLK,
    input SIGNAL,
    output PULSE
);
    reg x, y;
    always @(posedge SIGNAL) begin
        x <= ~x;
    end
    always @(negedge CLK) begin // posedge也可，但negedge更短
        y <= x;
    end
    assign PULSE = y ^ x;

    initial begin
        x = 0;
        y = 0;
    end
endmodule

// 要求: SIGNAL长度覆盖一个clk上升沿(如果和clk同时上升沿则不算) 所以一般要大于一个周期的高电平
// 上升沿异步，下降同步
module POSEDGE_TO_PULSE_LONG(
    input CLK,
    input SIGNAL,
    output reg PULSE
);
    reg bef;
    always @(negedge CLK) begin
        bef <= SIGNAL;
    end
    wire clk = CLK & SIGNAL;
    always @(posedge clk) begin
        PULSE = !bef;
    end

    initial begin
        bef = 0;
        PULSE = 0;
    end
endmodule

// 延迟一个周期的版本 对输入信号无长度要求 高电平持续一个周期
module POSEDGE_TO_PULSE_DELAY(
    input CLK,
    input SIGNAL,
    output reg PULSE
);
    reg prev_SIGNAL;

    always @(posedge CLK) begin
        // 如果上一刻是低电平且此时为高电平才高
        PULSE <= !prev_SIGNAL && SIGNAL;
        prev_SIGNAL <= SIGNAL;
    end

    initial begin
        prev_SIGNAL = 0;
        PULSE = 0;
    end
endmodule