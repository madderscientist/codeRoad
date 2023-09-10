// 日历，8个BCD计数器，前四个记录年，后四个记录月日
// 每个计数器对应输入SET[7:0]的一位，当对应位为1时，计数器值在输入ADD的上升沿+1
// 输入SET全为0时（自动模式），输入CLK的上升沿日+1，并在溢出时进位
// 输入SET不全为0是（设置模式），通过ADD改变计数器的时候，需要判断闰年和大小月。如果日比最大值大，则设置为最大值。
// 上面是之前的想法。实现时过于复杂，直接换成写死的case了。但TIMER模块中的24小时还是这个思路
module CALENDAR(
    input CLK,      // 来自TIMER的进位
    input REFRESH,  // 100Hz
    input [7:0] SET,
    input ADD,
    output [31:0] DATE
);
    // 一同步就会有奇怪的bug
    reg [7:0] set = 8'b0;
    reg ifcarry;
    // 同步SET信号
    always @(negedge REFRESH) begin // 不同步或者上升沿同步都会出问题，具体是SET改变时DATE的值也在变。
        set <= SET;
        ifcarry <= SET==8'b00000000;
    end

    // 进位信号处理
    wire [6:0] c;       // 从计数器取到的宽电平
    wire [6:0] carry;   // 脉冲化后的进位
    POSEDGE_TO_PULSE p01(REFRESH, c[0], carry[0]);
    POSEDGE_TO_PULSE p12(REFRESH, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(REFRESH, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(REFRESH, c[3], carry[3]);
    POSEDGE_TO_PULSE p45(REFRESH, c[4], carry[4]);

    /*-- 日期、月份的限制 组合逻辑--*/
    wire leapyear;      // 判断是否为闰年(1-是，0-否)
    LEAPYEAR _leapyear(DATE[31:28], DATE[27:24], DATE[23:20], DATE[19:16], leapyear);
    reg day31;
    // 判断是否为大月
    always @(*) begin
        case (DATE[15:8])
            8'b0000_0001, 8'b0000_0011, 8'b0000_0101, 8'b0000_0111,
            8'b0000_1000, 8'b0001_0000, 8'b0001_0010: day31 = 1;
            default: day31 = 0;
        endcase
    end
    // 日最大值
    reg [4:0] d_max;
    always @(*) begin
        if (DATE[15:8]==8'b0000_0010) begin // 是2月
            d_max = leapyear ? 5'd29 : 5'd28;
        end else begin
            d_max = day31 ? 5'd31 : 5'd30;
        end
    end
    // 刷新时机 日期和前面所有位都有关系，而月的个位之和十位有关系
    wire d_refresh = set[2] | set[3] | set[4] | set[5] | set[6] | set[7];
    /*-- 计数器 --*/
    DAY day(REFRESH, ifcarry ? CLK : 1'b0, d_refresh, set[1:0], ADD, d_max, c[0], DATE[7:0]);
    MONTH month(ifcarry ? carry[0] : 1'b0, set[3:2], ADD, c[1], DATE[15:8]);
    COUNTER #(9) y4(ifcarry ? carry[1] : 1'b0, set[4], ADD, c[2], DATE[19:16]);
    COUNTER #(9) y3(          carry[2],     set[5], ADD, c[3], DATE[23:20]);
    COUNTER #(9) y2(          carry[3],     set[6], ADD, c[4], DATE[27:24]);
    COUNTER #(9) y1(    // 没有进位输出，所以不传
        .CLK(carry[4]),
        .SET(set[7]),
        .ADD(ADD),
        .COUNT(DATE[31:28])
    );
endmodule