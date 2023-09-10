




module OLED_ShowData(
	
	input				sys_clk,
	input				rst_n,
	
	
	input				dht11_done,
	input[7:0]	   tempH,	
	input[7:0]	   tempL,
	input[7:0]	   humidityH,
	input[7:0]	   humidityL,
	
	
	input				ShowData_req,		 //字符显示请求
	input				write_done,			 //iic一组数据写完成
	
	output[23:0]	ShowData_Data,		 //字符显示数据
	
	output			ShowData_finish   //字符显示完成

);


//
//reg[3:0]		tempHH; //tempH的高位
//reg[3:0]		tempHL; //tempH的低位
//reg[3:0]		tempLH;
//reg[3:0]		tempLL;
//
//reg[3:0]		humidityHH;
//reg[3:0]		humidityHL;
//reg[3:0]		humidityLH;
//reg[3:0]		humidityLL;


reg[7:0]		tempHREG;
reg[7:0]		tempLREG;
reg[7:0]		humidityHREG;
reg[7:0]		humidityLREG;


reg[4:0]	font;

reg[4:0]			font_sel;
reg[4:0] 		font_index;
reg				font_row;

reg[7:0]			show_x;
reg[3:0]			show_y;
reg[23:0]		showfont_data_reg;
wire				onefont_finish;
wire[7:0]		fontdata;

assign onefont_finish  = (font_row == 1'b1 && font_index == 'd10 && write_done == 1'b1) ? 1'b1 : 1'b0;
assign ShowData_finish = (onefont_finish == 1'b1 && font_sel == 'd7) ? 1'b1 : 1'b0;
assign ShowData_Data = showfont_data_reg;

always@(*)
begin
	case(font_index)
	'd0:	showfont_data_reg <= {8'h78,8'h00,8'hB0 + show_y + font_row};
	'd1:  showfont_data_reg <= {8'h78,8'h00,8'h00 + show_x[3:0]};
	'd2:  showfont_data_reg <= {8'h78,8'h00,8'h10 + show_x[7:4]};
	default:	showfont_data_reg <= {8'h78,8'h40,fontdata}; //fontdata
	endcase
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_index <= 'd0;
	else if(write_done == 1'b1 && font_index == 'd10)
		font_index <= 'd0;
	else if(write_done == 1'b1 && ShowData_req == 1'b1)
		font_index <= font_index + 1'b1;
	else
		font_index <= font_index;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_row <= 1'b0;
	else if(onefont_finish == 1'b1)
		font_row <= 1'b0;
	else if(write_done == 1'b1 && font_index == 'd10)
		font_row <= 1'b1;
	else
		font_row <= font_row;
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_sel <= 'd0;
	else if(ShowData_finish == 1'b1)
		font_sel <= 'd0;
	else if(onefont_finish == 1'b1)
		font_sel <= font_sel + 1'b1;
	else
		font_sel <= font_sel;
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		show_x <= 'd0;
		show_y <= 'd0;
	end
	else if(font_sel == 'd0)
	begin
		show_x <= 'd54;
		show_y <= 'd3;
	end
	else if(font_sel == 'd1)
	begin
		show_x <= 'd62;
		show_y <= 'd3;
	end
	else if(font_sel == 'd2)
	begin
		show_x <= 'd79;
		show_y <= 'd3;
	end
	else if(font_sel == 'd3)
	begin
		show_x <= 'd88;
		show_y <= 'd3;
	end
	
	else if(font_sel == 'd4)
	begin
		show_x <= 'd54;
		show_y <= 'd5;
	end
	else if(font_sel == 'd5)
	begin
		show_x <= 'd62;
		show_y <= 'd5;
	end
	else if(font_sel == 'd6)
	begin
		show_x <= 'd79;
		show_y <= 'd5;
	end
	else if(font_sel == 'd7)
	begin
		show_x <= 'd88;
		show_y <= 'd5;
	end
	
	else
	begin
		show_x <= 'd0;
		show_y <= 'd0;
	end
end



always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font <= 'd0;
	else if(font_sel == 'd0)
		font <= tempHREG / 10;
	else if(font_sel == 'd1)
		font <= tempHREG % 10;
	else if(font_sel == 'd2)
		font <= tempLREG / 10;
	else if(font_sel == 'd3)
		font <= tempLREG % 10;
		
	else if(font_sel == 'd4)
		font <= humidityHREG / 10;
	else if(font_sel == 'd5)
		font <= humidityHREG % 10;
	else if(font_sel == 'd6)
		font <= humidityLREG / 10;
	else if(font_sel == 'd7)
		font <= humidityLREG % 10;
	else
		font <= font;		
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		tempHREG <= 'd0;
	else if(dht11_done == 1'b1)
		tempHREG <= tempH;
	else
		tempHREG <= tempHREG;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		tempLREG <= 'd0;
	else if(dht11_done == 1'b1)
		tempLREG <= tempL;
	else
		tempLREG <= tempLREG;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		humidityHREG <= 'd0;
	else if(dht11_done == 1'b1)
		humidityHREG <= humidityH;
	else
		humidityHREG <= humidityHREG;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		humidityLREG <= 'd0;
	else if(dht11_done == 1'b1)
		humidityLREG <= humidityL;
	else
		humidityLREG <= humidityLREG;
end



OLED_NumData OLED_NumDataHP(

	.sys_clk		(sys_clk),
	.rst_n		(rst_n),
	
	.font_row	(font_row),
	.font_sel	(font),
	.index		(font_index - 'd3),
	
	.data			(fontdata)

);

endmodule 