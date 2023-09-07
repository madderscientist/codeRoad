// 结构上分为4组，为时、分、秒、10毫秒，每组有2个4位BCD寄存器，分别表示个位和十位，每位对应一个8段数码管。
// 比如12:03:28:98表示12点3分28秒980毫秒
// 每个BCD计数器对应一个SET信号。当有SET信号为高时，计时器暂停自动增加。某计数器对应的SET为高时，在ADD信号的上升沿该计数器加一，不产生进位信号。[在COUNTER中实现]
// 当SET全为低时，计时器由10ms的CLK信号驱动，每10ms最低位计数器加一，当计数器溢出时，下一个计数器会加一。
// 为了防止竞争冒险，SET信号一直由CLK信号上升沿更新，实现同步。【已经不需要同步了，在FSM中实现了同步】
// 当最高位计数器溢出，会产生一个表示进位的脉冲c。
// ADD应是短脉冲，在模块外应与CLK同步【按钮脉冲消抖后已同步】
// 难点在于小时是24进制的。写了一个自适应最大值的COUNTER解决。【CLAENDAR中此法不方便，用它法】
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
    always @(negedge CLK) begin // 下降沿锁状态避免了一切竞争冒险！
        set <= SET;
        ifcarry <= SET==8'b00000000;
    end

    // 进位信号处理
    wire [7:0] c;       // 从计数器取到的宽电平
    wire [6:0] carry;   // 脉冲化后的进位
    // 由于这个模块要求信号长度必须大于clk，因此c[0]不能传入
    // 修正：模块已更新，但是c[0]无需脉冲化
    POSEDGE_TO_PULSE p12(CLK, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(CLK, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(CLK, c[3], carry[3]);
    POSEDGE_TO_PULSE p45(CLK, c[4], carry[4]);
    POSEDGE_TO_PULSE p56(CLK, c[5], carry[5]);
    POSEDGE_TO_PULSE p67(CLK, c[6], carry[6]);
    POSEDGE_TO_PULSE pout(CLK, c[7], CARRY);

    // 当设置模式下，单位间不需要进位，单位内照常
    COUNTER #(9) ms2(ifcarry ? CLK      : 0, set[0], ADD, c[0], TIME[3:0]);
    COUNTER #(9) ms1(          c[0],         set[1], ADD, c[1], TIME[7:4]);
    COUNTER #(9) s2 (ifcarry ? carry[1] : 0, set[2], ADD, c[2], TIME[11:8]);
    COUNTER #(5) s1 (          carry[2],     set[3], ADD, c[3], TIME[15:12]);
    COUNTER #(9) m2 (ifcarry ? carry[3] : 0, set[4], ADD, c[4], TIME[19:16]);
    COUNTER #(5) m1 (          carry[4],     set[5], ADD, c[5], TIME[23:20]);
    // 小时是满24清空，实现上和日期类似，只有小时的个位需要不断刷新
    wire h1_fresh = set[7];
    wire [3:0] h1_max = TIME[31:28]==4'b0010 ? 4'b0011 : 4'b1001;
    COUNTER_ADJUST h2 (CLK, ifcarry ? carry[5] : 0, set[6], ADD, h1_fresh, h1_max, c[6], TIME[27:24]);
    COUNTER #(2) h1 (          carry[6],     set[7], ADD, c[7], TIME[31:28]);
endmodule