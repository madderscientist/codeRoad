// ��д��61821326���
`timescale 1ns / 1ps

module ALU(
    input [3:0] ALUOp,           // ALU��������
    input [31:0] A,              // ����1
    input [31:0] B,              // ����2
    input [4:0]  C,              // ����3����λ��
    output reg zero,             // ������result�ı�־��resultΪ0���1���������0
    output reg [31:0] result     // ALU������
    );
    always@(*)
     begin
        case (ALUOp)
            4'b0000: result = B << C;   // �߼�����
            4'b0001: result = $signed(B) >>> C;   // ��������
            4'b0010: result = B >> C;   // �߼�����
            4'b0011: result = A * B;    // �޷��ų˷�
            4'b0100: result = A / B;    // �޷��ų���
            4'b0101: result = A + B;    // �޷��żӷ�
            4'b0110: result = A - B;    // �޷��ż���
            4'b0111: result = A & B;    // ��λ��
            4'b1000: result = A | B;    // ��λ��
            4'b1001: result = A ^ B;    // ��λ���
            4'b1010: result = ~(A | B); // ��λ���
            4'b1011: result = ($signed(A) < $signed(B)) ? 1 : 0; // �з��űȽ�
            4'b1100: result = (A < B) ? 1 : 0; // �޷��űȽ�
            default : result = 0;
        endcase
        // ����zero
        if (result)  zero = 0;
        else  zero = 1;
     end
endmodule