`timescale 1ns/1ps
// 数据选择器，选择通入数码管的数据
module SELECTOR(
    input [2:0] SELECT,
    input [31:0] CHANNEL1,
    input [31:0] CHANNEL2,
    input [31:0] CHANNEL3,
    input [31:0] CHANNEL4,
    input [31:0] CHANNEL5,
    input [31:0] CHANNEL6,
    output reg [31:0] CHANNEL_OUT
);
    always @(*) begin
        case (SELECT)
            3'b001: CHANNEL_OUT = CHANNEL2;
            3'b010: CHANNEL_OUT = CHANNEL3;
            3'b011: CHANNEL_OUT = CHANNEL4;
            3'b100: CHANNEL_OUT = CHANNEL5;
            3'b101: CHANNEL_OUT = CHANNEL6;
            default: CHANNEL_OUT = CHANNEL1;
        endcase
    end
endmodule