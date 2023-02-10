`timescale 1ns/ 1ps
module hourShow(
        input mode,         //1为12输出，0为24输出
        input [7:0] bcd_in,
        output reg [7:0] bcd_out
    );
    always @(*) begin
        if(mode && bcd_in > 8'b00010010) begin
            if(bcd_in[3:0]>=4'b0010) bcd_out<=bcd_in-8'b00010010;
            else bcd_out=bcd_in-8'b00011000;
            //+8'b00001010-8'b00100010;
        end
        else bcd_out <= bcd_in;
    end
endmodule