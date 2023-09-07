`timescale 1ns/1ps
// 数码管驱动 一共8个数码管，每个数码管有8位，共阳
// 输入BCD数据和小数点数据，输出8段数码管数据和8个数码管的使能信号
// 应设置DATA0为最右侧数码管对应数据，DOT[0]为最右侧数码管对应小数点
// EN为时序逻辑，其余为组合逻辑
module TUBES (
    input CLK,  // 500 ~ 1kHz
    input [7:0] EN_CTRL,    // 外部控制使能，控制亮灭，为0的位对应的数码管不亮   
    input [3:0] DATA0,      // verilog不支持二维数组，所以只能用8个一维数组来表示8个数码管(虽然systemVerilog支持但还是改了)
    input [3:0] DATA1,
    input [3:0] DATA2,
    input [3:0] DATA3,
    input [3:0] DATA4,
    input [3:0] DATA5,
    input [3:0] DATA6,
    input [3:0] DATA7,
    input [7:0] DOT,        // 8个小数点, DOT[0]对应TUBE[0]的小数点，为0的对应不亮
    output [7:0] EN,        // 8个使能
    output [7:0] TUBE       // 共用的8段
);
    reg [7:0] en;
    // en是一个循环移位器，负责扫描
    always @(posedge CLK) begin
        if (en == 0)
            en <= 8'b00000001;
        else begin
            if (en == 8'b10000000)
                en <= 8'b00000001;
            else
                en <= en << 1;
        end
    end
    assign EN = ~(en & EN_CTRL);

    wire [6:0] decoded;
    reg [3:0] todecode;
    DECODER d(todecode, decoded);
    assign TUBE = {(DOT&en)==0, decoded};

    always @(*) begin
        case (en)
            8'b00000001: todecode = DATA0;
            8'b00000010: todecode = DATA1;
            8'b00000100: todecode = DATA2;
            8'b00001000: todecode = DATA3;
            8'b00010000: todecode = DATA4;
            8'b00100000: todecode = DATA5;
            8'b01000000: todecode = DATA6;
            8'b10000000: todecode = DATA7;
            default: todecode = 4'b0000;
        endcase
    end

    initial begin
        en = 8'b00000001;
    end
endmodule