`timescale 1ns / 1ps

// UART发送测试
module uart_tx_test();
    reg CLK;      // 100M
    reg [31:0] DATA;
    reg [7:0] DATA2;
    reg EN;       // 长时间的高电平则发送
    reg RST;
    wire TX;

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

    UART_TX #(10_000_000, 100000000) _UART_TX(CLK, RST, tx_flag, tosend, TX, sending);

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
    always #1 CLK = ~CLK;
    initial begin
        pulse_generator = 0;
        pulse_cache = 0;
        sending_prev = 0;
        sending_now = 0;
        select = 0;
        DATA = 32'HA9876543;
        DATA2 = 8'H21;
        CLK = 0;
        EN = 0;
        RST = 1'b1;
        #5 EN = 1;
    end
endmodule