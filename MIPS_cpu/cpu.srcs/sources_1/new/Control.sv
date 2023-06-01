// 编写：61821328常兆骏
`timescale 1ns / 1ps
// 有限状态机输出控制信号
/*
instr   |branch |jr     |jump   |jal    |RegWE  |RegDst |ALUsrcB|ALUOp  |MemWE  |MemReg
beq     |1      |x      |0      |x      |0      |x      |0      |-      |0      |x
j       |x      |0      |1      |x      |0      |x      |x      |x      |0      |x
jal     |x      |0      |1      |1      |1      |x      |x      |x      |0      |x
jr      |x      |1      |1      |x      |0      |x      |x      |x      |0      |x
[R]+-*..|0      |x      |0      |0      |1      |1      |0      |?      |0      |0
[I]+ &  |0      |x      |0      |0      |1      |0      |1      |?      |0      |0
[R]<<>> |0      |x      |0      |0      |1      |1      |0      |?      |0      |0
lw      |0      |x      |0      |0      |1      |0      |1      |+      |0      |1
sw      |0      |x      |0      |0      |0      |x      |1      |+      |1      |x
*/
module Control(
    input [5:0] op,         // op操作符
    input [5:0] func,       // func操作符
    // PC控制
    output reg branch,      // 还需经过与ALU的zero再作用于PC
    output reg jr,
    output reg jump,

    output reg jal,         // 写$rs。和RegDst共同决定写的地址

    output reg RegWE,       // (RF)写使能信号
    output reg RegDst,      // 选择写的地址 1:[15:11],为R指令; 0:[20:16],为I指令。和jal共同决定写地址

    output reg ALUsrcB,     // 0:读取寄存器2; 1:符号拓展后的立即数
    output reg [3:0] ALUOp, // ALU操作控制

    output reg MemWE,       // 写数据存储器
    output reg MemReg       // 从内存到寄存器 0:计算结果, 1:读取结果
    );
    initial begin
        branch = 0;
        jr = 0;
        jump = 0;
        jal = 0;
        RegWE = 0;
        RegDst = 0;
        ALUsrcB = 0;
        ALUOp = 0;
        MemWE = 0;
        MemReg = 0;
    end
    always@(op or func)
    begin
        case (op)
            6'b000000: 
            begin
                case (func[5:3])
                    3'b100: 
                    begin
                        branch = 0;
                        jr = 0;
                        jump = 0;
                        jal = 0;
                        RegWE = 1;
                        RegDst = 1;
                        ALUsrcB = 0;
                        MemWE = 0;
                        MemReg = 0;
                        case (func[2:0])
                            3'b000: ALUOp = 4'b0101;//add                                
                            3'b010: ALUOp = 4'b0110;//sub
                            3'b100: ALUOp = 4'b0111;//and
                            3'b101: ALUOp = 4'b1000;//or
                            3'b111: ALUOp = 4'b1010;//nor
                            3'b110: ALUOp = 4'b1001;//xor
                            default:ALUOp =0;
                        endcase
                    end
                    3'b000:
                    begin
                        branch = 0;
                        jr = 0;
                        jump = 0;
                        jal = 0;
                        RegWE = 1;
                        RegDst = 1;
                        ALUsrcB = 0;
                        MemWE = 0;
                        MemReg = 0;
                        case (func[2:0])
                            3'b000: ALUOp = 4'b0000;//逻辑左移                               
                            3'b010: ALUOp = 4'b0010;//逻辑右移
                            3'b011: ALUOp = 4'b0001;//算数右移
                            default:ALUOp =0;
                        endcase
                    end
                    //jr
                    3'b001:
                    begin
                        branch = 0;
                        jr = 1;
                        jump = 1;
                        jal = 0;
                        RegWE = 0;
                        RegDst = 0;
                        ALUsrcB = 0;
                        ALUOp = 0;
                        MemWE = 0;
                        MemReg = 0;
                    end
                endcase
            end
            default: 
            begin
                case (op[5:3])
                    //i
                    3'b001:
                    begin
                        branch = 0;
                        jr = 0;
                        jump = 0;
                        jal = 0;
                        RegWE = 1;
                        RegDst = 0;
                        ALUsrcB = 1;
                        MemWE = 0;
                        MemReg = 0;
                        case (op[2:0])
                            3'b000: ALUOp = 4'b0101;//addi                                
                            3'b100: ALUOp = 4'b0111;//andi
                            3'b101: ALUOp = 4'b1000;//ori
                            3'b110: ALUOp = 4'b1001;//xori
                            default:ALUOp =0;
                        endcase
                    end 
                    //lw
                    3'b100: 
                    begin
                        branch = 0;
                        jr = 0;
                        jump = 0;
                        jal = 0;
                        RegWE = 1;
                        RegDst = 0;
                        ALUsrcB = 1;
                        ALUOp = 4'b0101;
                        MemWE = 0;
                        MemReg = 1;
                    end
                    //sw
                    3'b101: 
                    begin
                        branch = 0;
                        jr = 0;
                        jump = 0;
                        jal = 0;
                        RegWE = 0;
                        RegDst = 0;
                        ALUsrcB = 1;
                        ALUOp = 4'b0101;
                        MemWE = 1;
                        MemReg = 0;
                    end
                    //跳转
                    3'b000: 
                    begin
                        //branch = 0;
                        jr = 0;
                        //jump = 0;
                        //jal = 0;
                        //RegWE = 0;
                        RegDst = 0;
                        ALUsrcB = 0;
                        ALUOp = 0;
                        MemWE = 0;
                        MemReg = 0;
                        case (op[2:0])
                        //beq
                            3'b100:
                            begin
                                branch = 1;
                                jump = 0;
                                jal = 0;
                                RegWE = 0;
                                ALUsrcB = 0;
                                ALUOp = 4'b0110;
                            end
                        //j
                            3'b010:  
                            begin
                                branch = 0;
                                jump = 1;
                                jal = 0;
                                RegWE = 0; 
                            end  
                        //jal
                            3'b011: 
                            begin
                                branch = 0;
                                jump = 1;
                                jal = 1;
                                RegWE = 1; 
                            end  
                        endcase
                    end 
                endcase
            end
        endcase
    end
endmodule