





module oled_main(
	
	input			sys_clk,
	input			rst_n,
	

	input [2:0]         STATE,
    input [31:0]       DATA,
	output		 OLED_SCL,
	inout			OLED_SDA
    
);

wire dht11_done;
reg  dht11_req;
reg[7:0]  tempH;
reg[7:0]  tempL;	
reg[7:0]  humidityH;
reg[7:0]  humidityL; 


localparam S_DELAY	=	'd55_000_000;
reg[35:0]	delay;

always@(posedge sys_clk) begin
     humidityL <= DATA[3:0] + DATA[7:4] * 'd10;
     humidityH <= DATA[11:8] + DATA[15:12] * 'd10;
     tempL <= DATA[19:16] + DATA[23:20] * 'd10;
     tempH <= DATA[27:24] + DATA[31:28] * 'd10;
end 
 
 
 
 
/*assign tempH = 8'b1;
assign tempL = 8'b1;
assign humidityH = 8'b1;

always@(posedge sys_clk) begin
     if(x == 1'b0)  humidityL <= 8'b1;
     else humidityL <= 8'b00000010;
end */

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		delay <= 'd0;
	else if(dht11_done == 1'b1)
		delay <= 'd0;
	else if(delay == S_DELAY)
		delay <= delay;
	else 
		delay <= delay + 1'b1;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)	
		dht11_req <= 1'b0;
	else if(delay == S_DELAY)
		dht11_req <= 1'b1;
	else
		dht11_req <= 1'b0;


end





OLED_Top OLED_TopHP(

	.sys_clk		(sys_clk),
	.rst_n		(rst_n),
	
	.dht11_done		(1'b1),
	.tempH			(tempH),			//温度数据整数
	.tempL			(tempL),			//温度数据小数
	.humidityH		(humidityH),		//温度数据整数
	.humidityL		(humidityL),		//温度数据小数
	.State          (STATE),
	//OLED IIC
	.OLED_SCL	(OLED_SCL),
	.OLED_SDA	(OLED_SDA)
	
);


endmodule 