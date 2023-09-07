`timescale 1ns/1ps
// ��Ƶ�� ���Ƶ��fo������Ƶ��fi�Ĺ�ϵ�ǣ�
// fo = fi / (2*MAX)
// ����Ҫ��2^SIZE >= MAX
// �����ṩ100M��CLK Ҫ��Ϊ100HzӦ�ô���(19, 500000)
module DIVIDER #(parameter SIZE = 3, MAX = 5) (
    input CLK,
    output reg DIV
);
    reg [SIZE-1: 0] counter;
    always @(posedge CLK) begin
        if(counter == MAX-1) begin
            counter <= 0;
            DIV <= ~DIV;
        end
        else counter <= counter + 1;
    end

    // ����ʱ�� �ۺ�ʱ������
    initial begin
        DIV = 0;
        counter = 0;
    end
endmodule