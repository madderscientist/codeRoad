// ��д��61821326���
`timescale 1ns / 1ps
// ��д32���Ĵ���
module RegisterFile(
    input           clk,        // ʱ��
    input           RegWre,     // дʹ���źţ�Ϊ1ʱ����ʱ��������д��
    input [4:0]     A1,         // rs�Ĵ�����ַ����˿�
    input [4:0]     A2,         // rt�Ĵ�����ַ����˿�
    input [4:0]     WriteReg,   // ������д��ļĴ����˿ڣ����ַ��Դrt��rd�ֶ�
    input [31:0]    WriteData,  // д��Ĵ�������������˿�
    input           reset,      // 1�����
    output[31:0]    ReadData1,  // rs�Ĵ�����������˿�
    output[31:0]    ReadData2,  // rt�Ĵ�����������˿�
    output reg [31:0] register[31:0]    // �������
    );
    // ����
    integer i;

    // ���Ĵ��� ����߼�
    assign ReadData1 = register[A1];
    assign ReadData2 = register[A2];

    // �½���д�Ĵ���
    always@(negedge clk or posedge reset) begin
        if (reset) begin
            for (i = 0; i < 32; i = i + 1)
                register[i] <= 0;
        end
        else begin
            // ����Ĵ�����Ϊ0������RegWreΪ�棬д������
            if (RegWre && WriteReg != 5'b0) 
                register[WriteReg] <= WriteData;
        end
    end 

endmodule