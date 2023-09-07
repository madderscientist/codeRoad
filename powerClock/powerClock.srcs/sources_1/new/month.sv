`timescale 1ns/1ps
// 不需要刷新，写死
module MONTH(
    input CLK,
    input [1:0] SET,
    input ADD,
    output CARRY,
    output reg [7:0] DATE
);
    parameter MAX = 4'b1100;
    parameter MIN = 4'b0001;
    reg [3:0] count = MIN;

    wire [4:0] add10 = 4'b0010>count ? count + 4'b1010 : MAX;
    wire [4:0] add10_final = count==MAX ? 4'b0010 : add10;
    wire [4:0] add1 = count + 4'b1;
    wire [4:0] add1_final = add1>MAX ? MIN : add1;

    wire [4:0] next = SET==2'b10 ? add10_final : add1_final;
    wire clk = SET==2'b00 ? CLK : ADD;

    always @(posedge clk) begin
        count <= next;
    end
    // 5位2进制转2个4位BCD码
    always @(*) begin
        case (count)
            4'b0001: DATE = 8'b0000_0001;
            4'b0010: DATE = 8'b0000_0010;
            4'b0011: DATE = 8'b0000_0011;
            4'b0100: DATE = 8'b0000_0100;
            4'b0101: DATE = 8'b0000_0101;
            4'b0110: DATE = 8'b0000_0110;
            4'b0111: DATE = 8'b0000_0111;
            4'b1000: DATE = 8'b0000_1000;
            4'b1001: DATE = 8'b0000_1001;
            4'b1010: DATE = 8'b0001_0000;
            4'b1011: DATE = 8'b0001_0001;
            4'b1100: DATE = 8'b0001_0010;
            default: DATE = 8'b0000_0000;
        endcase
    end
    assign CARRY = count==MAX;
endmodule