// ����������ť����һ�ǰ��´���������ǳ�������
`timescale 1ns/1ps

// ��ť ����
// ���´���������
// Ҫ��ťδ����ʱΪ�͵�ƽ
module BUTTON_one (
    input CLK,      // Ӧȡ10ms����ʱ��ͬ��
    input BTN,
    output reg SIGNAL
);
    reg [1:0] count;
    // ԭ�����10ms�����μ�ⶼ�Ǹߵ�ƽ���ж�Ϊ����
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

// ����0.6���ʼÿ��0.3���������������� ����ʱ��һ���ڣ�����һ���½���
module BUTTON_loop (
    input CLK,      // Ӧȡ10ms����ʱ��ͬ��
    input BTN,
    output reg SIGNAL
);
    reg [5:0] count;
    always @(posedge CLK) begin
        if (BTN) begin
            if (count == 60) begin      // ��������
                count <= 30;
            end else begin
                count <= count + 1;
            end
        end else count <= 0;
    end
    // SIGNAL������
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