




/*数字数据0-9*/
module	OLED_NumData(

	input			sys_clk,
	input			rst_n,
	
	input			font_row,
	input[4:0]	font_sel,
	input[4:0]	index,
	
	output reg[7:0]	 data

);




/*0-9*/
reg[7:0]	data0[15:0];
reg[7:0]	data1[15:0];
reg[7:0]	data2[15:0];
reg[7:0]	data3[15:0];
reg[7:0]	data4[15:0];
reg[7:0]	data5[15:0];
reg[7:0]	data6[15:0];
reg[7:0]	data7[15:0];
reg[7:0]	data8[15:0];
reg[7:0]	data9[15:0];


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		data <= 'd0;
	else if(font_sel == 'd0)
		data <= data0[index + 'd8 * font_row];
	else if(font_sel == 'd1)
		data <= data1[index + 'd8 * font_row];
	else if(font_sel == 'd2)
		data <= data2[index + 'd8 * font_row];
	else if(font_sel == 'd3)
		data <= data3[index + 'd8 * font_row];
	else if(font_sel == 'd4)
		data <= data4[index + 'd8 * font_row];
	else if(font_sel == 'd5)
		data <= data5[index + 'd8 * font_row];
	else if(font_sel == 'd6)
		data <= data6[index + 'd8 * font_row];
	else if(font_sel == 'd7)
		data <= data7[index + 'd8 * font_row];
	else if(font_sel == 'd8)
		data <= data8[index + 'd8 * font_row];
	else if(font_sel == 'd9)
		data <= data9[index + 'd8 * font_row];
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data0[0] = 8'h00;
		data0[1] = 8'hE0;
		data0[2] = 8'h10;
		data0[3] = 8'h08;
		data0[4] = 8'h08;
		data0[5] = 8'h10;
		data0[6] = 8'hE0;
		data0[7] = 8'h00;
		data0[8] = 8'h00;
		data0[9] = 8'h0F;
		data0[10] = 8'h10;
		data0[11] = 8'h20;
		data0[12] = 8'h20;
		data0[13] = 8'h10;
		data0[14] = 8'h0F;
		data0[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data1[0] = 8'h00;
		data1[1] = 8'h00;
		data1[2] = 8'h10;
		data1[3] = 8'h10;
		data1[4] = 8'hF8;
		data1[5] = 8'h00;
		data1[6] = 8'h00;
		data1[7] = 8'h00;
		data1[8] = 8'h00;
		data1[9] = 8'h00;
		data1[10] = 8'h20;
		data1[11] = 8'h20;
		data1[12] = 8'h3F;
		data1[13] = 8'h20;
		data1[14] = 8'h20;
		data1[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data2[0] = 8'h00;
		data2[1] = 8'h70;
		data2[2] = 8'h08;
		data2[3] = 8'h08;
		data2[4] = 8'h08;
		data2[5] = 8'h08;
		data2[6] = 8'hF0;
		data2[7] = 8'h00;
		data2[8] = 8'h00;
		data2[9] = 8'h30;
		data2[10] = 8'h28;
		data2[11] = 8'h24;
		data2[12] = 8'h22;
		data2[13] = 8'h21;
		data2[14] = 8'h30;
		data2[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data3[0] = 8'h00;
		data3[1] = 8'h30;
		data3[2] = 8'h08;
		data3[3] = 8'h08;
		data3[4] = 8'h08;
		data3[5] = 8'h88;
		data3[6] = 8'h70;
		data3[7] = 8'h00;
		data3[8] = 8'h00;
		data3[9] = 8'h18;
		data3[10] = 8'h20;
		data3[11] = 8'h21;
		data3[12] = 8'h21;
		data3[13] = 8'h22;
		data3[14] = 8'h1C;
		data3[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data4[0] = 8'h00;
		data4[1] = 8'h00;
		data4[2] = 8'h80;
		data4[3] = 8'h40;
		data4[4] = 8'h30;
		data4[5] = 8'hF8;
		data4[6] = 8'h00;
		data4[7] = 8'h00;
		data4[8] = 8'h00;
		data4[9] = 8'h06;
		data4[10] = 8'h05;
		data4[11] = 8'h24;
		data4[12] = 8'h24;
		data4[13] = 8'h3F;
		data4[14] = 8'h24;
		data4[15] = 8'h24;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data5[0] = 8'h00;
		data5[1] = 8'hF8;
		data5[2] = 8'h88;
		data5[3] = 8'h88;
		data5[4] = 8'h88;
		data5[5] = 8'h08;
		data5[6] = 8'h08;
		data5[7] = 8'h00;
		data5[8] = 8'h00;
		data5[9] = 8'h19;
		data5[10] = 8'h20;
		data5[11] = 8'h20;
		data5[12] = 8'h20;
		data5[13] = 8'h11;
		data5[14] = 8'h0E;
		data5[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data6[0] = 8'h00;
		data6[1] = 8'hE0;
		data6[2] = 8'h10;
		data6[3] = 8'h88;
		data6[4] = 8'h88;
		data6[5] = 8'h90;
		data6[6] = 8'h00;
		data6[7] = 8'h00;
		data6[8] = 8'h00;
		data6[9] = 8'h0F;
		data6[10] = 8'h11;
		data6[11] = 8'h20;
		data6[12] = 8'h20;
		data6[13] = 8'h20;
		data6[14] = 8'h1F;
		data6[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data7[0] = 8'h00;
		data7[1] = 8'h18;
		data7[2] = 8'h08;
		data7[3] = 8'h08;
		data7[4] = 8'h88;
		data7[5] = 8'h68;
		data7[6] = 8'h18;
		data7[7] = 8'h00;
		data7[8] = 8'h00;
		data7[9] = 8'h00;
		data7[10] = 8'h00;
		data7[11] = 8'h3E;
		data7[12] = 8'h01;
		data7[13] = 8'h00;
		data7[14] = 8'h00;
		data7[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data8[0] = 8'h00;
		data8[1] = 8'h70;
		data8[2] = 8'h88;
		data8[3] = 8'h08;
		data8[4] = 8'h08;
		data8[5] = 8'h88;
		data8[6] = 8'h70;
		data8[7] = 8'h00;
		data8[8] = 8'h00;
		data8[9] = 8'h1C;
		data8[10] = 8'h22;
		data8[11] = 8'h21;
		data8[12] = 8'h21;
		data8[13] = 8'h22;
		data8[14] = 8'h1C;
		data8[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data9[0] = 8'h00;
		data9[1] = 8'hF0;
		data9[2] = 8'h08;
		data9[3] = 8'h08;
		data9[4] = 8'h08;
		data9[5] = 8'h10;
		data9[6] = 8'hE0;
		data9[7] = 8'h00;
		data9[8] = 8'h00;
		data9[9] = 8'h01;
		data9[10] = 8'h12;
		data9[11] = 8'h22;
		data9[12] = 8'h22;
		data9[13] = 8'h11;
		data9[14] = 8'h0F;
		data9[15] = 8'h00;
	end


end
endmodule 