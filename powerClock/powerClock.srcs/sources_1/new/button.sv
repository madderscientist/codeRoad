// 包含两个按钮，其一是按下触发，其二是长按连续
`timescale 1ns/1ps

// 按钮 除颤
// 按下触发短脉冲
// 要求按钮未按下时为低电平
module BUTTON_one (
    input CLK,      // 应取10ms，和时钟同步
    input BTN,
    output reg SIGNAL
);
    reg [1:0] count;
    // 原理：间隔10ms的两次检测都是高电平则判定为触发
    always @(posedge CLK) begin
        if (BTN) begin
            if (count != 2'b11)
                count <= count + 1;
            else ;
        end else count <= 0;
    end
    always @(posedge CLK) begin
        if (SIGNAL) SIGNAL <= 0;
        else if(count == 2) SIGNAL <= 1;
        else ;
    end

    initial begin
        count = 0;
        SIGNAL = 0;
    end
endmodule

// 按下0.6秒后开始每隔0.3秒连续触发短脉冲 脉冲时长一周期，覆盖一个下降沿
module BUTTON_loop (
    input CLK,      // 应取10ms，和时钟同步
    input BTN,
    output reg SIGNAL
);
    reg [5:0] count;
    always @(posedge CLK) begin
        if (BTN) begin
            if (count == 60) begin      // 连续触发
                count <= 30;
            end else begin
                count <= count + 1;
            end
        end else count <= 0;
    end
    // SIGNAL短脉冲
    always @(posedge CLK) begin
        if (SIGNAL) SIGNAL <= 0;
        else if(count == 60 || count == 2) SIGNAL <= 1;
        else ;
    end

    initial begin
        count = 0;
        SIGNAL = 0;
    end
endmodule