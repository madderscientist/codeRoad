`timescale 1ns/1ps
module UART_TX #(
	parameter Burd_rate = 9600 ,         // 波特率
	parameter Clk_freq  = 100_000_000    // 时钟频率
)(
	input clk_in,
	input rst_n,

	input  		 tx_flag,   // 脉冲，长度大于100M周期，小于发送完一位数据的时间
	input  [7:0] tx_data,
	
	output reg tx = 1'b1,
    output SENDING  // 发送时高电平，发完了低电平
);
    localparam CNT_MAX = Clk_freq/Burd_rate;
    
    reg [7:0]   tx_data_reg;
    reg         trans_en;       // 传数据时是高电平
    reg [13:0]  burd_cnt;
    reg  		bit_flag;
    reg  [3:0]  bit_cnt;

    // tx_flag为高时寄存tx_data
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            tx_data_reg <= 'h0;
        else if(tx_flag)
            tx_data_reg <= tx_data;
        else
            tx_data_reg <= tx_data_reg;
            
    // 生成发送使能信号
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            trans_en <= 'h0;
        else if(tx_flag)
            trans_en <= 1'b1;		
        else if(bit_cnt == 4'd10 && bit_flag == 1'b1)
            // 发送完了且到第十轮的1时才清零 所以tx_flag要在发送完前归零，不然开启同样数据的下一轮发送
            trans_en <= 1'b0;
        else ;

    // 累加波特计数器 计满了清零 时长为一位数据宽度
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            burd_cnt <= 'h0;
        else if((trans_en == 0) || (burd_cnt == CNT_MAX -1))
            burd_cnt <= 'h0;		
        else if(trans_en)
            burd_cnt <= burd_cnt + 1'b1;
        else ;

    // 生成比特发送使能信号
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            bit_flag <= 'b0;
        else if(burd_cnt ==  1)
            bit_flag <= 1'b1;		
        else 
            bit_flag <= 'b0;
            
    // 累加比特计数器 在bit_flag高电平时满九清零
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            bit_cnt <= 'b0;
        else if(bit_cnt ==  4'd10 && bit_flag == 1'b1)
            bit_cnt <= 'b0;		
        else if(bit_flag ==  1'b1)
            bit_cnt <= bit_cnt + 1'b1;
        else ;
            
    // 发送比特数据
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            tx <= 1'b1;
        else if(bit_flag == 1'b1) begin
            case(bit_cnt)
                0: tx <= 1'b0;
                1: tx <= tx_data_reg[0];
                2: tx <= tx_data_reg[1];
                3: tx <= tx_data_reg[2];
                4: tx <= tx_data_reg[3];
                5: tx <= tx_data_reg[4];
                6: tx <= tx_data_reg[5];
                7: tx <= tx_data_reg[6];
                8: tx <= tx_data_reg[7];
                default:tx <= 1'b1;
            endcase
        end

    assign SENDING = trans_en;
    initial begin
        tx_data_reg = 0;
        trans_en = 0;
        burd_cnt = 0;
        bit_flag = 0;
        bit_cnt = 0;
    end
endmodule

module UART_RX #(
	parameter Burd_rate    = 9600 ,         // 波特率
	parameter Clk_freq     = 100_000_000    // 时钟频率
)(
	input clk_in,
	input rst_n,
	input rx,
	
	output reg rx_flag,     // 是发完之后的一周期的脉冲
	output reg [7:0] rx_data
);
    localparam CNT_MAX =  Clk_freq/Burd_rate;
    
    reg [2:0] rx_reg;
    reg 	  start_flag;
    reg 	  receive_en;
    reg [13:0] buad_cnt;
    reg [3:0] bit_cnt;
    reg 	  bit_flag;
    reg [7:0] rx_data_reg;

    wire fall_edge;
    // 提取 rx打拍信号的下降沿
    assign fall_edge = (~rx_reg[1]) && rx_reg[2] ;
    assign receive_end = bit_cnt == 4'd9 && buad_cnt == CNT_MAX-1;
    // 将rx打三拍
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            rx_reg <= 3'b000;
        else 
            rx_reg <= {rx_reg[1],rx_reg[0],rx};
            
    // 产生起始信号 收到信号后的脉冲
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            start_flag <= 1'b0;
        else if(fall_edge && (receive_en == 1'b0))
            start_flag <= 1'b1; // start_flag导致receive_en, 后者又让前者重新置零
        else
            start_flag <= 1'b0;

    // 产生计数使能信号 接收时一直保持高电平
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            receive_en <= 1'b0;
        else if(start_flag == 1'b1 && bit_cnt == 4'b0)
            receive_en <= 1'b1;
        else if(receive_end)   // 满9次或满1次
            receive_en <= 1'b0;
        else
            receive_en <= receive_en;
            
    // 波特计数器累加	
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            buad_cnt <= 'h0;
        else if(buad_cnt == CNT_MAX - 1 || receive_en == 1'b0)
            buad_cnt <= 'h0;
        else if(receive_en == 1'b1)
            buad_cnt <= buad_cnt + 1'b1;

    // 比特采样标志信号 在每位时间中间采样 采样标志是big_flag=1
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            bit_flag <= 1'b0;
        else if(buad_cnt == CNT_MAX/2 - 1)
            bit_flag <= 1'b1;
        else 
            bit_flag <= 1'b0;

    // 接收比特计数器累加
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            bit_cnt <= 4'b0;
        else if(receive_end)
            bit_cnt <= 4'b0;
        else if(bit_flag)
            bit_cnt <= bit_cnt + 1'b1;	
            
    // 接收数据移位，先接收的是低位，最后接收的是高位
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n)
            rx_data_reg <= 8'b0;
        else if(bit_flag && bit_cnt > 4'd0)
            rx_data_reg <= {rx_reg[2],rx_data_reg[7:1]};
        else if(bit_cnt == 4'b0)
            rx_data_reg <= 8'b0;
        else
            rx_data_reg <= rx_data_reg;

    // 输出数据有效信号和八位数据
    always@(posedge clk_in or negedge rst_n)
        if(!rst_n) begin
            rx_data <= 8'b0;
            rx_flag <= 1'b0;
        end
        else if(receive_end) begin
            rx_data <= rx_data_reg;
            rx_flag <= 1'b1;
        end
        else begin
            rx_data <= 8'b0;
            rx_flag <= 1'b0;
        end
endmodule