// ��д��61821326���
`timescale 1ns / 1ps

// ��ȡ��ַֻ��14λ������ͬrom
module DataMem(
    input           clk,        // ʱ��
    input [31:0]    address,    // ���� memorio ģ�飬������ALU
    input [31:0]    write_data, // �������뵥Ԫ�� read_data2
    input           Memwrite,   // ���Կ��Ƶ�Ԫ
    output[31:0]    read_data   // �Ӵ洢���л�õ�����
);
    wire clock = !clk; // ��Ϊʹ��оƬ�Ĺ����ӳ٣�RAM �ĵ�ַ����������ʱ��������׼����, ʹ��ʱ�����������ݶ����������Բ��÷���ʱ�ӣ�ʹ�ö������ݱȵ�ַ׼����Ҫ���Լ���ʱ�ӣ��Ӷ��õ���ȷ��ַ��
    // ���� 64KB RAM
    ram ram (
        .clka(clock),             // input wire clka
        .wea(Memwrite),         // input wire [0 : 0] wea
        .addra(address[15:2]),  // input wire [13 : 0] addra
        .dina(write_data),      // input wire [31 : 0] dina
        .douta(read_data)       // output wire [31 : 0] douta
    );
endmodule
