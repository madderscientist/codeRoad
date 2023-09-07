`timescale 1ns / 1ps

module main(
    input CLK,
    input [15:0] SWITCH,    // ���Ҳ�Ĳ��뿪��Ϊ0
    input [4:0] BUTTON,     // 012345����������
    output [7:0] EN,
    output [7:0] TUBE,
    output [15:0] LED,      // 16��LED
    output RING             // ������ ���˲�����A11 ���˲�����J8
);
    // ʱ������
    wire clk_1k;    // �����ɨ��
    DIVIDER #(16,50000) divider_1k(CLK,clk_1k);
    wire clk_100;
    DIVIDER #(19,500000) divider_100(CLK,clk_100);
    wire clk_ring;  // ����784Hz
    DIVIDER #(16,63375) divider_ring(CLK,clk_ring);
    // ��ť���� ����Ƕ�����
    wire [4:0] btn;
    BUTTON_loop btn0(clk_100, BUTTON[0], btn[0]);   // ����
    BUTTON_loop btn1(clk_100, BUTTON[1], btn[1]);   // +1
    BUTTON_loop btn2(clk_100, BUTTON[2], btn[2]);   // ����
    BUTTON_loop btn3(clk_100, BUTTON[3], btn[3]);   // ������
    BUTTON_loop btn4(clk_100, BUTTON[4], btn[4]);   // ����ģʽ
    // �źŶ���
    wire [31:0] timer_out;
    wire [31:0] date_out;
    wire [31:0] alarm1_out;
    wire [31:0] alarm2_out;
    wire [31:0] alarm3_out;
    wire [31:0] countdown_out;

    // ״̬��
    wire [2:0] select;      // ѡ����·�ź���ʾ
    wire [7:0] set;         // ������һ��
    wire setmode = set!=8'b0;   // �Ƿ�������ģʽ 1��0��
    wire hourmode;          // Сʱ12/24����
    wire [2:0] alarm_en_fsm;// ����fsm������ʹ��
    wire [3:0] alarms;      // ���Ӻ͵���ʱ������
    wire [3:0] alarming;
    wire [3:0] alarms_stop;
    wire coundowning;   // �ǵ�ƽ
    wire cd_mode;       // ������
    FSM fsm(
        clk_100, btn[3], btn[0], btn[4], btn[2],
        alarms, alarms_stop,
        select, set,
        hourmode, alarm_en_fsm, alarming, coundowning, cd_mode
    );

    // ѡ�����
    wire [3:0] data_out[0:7];   // ѡ�������
    SELECTOR selector(
        select, timer_out, date_out, alarm1_out, alarm2_out, alarm3_out, countdown_out,
        {data_out[7], data_out[6], data_out[5], data_out[4], data_out[3], data_out[2], data_out[1], data_out[0]}
    );
    // ����ʱ��˸һλ ����ʱȫ����˸
    wire [7:0] en_spark;
    SPARKLE sparkle(clk_100, alarming==4'b0 ? set : 8'b11111111, en_spark);
    // ���������
    TUBES tube(clk_1k, en_spark,
        data_out[0], data_out[1], data_out[2], data_out[3], data_out[4], data_out[5], data_out[6], data_out[7],
        select==3'b001 ? 8'b00010100 : 8'b01010100, EN, TUBE
    );
    // LED����
    LED_DECODER ledDecoder(select, LED[15:10]);
    assign LED[0] = alarm_en_fsm[2];
    assign LED[1] = alarm_en_fsm[1];
    assign LED[2] = alarm_en_fsm[0];

    //== ʱ�ӹ��� ==//
    wire timer_carry;   // �����ڵĽ�λ
    wire [7:0] hour;    // ����ģʽ����24����
    wire [31:0] timer_out_origin = {hour, timer_out[23:0]};
    HOURMODE hourMode(setmode ? 0 : hourmode, hour[7:4], hour[3:0], timer_out[31:28], timer_out[27:24]);
    // ���治��ʱ��ʱ������set�ź�
    TIMER timer(clk_100, select==3'b0 ? set : 8'b0, btn[1], timer_carry, timer_out_origin);
    //== ���ڹ��� ==//
    CALENDAR calendar(timer_carry, clk_100, select==3'b001 ? set : 8'b0, btn[1], date_out);
    //== ����1 ==//
    wire alarm1_enable = alarm_en_fsm[0] && !((select==3'b0 || select==3'b010) && setmode);    // ʱ�ӻ����Ӷ�������ʱ����ʹ��
    ALARM alarm1(clk_100, select==3'b010 ? set : 8'b0, btn[1], alarm1_enable, timer_out_origin, alarm1_out, alarms[0]);
    wire alarm1_time_ctrl;  // 5s�ߵ�ƽ��10s�͵�ƽ��5s�ߵ�ƽ
    ALARM_TIME_1 alarm_time_1(clk_100, alarming[0], alarm1_time_ctrl, alarms_stop[0]);
    //== ����2 ==//
    wire alarm2_enable = alarm_en_fsm[1] && !((select==3'b0 || select==3'b011) && setmode);
    ALARM alarm2(clk_100, select==3'b011 ? set : 8'b0, btn[1], alarm2_enable, timer_out_origin, alarm2_out, alarms[1]);
    wire alarm2_time_ctrl;
    ALARM_TIME_1 alarm_time_2(clk_100, alarming[1], alarm2_time_ctrl, alarms_stop[1]);
    //== ����3 ==//
    wire alarm3_enable = alarm_en_fsm[2] && !((select==3'b0 || select==3'b100) && setmode);
    ALARM alarm3(clk_100, select==3'b100 ? set : 8'b0, btn[1], alarm3_enable, timer_out_origin, alarm3_out, alarms[2]);
    wire alarm3_time_ctrl;
    ALARM_TIME_1 alarm_time_3(clk_100, alarming[2], alarm3_time_ctrl, alarms_stop[2]);
    //== ����ʱ ==//
    wire cd_time_ctrl;
    ALARM_TIME_2 cd_time(clk_100, alarming[3], cd_time_ctrl, alarms_stop[3]);
    COUNTDOWN cd(clk_100, btn[1], select==3'b101 ? set : 8'b0, cd_mode, coundowning, countdown_out, alarms[3]);
    // �������
    assign RING = clk_ring & (alarm1_time_ctrl | alarm2_time_ctrl | alarm3_time_ctrl | cd_time_ctrl);
endmodule
// ���� Ŀ�� ���� ԭ��ͼ ״̬������ͼ ��������������(���ֽ���) ʵ����(����) �ܽ� �ֹ� ��¼(Դ����)
// ����˵����