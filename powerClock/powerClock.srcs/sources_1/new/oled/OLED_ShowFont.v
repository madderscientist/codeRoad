






module OLED_ShowFont(

	input				sys_clk,
	input				rst_n,
	
	input				ShowFont_req,		 //字符显示请求
	input				write_done,			 //iic一组数据写完成
	input [2:0]             State,
	output[23:0]	ShowFont_Data,		 //字符显示数据
	
	output			ShowFont_finish   //字符显示完成
);

reg[8:0]	   showfont_index;
reg[23:0]	showfont_data_reg;

wire[7:0]	fontdata;


reg[5:0]		font_index; //当前显示第几个字符
reg[1:0]		font_size; //1 :16*16   0 : 8*16
reg[7:0]		show_x;
reg[3:0]		show_y;
reg			font_row;

wire onefont_finish;


assign onefont_finish = ((showfont_index == ('d10 + 'd8 * font_size)) && (font_row == 1'b1) && write_done == 1'b1) ? 1'b1 : 1'b0;
assign ShowFont_finish = (onefont_finish == 1'b1 && font_index == 'd14) ? 1'b1 : 1'b0;

assign ShowFont_Data = showfont_data_reg;

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		showfont_index <= 'd0;
	else if(onefont_finish == 1'b1)
		showfont_index <= 'd0;
	else if(font_size == 'd0 && showfont_index == 'd10 && write_done == 1'b1)
		showfont_index <= 'd0;
	else if(font_size == 'd1 && showfont_index == 'd18 && write_done == 1'b1)
		showfont_index <= 'd0;
	else if(write_done == 1'b1 && ShowFont_req == 1'b1)
		showfont_index <= showfont_index + 1'b1;
	else
		showfont_index <= showfont_index;
end

always@(*)
begin
	case(showfont_index)
	'd0:	showfont_data_reg <= {8'h78,8'h00,8'hB0 + show_y + font_row};
	'd1:  showfont_data_reg <= {8'h78,8'h00,8'h00 + show_x[3:0]};
	'd2:  showfont_data_reg <= {8'h78,8'h00,8'h10 + show_x[7:4]};
	default:	showfont_data_reg <= {8'h78,8'h40,fontdata}; //fontdata
	endcase
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_row <= 1'b0;
	else if(onefont_finish == 1'b1)
		font_row <= 1'b0;
	else if(font_size == 'd0 && showfont_index == 'd10 && write_done == 1'b1)
		font_row <= 1'b1;
	else if(font_size == 'd1 && showfont_index == 'd18 && write_done == 1'b1)
		font_row <= 1'b1;
	else
		font_row <= font_row;
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_size <= 'd0;
	else if(font_index <= 'd3)
		font_size <= 'd0;
	else if(font_index == 'd10 || font_index == 'd12 || font_index == 'd13 || font_index == 'd14)
		font_size <= 'd0;
	else
		font_size <= 'd1;
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		font_index <= 'd0;
	else if(ShowFont_finish == 1'b1)
		font_index <= 'd0;
	else if(onefont_finish == 1'b1)
		font_index <= font_index + 1'b1;
	else
		font_index <= font_index;
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)                                   //F
	begin
		show_x <= 'd8;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd0) //P
	begin
		show_x <= 'd24;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd1)  //G
	begin
		show_x <= 'd40;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd2)  //A
	begin
		show_x <= 'd56;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd3) //之
	begin
		show_x <= 'd77;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd4) //旅
	begin
		show_x <= 'd100;
		show_y <= 'd0;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd5)	//温
	begin
		show_x <= 'd10;
		show_y <= 'd3;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd6)	 //度
	begin
		show_x <= 'd30;
		show_y <= 'd3;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd7)	//湿
	begin
		show_x <= 'd10;
		show_y <= 'd5;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd8)	 //度
	begin
		show_x <= 'd30;
		show_y <= 'd5;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd9)	 //.
	begin
		show_x <= 'd70;
		show_y <= 'd3;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd10)	 //℃
	begin
		show_x <= 'd100;
		show_y <= 'd3;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd11)	 //R
	begin
		show_x <= 'd100;
		show_y <= 'd5;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd12)	 //H
	begin
		show_x <= 'd108;
		show_y <= 'd5;
	end
	else if(onefont_finish == 1'b1 && font_index == 'd13)	 //.
	begin
		show_x <= 'd70;
		show_y <= 'd5;
	end
	else
	begin
		show_x <= show_x;
		show_y <= show_y;
	end

end

OLED_FontData OLED_FontData_HP(
	
	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	.font_row 		(font_row),
	.font_sel		(font_index),
	.State          (State),
	.index			(showfont_index - 'd3),
	.data				(fontdata)
);


endmodule 