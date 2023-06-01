// 编写：61821326李睿刚
`timescale 1ns / 1ps
// 读写32个寄存器
module RegisterFile(
    input           clk,        // 时钟
    input           RegWre,     // 写使能信号，为1时，在时钟上升沿写入
    input [4:0]     A1,         // rs寄存器地址输入端口
    input [4:0]     A2,         // rt寄存器地址输入端口
    input [4:0]     WriteReg,   // 将数据写入的寄存器端口，其地址来源rt或rd字段
    input [31:0]    WriteData,  // 写入寄存器的数据输入端口
    input           reset,      // 1则归零
    output[31:0]    ReadData1,  // rs寄存器数据输出端口
    output[31:0]    ReadData2,  // rt寄存器数据输出端口
    output reg [31:0] register[31:0]    // 可以输出
    );
    // 清零
    integer i;

    // 读寄存器 组合逻辑
    assign ReadData1 = register[A1];
    assign ReadData2 = register[A2];

    // 下降沿写寄存器
    always@(negedge clk or posedge reset) begin
        if (reset) begin
            for (i = 0; i < 32; i = i + 1)
                register[i] <= 0;
        end
        else begin
            // 如果寄存器不为0，并且RegWre为真，写入数据
            if (RegWre && WriteReg != 5'b0) 
                register[WriteReg] <= WriteData;
        end
    end 

endmodule