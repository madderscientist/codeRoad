// ±‡–¥£∫61821326¿ÓÓ£∏’
`timescale 1ns / 1ps

module SignExtend(
    input[15:0] imm,
    output [31:0] SignImm
);
    assign SignImm[15:0] = imm[15:0];
    assign SignImm[31:16] = {16{imm[15]}};
endmodule