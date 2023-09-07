`timescale 1ns/1ps
// 计数器
// SET = 0时，为自动模式，CLK上升沿驱动，会进位
// 进位信号CARRY输出一个长高电平，为了防止竞争冒险，需要在外部对此信号脉冲化
// SET = 1时，为设置模式，ADD上升沿驱动
// 注意，此模块内ADD和SET是异步信号
// 要保证时序必须在模块外，将SET ADD CLK变成同步信号
module COUNTER #(parameter MAX = 9) (
    input CLK,
    input SET,
    input ADD,
    output reg CARRY,
    output reg [3:0] COUNT
);
    wire clk = SET ? ADD : CLK;     // 信号切换的时候可能造成上升沿。但只要SET在CLK上升沿更新就行
    always @(posedge clk) begin
        if(COUNT == MAX) begin
            COUNT <= 0;
            CARRY <= 1;
        end else begin
            COUNT <= COUNT + 1;
            CARRY <= 0;
        end
    end
    initial begin
        CARRY = 0;
        COUNT = 0;
    end
endmodule

// 自适应最大值的计数器
// IF_REFRESH: [要刷新] min (REFRESH触发)
// !IF_REFRESH:
//      SET: [在设置自己] +1 (ADD触发)
//      !SET:[不在设置自己] +1 (CLK触发)
// ADD SET CLK REFRESH需要在外部同步
module COUNTER_ADJUST (
    input REFRESH,
    input CLK,
    input SET,      // 是否在设置这一个
    input ADD,
    input IF_REFRESH,
    input [3:0] MAX,
    output reg CARRY,
    output reg [3:0] COUNT
);
    wire clk_1 = SET ? ADD : CLK;
    wire clk = IF_REFRESH ? REFRESH : clk_1;
    always @(posedge clk) begin
        if (IF_REFRESH)
            COUNT <= COUNT > MAX ? MAX : COUNT;
        else begin
            if (COUNT == MAX) begin
                COUNT <= 0;
                CARRY <= 1;
            end else begin
                COUNT <= COUNT + 1;
                CARRY <= 0;
            end
        end
    end
    initial begin
        CARRY = 0;
        COUNT = 0;
    end
endmodule