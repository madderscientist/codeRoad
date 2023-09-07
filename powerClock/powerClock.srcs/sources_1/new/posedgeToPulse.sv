// ������ת������
`timescale 1ns/1ps
// �������İ汾 ��SIGNAL����������
// �������첽�����½���ͬ��
module POSEDGE_TO_PULSE (
    input CLK,
    input SIGNAL,
    output PULSE
);
    reg x, y;
    always @(posedge SIGNAL) begin
        x <= ~x;
    end
    always @(negedge CLK) begin // posedgeҲ�ɣ���negedge����
        y <= x;
    end
    assign PULSE = y ^ x;

    initial begin
        x = 0;
        y = 0;
    end
endmodule

// Ҫ��: SIGNAL���ȸ���һ��clk������(�����clkͬʱ����������) ����һ��Ҫ����һ�����ڵĸߵ�ƽ
// �������첽���½�ͬ��
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

// �ӳ�һ�����ڵİ汾 �������ź��޳���Ҫ�� �ߵ�ƽ����һ������
module POSEDGE_TO_PULSE_DELAY(
    input CLK,
    input SIGNAL,
    output reg PULSE
);
    reg prev_SIGNAL;

    always @(posedge CLK) begin
        // �����һ���ǵ͵�ƽ�Ҵ�ʱΪ�ߵ�ƽ�Ÿ�
        PULSE <= !prev_SIGNAL && SIGNAL;
        prev_SIGNAL <= SIGNAL;
    end

    initial begin
        prev_SIGNAL = 0;
        PULSE = 0;
    end
endmodule