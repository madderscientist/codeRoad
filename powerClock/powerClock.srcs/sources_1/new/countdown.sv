`timescale 1ns/1ps
// 倒计时
module COUNTDOWN(
    input CLK,              // 100Hz
    input ADD,
    input [7:0] SET,
    input MODE,             // 脉冲，切换模式用
    input SUSPEND,          // 长时间的高低电平
    output [31:0] COUNT,
    output PULSE
);
    wire [31:0] time0;
    COUNTER_SET_99 counter_set(CLK, SET, ADD, time0);
    
    reg [3:0] ms_1;     //毫秒个位
    reg [3:0] ms_10;    //毫秒十位
    reg [3:0] s_1;      //秒个位
    reg [3:0] s_10;     //秒十位
    reg [3:0] m_1;      //分个位
    reg [3:0] m_10;     //分十位
    reg [3:0] h_1;      //时个位
    reg [3:0] h_10;     //时十位  
    reg [6:0] cnt;
    reg if_pulse;
    reg mode;           // 模式

    assign COUNT = mode ? {h_10,h_1,m_10,m_1,s_10,s_1,ms_10,ms_1} : time0;
    wire suspend = mode & SUSPEND;
    
    reg MODE_pre,MODE_now;
    always @(negedge CLK) begin
        MODE_pre <= MODE_now; MODE_now <= MODE;
    end
    wire MODE_pos = MODE_now & ~MODE_pre;

    always @(posedge CLK) begin
        if(MODE_pos)begin
            mode <= ~mode;
            if (!mode) begin
                // 赋值
                ms_1 <= time0[3:0]; 
                ms_10 <= time0[7:4];
                s_1 <= time0[11:8];
                s_10 <= time0[15:12];
                m_1 <= time0[19:16];
                m_10 <= time0[23:20];
                h_1 <= time0[27:24];
                h_10 <= time0[31:28];
                cnt <= 7'b0000000;
                if_pulse <= 0;
            end else ;
        end else begin
            if (suspend) begin
                //倒计时
                cnt <= cnt + 7'b1;
                //当计满十毫秒，且十毫秒位大于0时，--》十毫秒位减1
                if(cnt !=100 && ms_1 > 4'b0) ms_1 <= ms_1 - 4'b1;
                //当计满十毫秒，且十毫秒位等于0，百毫秒位大于0，--》百毫秒位减1，十毫秒位变9。
                else if(cnt !=100 && ms_1 == 4'b0 && ms_10 > 4'b0 ) begin
                    ms_10 <= ms_10 - 4'b1;
                    ms_1 <= 4'b1001;
                end
                //当计满一毫秒，且毫秒为00，其他位中有大于0的位时，--》毫秒由00变为99.
                else if(cnt !=100 && ms_1 == 4'b0 && ms_10 == 4'b0 && (s_1 || s_10 || m_1 || m_10 || h_1 || h_10) ) begin
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                    s_1 <= s_1-7'b0000001;
                    cnt <= 7'b0000001;
                end
                //当计满一秒，且秒的个位大于0时，--》秒的个位减1
                else if(cnt == 7'b1100100 && s_1 > 4'b0) begin
                    s_1 <= s_1 - 4'b1;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end
                //当计满一秒，且秒的个位等于0，秒的十位大于0时，--》秒的十位减1，秒的个位变9
                else if(cnt == 7'b1100100 && s_1 == 4'b0 && s_10 > 4'b0 ) begin
                    s_10 <= s_10 - 4'b1;
                    s_1 <= 4'b1001;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end
                //当计满一秒，且秒为00，并且分的个位大于0时，--》分的个位减1，秒由00变为59
                else if(cnt == 7'b1100100 && s_1 == 4'b0 && s_10 == 4'b0 && m_1 > 4'b0 ) begin
                    m_1 <= m_1 - 4'b0001;	
                    s_10 <= 4'b0101;
                    s_1 <= 4'b1001;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end
                //当计满一秒，且秒为00，并且分的个位等于0，分的十位大于0时，--》分的十位减1，个位变为9，秒由00变为59
                else if(cnt == 7'b1100100 && s_1 == 4'b0 && s_10 == 4'b0 && m_1 == 4'b0 && m_10 > 4'b0) begin
                    m_10 <= m_10 - 4'b1;
                    m_1 <= 4'b1001;
                    s_10 <= 4'b0101;
                    s_1 <= 4'b1001;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end
                //当计满一秒，且秒为00，分也为00，时的个位大于0时，--》时的个位减1，分由00变为59，秒也由00变为59
                else if(cnt == 7'b1100100 && s_1 == 4'b0 && s_10 == 4'b0 && m_1 == 4'b0 && m_10 == 4'b0 && h_1 > 4'b0) begin
                    h_1 <= h_1 - 4'b1;
                    m_10 <= 4'b0101;
                    m_1 <= 4'b1001;
                    s_10 <= 4'b0101;
                    s_1 <= 4'b1001;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end                  
                //当计满一秒，且秒为00，分也为00，时的个位等于0，时的十位大于0时，--》时的十位减1，个位变9，分由00变为59，秒也由00变为59
                else if(cnt == 7'b1100100 && s_1 == 4'b0 && s_10 == 4'b0&& m_1 == 4'b0 && m_10 == 4'b0 && h_1 == 4'd0 && h_10 > 4'b0 && h_10 < 4'd2 ) begin
                    h_10 <= h_10 - 4'd1;
                    h_1 <= 4'b1001;
                    m_10 <= 4'b0101;
                    m_1 <= 4'b1001;
                    s_10 <= 4'b0101;
                    s_1 <= 4'b1001;
                    cnt <= 7'b0000001;
                    ms_10 <= 4'b1001;
                    ms_1 <= 4'b1001;
                end
                //当计满一秒，且时分秒为：00 00 00时，--》还原为预设值
                else if( s_1 == 4'd0 && s_10 == 4'd0 && m_1 == 4'd0 && m_10 == 4'd0 && h_1 == 4'd0 && h_10 == 4'd0 && if_pulse == 0)
                    if_pulse <= 1;
                else if(SUSPEND == 1'b1) cnt <= cnt + 32'b1;
                else cnt <= cnt;
            end else ;
        end
    end

    POSEDGE_TO_PULSE_LONG p(CLK, if_pulse , PULSE);

    initial begin
       mode = 0; ms_1 = 0; ms_10 = 0;
       s_1 = 0; s_10 = 0; m_1 = 0;
       m_10 = 0; h_1 = 0; h_10 = 0;
       cnt = 0; if_pulse = 0; MODE_pre = 0; MODE_now = 0;
    end
endmodule

//  小时到99的可设置计数器
module COUNTER_SET_99(
    input CLK,
    input [7:0] SET,
    input ADD,
    output  [31:0] TIME0
);
    wire [3:0] c;
    wire [3:0] carry;
    reg [7:0] set;
    always @(negedge CLK) begin
        set <= SET;
    end

    POSEDGE_TO_PULSE p23(CLK, c[0], carry[0]);
    POSEDGE_TO_PULSE p12(CLK, c[1], carry[1]);
    POSEDGE_TO_PULSE p2(CLK, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(CLK, c[3], carry[3]);

    COUNTER #(9) ms2(.CLK(1'b0),    .SET(set[0]), .ADD(ADD), .COUNT(TIME0[3:0]), .CARRY(c[0]));
    COUNTER #(9) ms1(.CLK(carry[0]),.SET(set[1]), .ADD(ADD), .COUNT(TIME0[7:4]));
    COUNTER #(9) s2 (.CLK(1'b0),    .SET(set[2]), .ADD(ADD), .COUNT(TIME0[11:8]),.CARRY(c[1]));
    COUNTER #(5) s1 (.CLK(carry[1]),.SET(set[3]), .ADD(ADD), .COUNT(TIME0[15:12]));
    COUNTER #(9) m2 (.CLK(1'b0),    .SET(set[4]), .ADD(ADD), .COUNT(TIME0[19:16]),.CARRY(c[2]));
    COUNTER #(5) m1 (.CLK(carry[2]),.SET(set[5]), .ADD(ADD), .COUNT(TIME0[23:20]));
    COUNTER #(9) h2 (.CLK(1'b0),    .SET(set[6]), .ADD(ADD), .COUNT(TIME0[27:24]),.CARRY(c[3]));
    COUNTER #(9) h1 (.CLK(carry[3]),.SET(set[7]), .ADD(ADD), .COUNT(TIME0[31:28]));
endmodule