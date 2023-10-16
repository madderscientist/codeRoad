`timescale 1ns / 1ps

// FSM 闹钟并发测试
module fsm_alarm_test();
    reg clk;
    reg [3:0]alarm;
    reg [3:0]stop;
    wire [3:0] alarming;
    FSM fsm(
        .CLK(clk),
        .ALARM(alarm),
        .STOP(stop),
        .ALARMING(alarming)
    );
    always #5 clk = ~clk;
    initial begin
        clk = 0;
        alarm = 0;
        stop = 0;
        #15 alarm[0] = 1;
        #10 alarm[0] = 0;
        #10 alarm[1] = 1;
        #10 alarm[1] = 0;
        #100 stop[0] = 1;
        #10 stop[0] = 0; 
        #30 stop[1] = 1;
        #10 stop[1] = 0;
    end
endmodule
