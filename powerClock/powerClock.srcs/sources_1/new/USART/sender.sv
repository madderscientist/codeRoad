`timescale 1ns/1ps
// 数据发送 难点在于调控UART_TX一个个发送
// 有了之前的经验，应该用CLK沿每时每刻检测信号，而不是某个信号的沿触发。这样做就要用到之前总结的沿检测写法：一个prev一个now。
// 要等待UART_TX发送完。一开始用的检测sending电平，但是发出来的数据很奇怪。后来直接换成检测其下降沿才解决。
// UART_TX的发送需要一个脉冲启动，所以用到了总结的脉冲电路，取反即启动。
// 关于信号的竞争冒险，我直接用CLK的两个沿触发避免了一些纠纷。
// CLK是100M，所以一些时序上的延迟完全感觉不到
module SENDER(
    input CLK,      // 100M
    input [31:0] DATA,
    input [7:0] DATA2,
    input EN,       // 长时间的高电平则发送
    input RST,
    output TX
);
    reg [2:0] select;
    reg [7:0] tosend;
    wire sending;

    // tx_flag是2周期的脉冲，用于启动一次串口
    reg pulse_generator;
    reg pulse_cache;
    wire tx_flag = pulse_generator^pulse_cache;
    always @(posedge CLK) begin
        pulse_cache <= pulse_generator;
    end

    reg sending_prev, sending_now;
    always @(posedge CLK) begin
        sending_prev <= sending_now;
        sending_now <= sending;
    end
    wire sending_neg = sending_prev & ~sending_now;

    UART_TX #(9600, 100000000) _UART_TX(CLK, RST, tx_flag, tosend, TX, sending);

    always @(negedge CLK) begin
        if(!EN) select <= 3'b0;
        else if(select==3'b000||(select<3'b101 && sending_neg)) begin   // 下一位且开始发送
            select <= select + 3'b1;
            pulse_generator <= ~pulse_generator;
        end else select <= select;
    end

    always @(*) begin
        case (select)
            3'b000: tosend = {8{1'b0}};
            3'b001: tosend = DATA[7:0];
            3'b010: tosend = DATA[15:8];
            3'b011: tosend = DATA[23:16];
            3'b100: tosend = DATA[31:24];
            default: tosend = DATA2;
        endcase
    end

    initial begin
        pulse_generator = 0;
        pulse_cache = 0;
        sending_prev = 0;
        sending_now = 0;
        select = 0;
    end
endmodule