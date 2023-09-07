`timescale 1ns/1ps
// 译码器 输出从高到低为GFEDCBA
// 由于数码管共阳，所以亮的是置零位
module DECODER (
    input [3:0] DATA,
    output reg [6:0] SEG
);
    always @(*) begin
        case (DATA)
            4'b0000: SEG = 7'b1000000;
            4'b0001: SEG = 7'b1111001;
            4'b0010: SEG = 7'b0100100;
            4'b0011: SEG = 7'b0110000;
            4'b0100: SEG = 7'b0011001;
            4'b0101: SEG = 7'b0010010;
            4'b0110: SEG = 7'b0000010;
            4'b0111: SEG = 7'b1111000;
            4'b1000: SEG = 7'b0000000;
            4'b1001: SEG = 7'b0010000;
            4'b1010: SEG = 7'b0001000;
            4'b1011: SEG = 7'b0000011;
            4'b1100: SEG = 7'b1000110;
            4'b1101: SEG = 7'b0100001;
            4'b1110: SEG = 7'b0000110;
            4'b1111: SEG = 7'b0001110;
            default: ;
        endcase
    end
endmodule

module LED_DECODER(
    input [2:0] states,
    output reg [5:0] LEDS
);
    always @(*) begin
        case (states)
            3'b001: LEDS = 6'b010000;
            3'b010: LEDS = 6'b001000;
            3'b011: LEDS = 6'b000100;
            3'b100: LEDS = 6'b000010;
            3'b101: LEDS = 6'b000001;
            default: LEDS = 6'b100000;
        endcase
    end
endmodule