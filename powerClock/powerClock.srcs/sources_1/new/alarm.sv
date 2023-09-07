`timescale 1ns/1ps
module ALARM(
    input CLK,
    input [7:0] SET,
    input ADD,
    input ENABLE,   // 当时钟设置、此闹钟设置时ENABLE应为0
    input [31:0] NOW,
    output [31:0] TIME,
    output ALARM    // 脉冲
);
    wire [3:0] c;
    wire [3:0] carry;
    reg [7:0] set;
    always @(negedge CLK) begin
        set <= SET;
    end

    POSEDGE_TO_PULSE p01(CLK, c[0], carry[0]);
    POSEDGE_TO_PULSE p12(CLK, c[1], carry[1]);
    POSEDGE_TO_PULSE p23(CLK, c[2], carry[2]);
    POSEDGE_TO_PULSE p34(CLK, c[3], carry[3]);

    COUNTER #(9) ms2(.CLK(0),       .SET(set[0]), .ADD(ADD), .COUNT(TIME[3:0]), .CARRY(c[0]));
    COUNTER #(9) ms1(.CLK(carry[0]),.SET(set[1]), .ADD(ADD), .COUNT(TIME[7:4]));
    COUNTER #(9) s2 (.CLK(0),       .SET(set[2]), .ADD(ADD), .COUNT(TIME[11:8]),.CARRY(c[1]));
    COUNTER #(5) s1 (.CLK(carry[1]),.SET(set[3]), .ADD(ADD), .COUNT(TIME[15:12]));
    COUNTER #(9) m2 (.CLK(0),       .SET(set[4]), .ADD(ADD), .COUNT(TIME[19:16]),.CARRY(c[2]));
    COUNTER #(5) m1 (.CLK(carry[2]),.SET(set[5]), .ADD(ADD), .COUNT(TIME[23:20]));
    wire h1_fresh = set[7];
    wire [3:0] h1_max = TIME[31:28]==4'b0010 ? 4'b0011 : 4'b1001;
    COUNTER_ADJUST h2 (CLK, 0, set[6], ADD, h1_fresh, h1_max, c[3], TIME[27:24]);
    COUNTER #(2) h1 (.CLK(carry[3]),.SET(set[7]), .ADD(ADD), .COUNT(TIME[31:28]));

    assign ALARM = NOW == TIME ? ENABLE : 0;    // 用时钟的10ms保证了这是个脉冲（可能需要改
endmodule