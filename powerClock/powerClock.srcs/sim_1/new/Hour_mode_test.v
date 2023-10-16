`timescale 1ns / 1ps

// hour_mode 小时的12/24进制切换测试
module hourmode_test();
    reg [3:0] h1, h2;
    wire [3:0] ho1, ho2;
    reg mode;
    HOURMODE HM(mode, h1, h2, ho1, ho2);
    initial begin
        mode = 1;
        h1 = 0;
        h2 = 0;
        #10 h2 = 1;
        #10 h2 = 2;
        #10 h2 = 3;
        #10 h2 = 4;
        #10 h1 = 1;
        #10 h2 = 5;
    end
endmodule