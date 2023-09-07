`timescale 1ns/1ps
// 要求输入信号至少覆盖一个下降沿 信号全是上升沿触发事件
// 只有ADD没有接入FSM
// 四个报警原有各自单独的触发，所以要一个个解除
module FSM(
    input CLK,
    input BTN_NEXT,
    input BTN_LEFT,
    input BTN_SET,
    input BTN_FUNCTIONAL,   // 功能各异
    // 四个需要报警的源
    input [3:0] ALARM,
    // 四个终止报警源
    input [3:0] STOP,           // 应该外接延时电路取消alarming

    output reg [2:0] SELECT,    // 选择哪个界面
    output reg [7:0] SET,       // 1对应位设置 全0则计时模式
    output reg HOUR_MODE,       // （时钟模式）12/24进制
    output reg [2:0] ALARM_EN,  // 闹钟使能
    output reg [3:0] ALARMING,  // 是否正在响铃(四个源)(高低电平)
    output reg COUNTDOWNING,    // 是否正在倒计时(高低电平)
    output CD_CANCEL            // 脉冲，翻转倒计时状态
);
    // 信号处理: 下降沿锁状态，上升沿切换状态
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
    // 是否有上升沿 以下信号只在下降沿更新，在上升沿是稳定的
    wire btn_next_pos = btn_next_now & ~btn_next_prev;
    wire btn_left_pos = btn_left_now & ~btn_left_prev;
    wire btn_set_pos = btn_set_now & ~btn_set_prev;
    wire btn_functional_pos = btn_functional_now & ~btn_functional_prev;
    wire [3:0] alarm_pos = alarm_now & ~alarm_prev;
    wire [3:0] stop_pos = stop_now & ~stop_prev;

    reg countdown_mode, countdown_pulse; // 一个变化，一个跟随，形成脉冲 同时前者也表示模式
    assign CD_CANCEL = countdown_mode ^ countdown_pulse;
    always @(posedge CLK) begin
        countdown_pulse <= countdown_mode;
    end

    wire [3:0] alarming_temp = (ALARMING | alarm_pos) & ~stop_pos;
    // 状态机 理论上应该缓存事件(用事件队列)防止同时触发 实际上几乎不会同时发生
    always @(posedge CLK) begin
        if (alarming_temp != 4'b0) begin    // 正在响铃
            SET <= 8'b00000000;             // 退出设置模式
            // 闹钟突发事件拥有最高优先级 必须先处理了
            if (btn_functional_pos || btn_left_pos || btn_next_pos || btn_set_pos) begin    // 任意键停止响铃
                ALARMING <= {
                    SELECT==3'b101 ? 0 : alarming_temp[3],
                    SELECT==3'b100 ? 0 : alarming_temp[2],
                    SELECT==3'b011 ? 0 : alarming_temp[1], 
                    SELECT==3'b010 ? 0 : alarming_temp[0]
                };
            end else begin
                ALARMING <= alarming_temp;
                // 跳转至对应界面
                if (alarming_temp[0]) begin
                    SELECT <= 3'b010;
                end else if (alarming_temp[1]) begin
                    SELECT <= 3'b011;
                end else if (alarming_temp[2]) begin
                    SELECT <= 3'b100;
                end else begin  // 倒计时到了
                    COUNTDOWNING <= 0;
                    countdown_mode <= 0;
                    SELECT <= 3'b101;
                end
            end 
        end else begin      // 未响铃 正常逻辑
            ALARMING <= 4'b0;               // STOP清除最后一个 必须加
            if (SET == 8'b00000000) begin   // 非设置模式
                if (btn_set_pos) begin      // 进入设置模式
                    if (SELECT==3'b101 && countdown_mode) begin // 如果正在倒计时，则【退出倒计时】
                        COUNTDOWNING <= 0;
                        countdown_mode <= 0;   // 发生一个脉冲
                    end else begin
                        SET <= 8'b00000001;
                    end
                end else if (btn_next_pos) begin    // 下一个界面
                    if (SELECT==5) SELECT <= 0;
                    else SELECT <= SELECT + 1;
                end else if (btn_functional_pos) begin  // 不同界面功能各异
                    case (SELECT)
                        3'b001: ;
                        3'b010: ALARM_EN[0] <= ~ALARM_EN[0];
                        3'b011: ALARM_EN[1] <= ~ALARM_EN[1];
                        3'b100: ALARM_EN[2] <= ~ALARM_EN[2];
                        3'b101: begin       // 开始、暂停
                            if (countdown_mode) COUNTDOWNING <= ~COUNTDOWNING;  // 如果正在倒计时
                            else begin      // 如果不是倒计时模式，则进入倒计时，且开始
                                countdown_mode <= 1;
                                COUNTDOWNING <= 1;
                            end
                        end
                        default: HOUR_MODE <= ~HOUR_MODE;   // 时钟界面为小时模式切换
                    endcase
                end else ;
            end else begin                  // 设置模式下
                if (btn_left_pos) begin // 左移
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
                    SET <= 8'b00000000; // 退出设置模式
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