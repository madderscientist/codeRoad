`timescale 1ns/1ps
// ����4λBCD���ж��Ƿ�Ϊ����
// �������ݿ��Ա�4���������ǲ��ܱ�100���������߿��Ա�400����
// ����ʮ������λ��ݵ�BCD�������룬��16λ����ʾΪABCD��
module LEAPYEAR(
    input [3:0] A,
    input [3:0] B,
    input [3:0] C,
    input [3:0] D,
    output reg leapyear
);
    // AB�Ƿ�4��������A*10+B�ܷ�4��������[(A*10)%4+B%4]�ĵ���λ�Ƿ�Ϊ��
    // ���ݹ۲죬��AΪ������A*10%4=2������Ϊ0����B%4�Ľ��ΪB�ĵ���λ
    wire[1:0] ABmod4_temp = ({A[0],1'b0} + B[1:0]);
    wire ABmod4 = ABmod4_temp == 2'b00;     // Ϊ1����4�ı���
    // CD�Ƿ�4����ͬ��
    wire[1:0] CDmod4_temp = ({C[0],1'b0} + D[1:0]);
    wire CDmod4 = CDmod4_temp == 2'b00;
    // ����߼��ж��Ƿ�Ϊ����
    always @(*) begin
        // �ܱ�400������CDȫΪ0����AB�ܱ�4������
        if ({C,D} == 8'b0) begin
            if(ABmod4) leapyear = 1'b1;
            else leapyear = 1'b0;
        // ���Ա�4�����Ҳ��ܱ�100������CD��ȫΪ0����CD�ܱ�4����������ֻҪ�ж���λʮ������AB��CD�ܷ�4������
        end else if (CDmod4) leapyear = 1'b1;
        else leapyear = 1'b0;
    end
endmodule