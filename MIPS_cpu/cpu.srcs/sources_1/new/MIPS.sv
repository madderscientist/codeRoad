// ±‡–¥£∫61821326¿ÓÓ£∏’
`timescale 1ns / 1ps

module MIPS();
    reg clk = 1;
    reg PC_reset = 0;
    reg RF_reset = 0;
    wire [31:0]r[31:0];
    
    
    wire[31:0] PC;
    wire[31:0] PC_plus4;
    wire[31:0] Instr;
    wire[31:0] RD1;
    wire[31:0] RD2;
    wire[31:0] SignImm;
    wire branch, jr, jump, jal, RegWE, RegDst, ALUsrcB, MemWE, MemReg;
    wire[3:0] ALUOp;
    wire ALUzero;
    wire[31:0] ALUResult;
    wire[31:0] DMresult;
    
    Control ctrl(
        .op(Instr[31:26]),
        .func(Instr[5:0]),
        .branch(branch),
        .jr(jr),
        .jump(jump),
        .jal(jal),
        .RegWE(RegWE),
        .RegDst(RegDst),
        .ALUsrcB(ALUsrcB),
        .ALUOp(ALUOp),
        .MemWE(MemWE),
        .MemReg(MemReg)
    );
    InstrMem im(
        .clk(clk),
        .PC(PC),
        .Instruction(Instr)
    );
    SignExtend se(
        .imm(Instr[15:0]),
        .SignImm(SignImm)
    );
    PC pc(
        .clk(clk),
        .offset(SignImm),
        .absoluteAddr(Instr[25:0]),
        .registerAddr(RD1),
        .branch(ALUzero & branch),
        .jr(jr),
        .jump(jump),
        .reset(PC_reset),
        .PC(PC),
        .PC_plus4(PC_plus4)
    );
    RegisterFile rf(
        .clk(clk),
        .RegWre(RegWE),
        .A1(Instr[25:21]),
        .A2(Instr[20:16]),
        .WriteReg(jal?5'd31:(RegDst?Instr[15:11]:Instr[20:16])),
        .WriteData(jal?PC_plus4:(MemReg?DMresult:ALUResult)),
        .reset(RF_reset),
        .ReadData1(RD1),
        .ReadData2(RD2),
        .register(r)
    );
    ALU alu(
        .ALUOp(ALUOp),
        .A(RD1),
        .B(ALUsrcB?SignImm:RD2),
        .C(Instr[10:6]),
        .zero(ALUzero),
        .result(ALUResult)
    );
    DataMem dm(
        .clk(clk),
        .address(ALUResult),
        .write_data(RD2),
        .Memwrite(MemWE),
        .read_data(DMresult)
    );
    
    always #5 clk = ~clk;
    initial begin
        #2 PC_reset = 1; RF_reset = 1;
        #2 PC_reset = 0; RF_reset = 0;
    end
endmodule
