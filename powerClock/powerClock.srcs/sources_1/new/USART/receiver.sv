`timescale 1ns/1ps
// 接收串口。注意点在于100M转100Hz脉冲，因为FSM中用了边沿检测，对信号长度有要求。用了POSEDGE_TO_PULSE的方法。
module RECEIVER(
    input CLK_100M,
    input CLK_100,
    input RX,
    input RST,
    output reg [4:0] BTN
);
    wire rx_flag;
    wire [7:0] rx_data;
    UART_RX #(9600, 100_000_000) _UART_RX(CLK_100M, RST, RX, rx_flag, rx_data);
    // 100M脉冲转100Hz脉冲
    reg [4:0] btn_flag;
    always @(negedge CLK_100M) begin    // 用rx_flag的上升沿或下降沿会有问题，原因未知
        if (rx_flag) begin
            case (rx_data)
                8'b0000_0000: btn_flag[0] <= ~btn_flag[0];
                8'b0000_0001: btn_flag[1] <= ~btn_flag[1]; 
                8'b0000_0010: btn_flag[2] <= ~btn_flag[2]; 
                8'b0000_0011: btn_flag[3] <= ~btn_flag[3]; 
                8'b0000_0100: btn_flag[4] <= ~btn_flag[4]; 
                default: ;
            endcase
        end else ;
    end

    // 检测是否有变化
    reg [4:0] btn_prev;
    reg [4:0] btn_now;
    wire [4:0] btn_change = btn_prev ^ btn_now;
    always @(negedge CLK_100) begin
        btn_prev <= btn_now;
        btn_now <= btn_flag;
    end
    // 措施
    always @(posedge CLK_100) begin
        BTN <= btn_change;
    end
endmodule