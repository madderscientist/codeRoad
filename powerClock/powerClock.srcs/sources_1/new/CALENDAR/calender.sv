// ������8��BCD��������ǰ�ĸ���¼�꣬���ĸ���¼����
// ÿ����������Ӧ����SET[7:0]��һλ������ӦλΪ1ʱ��������ֵ������ADD��������+1
// ����SETȫΪ0ʱ���Զ�ģʽ��������CLK����������+1���������ʱ��λ
// ����SET��ȫΪ0�ǣ�����ģʽ����ͨ��ADD�ı��������ʱ����Ҫ�ж�����ʹ�С�¡�����ձ����ֵ��������Ϊ���ֵ��
// ������֮ǰ���뷨��ʵ��ʱ���ڸ��ӣ�ֱ�ӻ���д����case�ˡ���TIMERģ���е�24Сʱ�������˼·
module CALENDAR(
    input CLK,      // ����TIMER�Ľ�λ
    input REFRESH,  // 100Hz
    input [7:0] SET,
    input ADD,
    output [31:0] DATE
);
    // һͬ���ͻ�����ֵ�bug
    reg [7:0] set = 8'b0;
    reg ifcarry;
    // ͬ��SET�ź�
    always @(negedge REFRESH) begin // ��ͬ������������ͬ����������⣬������SET�ı�ʱDATE��ֵҲ�ڱ䡣
        set <= SET;
        ifcarry <= SET==8'b00000000;
    end

    // ��λ�źŴ���
    wire [6:0] c;       // �Ӽ�����ȡ���Ŀ��ƽ
    wire [6:0] carry;   // ���廯��Ľ�λ
    POSEDGE_TO_PULSE p01(REFRESH, c[0], carry[0]);
    POSEDGE_TO_PULSE p12(REFRESH, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(REFRESH, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(REFRESH, c[3], carry[3]);
    POSEDGE_TO_PULSE p45(REFRESH, c[4], carry[4]);

    /*-- ���ڡ��·ݵ����� ����߼�--*/
    wire leapyear;      // �ж��Ƿ�Ϊ����(1-�ǣ�0-��)
    LEAPYEAR _leapyear(DATE[31:28], DATE[27:24], DATE[23:20], DATE[19:16], leapyear);
    reg day31;
    // �ж��Ƿ�Ϊ����
    always @(*) begin
        case (DATE[15:8])
            8'b0000_0001, 8'b0000_0011, 8'b0000_0101, 8'b0000_0111,
            8'b0000_1000, 8'b0001_0000, 8'b0001_0010: day31 = 1;
            default: day31 = 0;
        endcase
    end
    // �����ֵ
    reg [4:0] d_max;
    always @(*) begin
        if (DATE[15:8]==8'b0000_0010) begin // ��2��
            d_max = leapyear ? 5'd29 : 5'd28;
        end else begin
            d_max = day31 ? 5'd31 : 5'd30;
        end
    end
    // ˢ��ʱ�� ���ں�ǰ������λ���й�ϵ�����µĸ�λ֮��ʮλ�й�ϵ
    wire d_refresh = set[2] | set[3] | set[4] | set[5] | set[6] | set[7];
    /*-- ������ --*/
    DAY day(REFRESH, ifcarry ? CLK : 1'b0, d_refresh, set[1:0], ADD, d_max, c[0], DATE[7:0]);
    MONTH month(ifcarry ? carry[0] : 1'b0, set[3:2], ADD, c[1], DATE[15:8]);
    COUNTER #(9) y4(ifcarry ? carry[1] : 1'b0, set[4], ADD, c[2], DATE[19:16]);
    COUNTER #(9) y3(          carry[2],     set[5], ADD, c[3], DATE[23:20]);
    COUNTER #(9) y2(          carry[3],     set[6], ADD, c[4], DATE[27:24]);
    COUNTER #(9) y1(    // û�н�λ��������Բ���
        .CLK(carry[4]),
        .SET(set[7]),
        .ADD(ADD),
        .COUNT(DATE[31:28])
    );
endmodule