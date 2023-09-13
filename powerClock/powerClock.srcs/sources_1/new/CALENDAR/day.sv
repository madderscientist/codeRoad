`timescale 1ns/1ps
// 最小值为1，最大值要设置
module DAY(
    input REFRESH,
    input CLK,
    input IF_REFRESH,
    input [1:0] SET,
    input ADD,
    input [4:0] MAX,    // 2进制 大于等于10
    output CARRY,
    output [7:0] DATE
);
    parameter MIN = 5'b1;
    reg [4:0] count = 5'b00001;
    // +10的逻辑：如果十位相同则改十位，如果不同需要更新
    wire [4:0] minus10 = MAX - 5'b01010;
    wire [7:0] max_bcd;
    B_DECODER bd10(MAX, max_bcd);
    wire [4:0] add10 = minus10>count ? count + 5'b01010 : MAX;
    wire [4:0] add10_final = max_bcd[7:4]==DATE[7:4] ? DATE[3:0] : add10;
    // +1的逻辑：如果加1后为0或者大于最大值则为1，否则加1
    wire [4:0] add1 = count + 5'b00001;
    wire [4:0] add1_final = add1==5'b0||add1>MAX ? 5'b00001 : add1;
    wire [4:0] upper = count>MAX ? MAX : count;
    wire [4:0] inrange = upper<MIN ? MIN : upper;

    reg [4:0] next;
    always @(*) begin
        case ({IF_REFRESH,SET})
            3'b000, 3'b001: next = add1_final;
            3'b010: next = add10_final; 
            default: next = inrange;
        endcase
    end

    reg clk;
    always @(*) begin
        if(IF_REFRESH) clk = REFRESH;
        else begin
            if (SET==2'b00) clk = CLK;
            else clk = ADD;
        end
    end

    always @(posedge clk) begin
        count <= next;
    end
    B_DECODER bd(count, DATE);
    assign CARRY = count!=MAX;
endmodule

// 5位2进制转2个4位BCD码
module B_DECODER(
    input [4:0] BINARY,
    output reg [7:0] BCD
);
    always @(*) begin
        case (BINARY)
            5'b00001: BCD = 8'b0000_0001;
            5'b00010: BCD = 8'b0000_0010;
            5'b00011: BCD = 8'b0000_0011;
            5'b00100: BCD = 8'b0000_0100;
            5'b00101: BCD = 8'b0000_0101;
            5'b00110: BCD = 8'b0000_0110;
            5'b00111: BCD = 8'b0000_0111;
            5'b01000: BCD = 8'b0000_1000;
            5'b01001: BCD = 8'b0000_1001;
            5'b01010: BCD = 8'b0001_0000;
            5'b01011: BCD = 8'b0001_0001;
            5'b01100: BCD = 8'b0001_0010;
            5'b01101: BCD = 8'b0001_0011;
            5'b01110: BCD = 8'b0001_0100;
            5'b01111: BCD = 8'b0001_0101;
            5'b10000: BCD = 8'b0001_0110;
            5'b10001: BCD = 8'b0001_0111;
            5'b10010: BCD = 8'b0001_1000;
            5'b10011: BCD = 8'b0001_1001;
            5'b10100: BCD = 8'b0010_0000;
            5'b10101: BCD = 8'b0010_0001;
            5'b10110: BCD = 8'b0010_0010;
            5'b10111: BCD = 8'b0010_0011;
            5'b11000: BCD = 8'b0010_0100;
            5'b11001: BCD = 8'b0010_0101;
            5'b11010: BCD = 8'b0010_0110;
            5'b11011: BCD = 8'b0010_0111;
            5'b11100: BCD = 8'b0010_1000;
            5'b11101: BCD = 8'b0010_1001;
            5'b11110: BCD = 8'b0011_0000;
            5'b11111: BCD = 8'b0011_0001;
            default: BCD = 8'b0000_0000;
        endcase
    end
endmodule