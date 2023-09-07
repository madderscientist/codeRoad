// �ṹ�Ϸ�Ϊ4�飬Ϊʱ���֡��롢10���룬ÿ����2��4λBCD�Ĵ������ֱ��ʾ��λ��ʮλ��ÿλ��Ӧһ��8������ܡ�
// ����12:03:28:98��ʾ12��3��28��980����
// ÿ��BCD��������Ӧһ��SET�źš�����SET�ź�Ϊ��ʱ����ʱ����ͣ�Զ����ӡ�ĳ��������Ӧ��SETΪ��ʱ����ADD�źŵ������ظü�������һ����������λ�źš�[��COUNTER��ʵ��]
// ��SETȫΪ��ʱ����ʱ����10ms��CLK�ź�������ÿ10ms���λ��������һ�������������ʱ����һ�����������һ��
// Ϊ�˷�ֹ����ð�գ�SET�ź�һֱ��CLK�ź������ظ��£�ʵ��ͬ�������Ѿ�����Ҫͬ���ˣ���FSM��ʵ����ͬ����
// �����λ����������������һ����ʾ��λ������c��
// ADDӦ�Ƕ����壬��ģ����Ӧ��CLKͬ������ť������������ͬ����
// �ѵ�����Сʱ��24���Ƶġ�д��һ������Ӧ���ֵ��COUNTER�������CLAENDAR�д˷������㣬��������
`timescale 1ns/1ps

module TIMER (
    input CLK,
    input [7:0] SET,
    input ADD,
    output CARRY,
    output [31:0] TIME
);
    reg [7:0] set;
    reg ifcarry;
    always @(negedge CLK) begin // �½�����״̬������һ�о���ð�գ�
        set <= SET;
        ifcarry <= SET==8'b00000000;
    end

    // ��λ�źŴ���
    wire [7:0] c;       // �Ӽ�����ȡ���Ŀ��ƽ
    wire [6:0] carry;   // ���廯��Ľ�λ
    // �������ģ��Ҫ���źų��ȱ������clk�����c[0]���ܴ���
    // ������ģ���Ѹ��£�����c[0]�������廯
    POSEDGE_TO_PULSE p12(CLK, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(CLK, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(CLK, c[3], carry[3]);
    POSEDGE_TO_PULSE p45(CLK, c[4], carry[4]);
    POSEDGE_TO_PULSE p56(CLK, c[5], carry[5]);
    POSEDGE_TO_PULSE p67(CLK, c[6], carry[6]);
    POSEDGE_TO_PULSE pout(CLK, c[7], CARRY);

    // ������ģʽ�£���λ�䲻��Ҫ��λ����λ���ճ�
    COUNTER #(9) ms2(ifcarry ? CLK      : 0, set[0], ADD, c[0], TIME[3:0]);
    COUNTER #(9) ms1(          c[0],         set[1], ADD, c[1], TIME[7:4]);
    COUNTER #(9) s2 (ifcarry ? carry[1] : 0, set[2], ADD, c[2], TIME[11:8]);
    COUNTER #(5) s1 (          carry[2],     set[3], ADD, c[3], TIME[15:12]);
    COUNTER #(9) m2 (ifcarry ? carry[3] : 0, set[4], ADD, c[4], TIME[19:16]);
    COUNTER #(5) m1 (          carry[4],     set[5], ADD, c[5], TIME[23:20]);
    // Сʱ����24��գ�ʵ���Ϻ��������ƣ�ֻ��Сʱ�ĸ�λ��Ҫ����ˢ��
    wire h1_fresh = set[7];
    wire [3:0] h1_max = TIME[31:28]==4'b0010 ? 4'b0011 : 4'b1001;
    COUNTER_ADJUST h2 (CLK, ifcarry ? carry[5] : 0, set[6], ADD, h1_fresh, h1_max, c[6], TIME[27:24]);
    COUNTER #(2) h1 (          carry[6],     set[7], ADD, c[7], TIME[31:28]);
endmodule