`timescale 1ns/1ps
// Ҫ�������ź����ٸ���һ���½��� �ź�ȫ�������ش����¼�
// ֻ��ADDû�н���FSM
// �ĸ�����ԭ�и��Ե����Ĵ���������Ҫһ�������
module FSM(
    input CLK,
    input BTN_NEXT,
    input BTN_LEFT,
    input BTN_SET,
    input BTN_FUNCTIONAL,   // ���ܸ���
    // �ĸ���Ҫ������Դ
    input [3:0] ALARM,
    // �ĸ���ֹ����Դ
    input [3:0] STOP,           // Ӧ�������ʱ��·ȡ��alarming

    output reg [2:0] SELECT,    // ѡ���ĸ�����
    output reg [7:0] SET,       // 1��Ӧλ���� ȫ0���ʱģʽ
    output reg HOUR_MODE,       // ��ʱ��ģʽ��12/24����
    output reg [2:0] ALARM_EN,  // ����ʹ��
    output reg [3:0] ALARMING,  // �Ƿ���������(�ĸ�Դ)(�ߵ͵�ƽ)
    output reg COUNTDOWNING,    // �Ƿ����ڵ���ʱ(�ߵ͵�ƽ)
    output CD_CANCEL            // ���壬��ת����ʱ״̬
);
    // �źŴ���: �½�����״̬���������л�״̬
    reg btn_next_prev, btn_next_now;
    reg btn_left_prev, btn_left_now;
    reg btn_set_prev, btn_set_now;
    reg btn_functional_prev, btn_functional_now;
    reg [3:0] alarm_prev;
    reg [3:0] alarm_now;
    reg [3:0] stop_prev;
    reg [3:0] stop_now;
    always @(negedge CLK) begin
        btn_next_prev <= btn_next_now;  btn_next_now <= BTN_NEXT;
        btn_left_prev <= btn_left_now; btn_left_now <= BTN_LEFT;
        btn_set_prev <= btn_set_now; btn_set_now <= BTN_SET;
        btn_functional_prev <= btn_functional_now; btn_functional_now <= BTN_FUNCTIONAL;
        alarm_prev <= alarm_now; alarm_now <= ALARM;
        stop_prev <= stop_now; stop_now <= STOP;
    end
    // �Ƿ��������� �����ź�ֻ���½��ظ��£������������ȶ���
    wire btn_next_pos = btn_next_now & ~btn_next_prev;
    wire btn_left_pos = btn_left_now & ~btn_left_prev;
    wire btn_set_pos = btn_set_now & ~btn_set_prev;
    wire btn_functional_pos = btn_functional_now & ~btn_functional_prev;
    wire [3:0] alarm_pos = alarm_now & ~alarm_prev;
    wire [3:0] stop_pos = stop_now & ~stop_prev;

    reg countdown_mode, countdown_pulse; // һ���仯��һ�����棬�γ����� ͬʱǰ��Ҳ��ʾģʽ
    assign CD_CANCEL = countdown_mode ^ countdown_pulse;
    always @(posedge CLK) begin
        countdown_pulse <= countdown_mode;
    end

    wire [3:0] alarming_temp = (ALARMING | alarm_pos) & ~stop_pos;
    // ״̬�� ������Ӧ�û����¼�(���¼�����)��ֹͬʱ���� ʵ���ϼ�������ͬʱ����
    always @(posedge CLK) begin
        if (alarming_temp != 4'b0) begin    // ��������
            SET <= 8'b00000000;             // �˳�����ģʽ
            // ����ͻ���¼�ӵ��������ȼ� �����ȴ�����
            if (btn_functional_pos || btn_left_pos || btn_next_pos || btn_set_pos) begin    // �����ֹͣ����
                ALARMING <= {
                    SELECT==3'b101 ? 0 : alarming_temp[3],
                    SELECT==3'b100 ? 0 : alarming_temp[2],
                    SELECT==3'b011 ? 0 : alarming_temp[1], 
                    SELECT==3'b010 ? 0 : alarming_temp[0]
                };
            end else begin
                ALARMING <= alarming_temp;
                // ��ת����Ӧ����
                if (alarming_temp[0]) begin
                    SELECT <= 3'b010;
                end else if (alarming_temp[1]) begin
                    SELECT <= 3'b011;
                end else if (alarming_temp[2]) begin
                    SELECT <= 3'b100;
                end else begin  // ����ʱ����
                    COUNTDOWNING <= 0;
                    countdown_mode <= 0;
                    SELECT <= 3'b101;
                end
            end 
        end else begin      // δ���� �����߼�
            ALARMING <= 4'b0;               // STOP������һ�� �����
            if (SET == 8'b00000000) begin   // ������ģʽ
                if (btn_set_pos) begin      // ��������ģʽ
                    if (SELECT==3'b101 && countdown_mode) begin // ������ڵ���ʱ�����˳�����ʱ��
                        COUNTDOWNING <= 0;
                        countdown_mode <= 0;   // ����һ������
                    end else begin
                        SET <= 8'b00000001;
                    end
                end else if (btn_next_pos) begin    // ��һ������
                    if (SELECT==5) SELECT <= 0;
                    else SELECT <= SELECT + 1;
                end else if (btn_functional_pos) begin  // ��ͬ���湦�ܸ���
                    case (SELECT)
                        3'b001: ;
                        3'b010: ALARM_EN[0] <= ~ALARM_EN[0];
                        3'b011: ALARM_EN[1] <= ~ALARM_EN[1];
                        3'b100: ALARM_EN[2] <= ~ALARM_EN[2];
                        3'b101: begin       // ��ʼ����ͣ
                            if (countdown_mode) COUNTDOWNING <= ~COUNTDOWNING;  // ������ڵ���ʱ
                            else begin      // ������ǵ���ʱģʽ������뵹��ʱ���ҿ�ʼ
                                countdown_mode <= 1;
                                COUNTDOWNING <= 1;
                            end
                        end
                        default: HOUR_MODE <= ~HOUR_MODE;   // ʱ�ӽ���ΪСʱģʽ�л�
                    endcase
                end else ;
            end else begin                  // ����ģʽ��
                if (btn_left_pos) begin // ����
                    case (SET)
                        8'b00000001: SET <= 8'b00000010;
                        8'b00000010: SET <= 8'b00000100;
                        8'b00000100: SET <= 8'b00001000;
                        8'b00001000: SET <= 8'b00010000;
                        8'b00010000: SET <= 8'b00100000;
                        8'b00100000: SET <= 8'b01000000;
                        8'b01000000: SET <= 8'b10000000;
                        default: SET <= 8'b00000001;
                    endcase
                end else if (btn_set_pos) begin
                    SET <= 8'b00000000; // �˳�����ģʽ
                end
            end
        end
    end

    initial begin
        btn_next_prev = 0; btn_next_now = 0;
        btn_left_prev = 0; btn_left_now = 0;
        btn_set_prev = 0; btn_set_now = 0;
        btn_functional_prev = 0; btn_functional_now = 0;
        alarm_prev = 0; alarm_now = 0;
        stop_prev = 0; stop_now = 0;
        countdown_mode = 0;
        countdown_pulse = 0;
        SELECT = 0; SET = 0; HOUR_MODE = 0; COUNTDOWNING = 0;
        ALARM_EN = 0; ALARMING = 0;
    end
endmodule