`timescale 1ns/1ps
// ������
// SET = 0ʱ��Ϊ�Զ�ģʽ��CLK���������������λ
// ��λ�ź�CARRY���һ�����ߵ�ƽ��Ϊ�˷�ֹ����ð�գ���Ҫ���ⲿ�Դ��ź����廯
// SET = 1ʱ��Ϊ����ģʽ��ADD����������
// ע�⣬��ģ����ADD��SET���첽�ź�
// Ҫ��֤ʱ�������ģ���⣬��SET ADD CLK���ͬ���ź�
module COUNTER #(parameter MAX = 9) (
    input CLK,
    input SET,
    input ADD,
    output reg CARRY,
    output reg [3:0] COUNT
);
    wire clk = SET ? ADD : CLK;     // �ź��л���ʱ�������������ء���ֻҪSET��CLK�����ظ��¾���
    always @(posedge clk) begin
        if(COUNT == MAX) begin
            COUNT <= 0;
            CARRY <= 1;
        end else begin
            COUNT <= COUNT + 1;
            CARRY <= 0;
        end
    end
    initial begin
        CARRY = 0;
        COUNT = 0;
    end
endmodule

// ����Ӧ���ֵ�ļ�����
// IF_REFRESH: [Ҫˢ��] min (REFRESH����)
// !IF_REFRESH:
//      SET: [�������Լ�] +1 (ADD����)
//      !SET:[���������Լ�] +1 (CLK����)
// ADD SET CLK REFRESH��Ҫ���ⲿͬ��
module COUNTER_ADJUST (
    input REFRESH,
    input CLK,
    input SET,      // �Ƿ���������һ��
    input ADD,
    input IF_REFRESH,
    input [3:0] MAX,
    output reg CARRY,
    output reg [3:0] COUNT
);
    wire clk_1 = SET ? ADD : CLK;
    wire clk = IF_REFRESH ? REFRESH : clk_1;
    always @(posedge clk) begin
        if (IF_REFRESH)
            COUNT <= COUNT > MAX ? MAX : COUNT;
        else begin
            if (COUNT == MAX) begin
                COUNT <= 0;
                CARRY <= 1;
            end else begin
                COUNT <= COUNT + 1;
                CARRY <= 0;
            end
        end
    end
    initial begin
        CARRY = 0;
        COUNT = 0;
    end
endmodule