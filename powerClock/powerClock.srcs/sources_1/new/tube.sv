`timescale 1ns/1ps
// ��������� һ��8������ܣ�ÿ���������8λ������
// ����BCD���ݺ�С�������ݣ����8����������ݺ�8������ܵ�ʹ���ź�
// Ӧ����DATA0Ϊ���Ҳ�����ܶ�Ӧ���ݣ�DOT[0]Ϊ���Ҳ�����ܶ�ӦС����
// ENΪʱ���߼�������Ϊ����߼�
module TUBES (
    input CLK,  // 500 ~ 1kHz
    input [7:0] EN_CTRL,    // �ⲿ����ʹ�ܣ���������Ϊ0��λ��Ӧ������ܲ���   
    input [3:0] DATA0,      // verilog��֧�ֶ�ά���飬����ֻ����8��һά��������ʾ8�������(��ȻsystemVerilog֧�ֵ����Ǹ���)
    input [3:0] DATA1,
    input [3:0] DATA2,
    input [3:0] DATA3,
    input [3:0] DATA4,
    input [3:0] DATA5,
    input [3:0] DATA6,
    input [3:0] DATA7,
    input [7:0] DOT,        // 8��С����, DOT[0]��ӦTUBE[0]��С���㣬Ϊ0�Ķ�Ӧ����
    output [7:0] EN,        // 8��ʹ��
    output [7:0] TUBE       // ���õ�8��
);
    reg [7:0] en;
    // en��һ��ѭ����λ��������ɨ��
    always @(posedge CLK) begin
        if (en == 0)
            en <= 8'b00000001;
        else begin
            if (en == 8'b10000000)
                en <= 8'b00000001;
            else
                en <= en << 1;
        end
    end
    assign EN = ~(en & EN_CTRL);

    wire [6:0] decoded;
    reg [3:0] todecode;
    DECODER d(todecode, decoded);
    assign TUBE = {(DOT&en)==0, decoded};

    always @(*) begin
        case (en)
            8'b00000001: todecode = DATA0;
            8'b00000010: todecode = DATA1;
            8'b00000100: todecode = DATA2;
            8'b00001000: todecode = DATA3;
            8'b00010000: todecode = DATA4;
            8'b00100000: todecode = DATA5;
            8'b01000000: todecode = DATA6;
            8'b10000000: todecode = DATA7;
            default: todecode = 4'b0000;
        endcase
    end

    initial begin
        en = 8'b00000001;
    end
endmodule