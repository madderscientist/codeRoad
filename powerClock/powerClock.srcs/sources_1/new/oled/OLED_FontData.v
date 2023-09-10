




//中文16*16 数字和字母8*16
module OLED_FontData(
	input		sys_clk,
	input		rst_n,
	input		font_row,
	input[5:0]	font_sel,
	input[8:0]	index,
	input[2:0]	State,
	output reg[7:0]	 data
);


reg[7:0] data0[15:0];   //F
reg[7:0] data1[15:0];   //G
reg[7:0] data2[15:0];   //P
reg[7:0] data3[15:0];   //A
reg[7:0] data4[31:0];   //之
reg[7:0] data5[31:0];   //旅

reg[7:0] data6[31:0];   //温
reg[7:0] data7[31:0];   //度
reg[7:0] data8[31:0];   //湿
reg[7:0] data9[31:0];   //度
reg[7:0] data10[15:0];   //.
reg[7:0] data11[31:0];   //℃
reg[7:0] data12[15:0];   //R
reg[7:0] data13[15:0];   //H

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 'd0)
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
		data <= data4[index + 'd16 * font_row];
	else if(font_sel == 'd5)
		data <= data5[index + 'd16 * font_row];
	else if(font_sel == 'd6)
		data <= data6[index + 'd16 * font_row];
	else if(font_sel == 'd7)
		data <= data7[index + 'd16 * font_row];
	else if(font_sel == 'd8)
		data <= data8[index + 'd16 * font_row];
	else if(font_sel == 'd9)
		data <= data9[index + 'd16 * font_row];
	else if(font_sel == 'd10)
		data <= data10[index + 'd8 * font_row];
	else if(font_sel == 'd11)
		data <= data11[index + 'd16 * font_row];
	else if(font_sel == 'd12)
		data <= data12[index + 'd8 * font_row];
	else if(font_sel == 'd13)
		data <= data13[index + 'd8 * font_row];
	else if(font_sel == 'd14)
		data <= data10[index + 'd8 * font_row];
	else
		data <= data;
end

// 左1
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin 
	    if( State == 'd0 || State == 'd1 || State == 'd2 || State == 'd3 || State == 'd4 || State == 'd5 ) begin  
	       
		data0[0] = 8'h00;
		data0[1] = 8'h00;
		data0[2] = 8'h00;
		data0[3] = 8'h00;
		data0[4] = 8'h00;
		data0[5] = 8'h00;
		data0[6] = 8'h00;
		data0[7] = 8'h00;
		data0[8] = 8'h00;
		data0[9] = 8'h00;
		data0[10] = 8'h00;
		data0[11] = 8'h00;
		data0[12] = 8'h00;
		data0[13] = 8'h00;
		data0[14] = 8'h00;
		data0[15] = 8'h00;
	    end else ;
     end
end

//左2
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin   
	     if( State == 'd0 || State == 'd1 || State == 'd2 || State == 'd3 || State == 'd4 || State == 'd5 ) begin
		     data1[0] = 8'h00;
		     data1[1] = 8'h00;
		     data1[2] = 8'h00;
		     data1[3] = 8'h00;
		     data1[4] = 8'h00;
		     data1[5] = 8'h00;
		     data1[6] = 8'h00;
		     data1[7] = 8'h00;
		     data1[8] = 8'h00;
		     data1[9] = 8'h00;
		     data1[10] = 8'h00;
		     data1[11] = 8'h00;
		     data1[12] = 8'h00;
		     data1[13] = 8'h00;
		     data1[14] = 8'h00;
		     data1[15] = 8'h00;
		  end else ;
	end
end

//左三
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin 
	    if( State == 'd0 || State == 'd1 || State == 'd2 || State == 'd3 || State == 'd4 || State == 'd5 )
	    begin
		   data2[0] = 8'h00;
		   data2[1] = 8'h00;
		   data2[2] = 8'h00;
		   data2[3] = 8'h00;
		   data2[4] = 8'h00;
		   data2[5] = 8'h00;
		   data2[6] = 8'h00;
		   data2[7] = 8'h00;
		   data2[8] = 8'h00;
		   data2[9] = 8'h00;
		   data2[10] = 8'h00;
		   data2[11] = 8'h00;
		   data2[12] = 8'h00;
		   data2[13] = 8'h00;
		   data2[14] = 8'h00;
		   data2[15] = 8'h00;
		end else;
	end
end

//左四
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
	    if(State == 'd0 || State == 'd1 || State == 'd5 ) begin
		   data3[0] = 8'h00;
		   data3[1] = 8'h00;
		   data3[2] = 8'h00;
		   data3[3] = 8'h00;
		   data3[4] = 8'h00;
		   data3[5] = 8'h00;
           data3[6] = 8'h00;
		   data3[7] = 8'h00;
		   data3[8] = 8'h00;
		   data3[9] = 8'h00;
		   data3[10] = 8'h00;
		   data3[11] = 8'h00;
		   data3[12] = 8'h00;
		   data3[13] = 8'h00;
		   data3[14] = 8'h00;
		   data3[15] = 8'h00;
		end
		//1
		else if(State == 2) begin
           data3[0] =  8'h00;
           data3[1] =  8'h00;
           data3[2] =  8'h10;
           data3[3] =  8'h10;
           data3[4] =  8'hF8;
           data3[5] =  8'h00;
           data3[6] =  8'h00;
           data3[7] =  8'h00;
           data3[8] =  8'h00;
           data3[9] =  8'h00;
           data3[10] =  8'h20;
           data3[11] =  8'h20;
           data3[12] =  8'h3F;
           data3[13] =  8'h20;
           data3[14] =  8'h20;
           data3[15] =  8'h00;		
		end
		
		 //2
		else if(State == 3) begin
           data3[0] =  8'h00;
           data3[1] =  8'h70;
           data3[2] =  8'h08;
           data3[3] =  8'h08;
           data3[4] =  8'h08;
           data3[5] =  8'h08;
           data3[6] =  8'hF0;
           data3[7] =  8'h00;
           data3[8] =  8'h00;
           data3[9] =  8'h30;
           data3[10] =  8'h28;
           data3[11] =  8'h24;
           data3[12] =  8'h22;
           data3[13] =  8'h21;
           data3[14] =  8'h30;
           data3[15] =  8'h00;
		end
		
		//3
		else begin 		
           data3[0] =  8'h00;
           data3[1] =  8'h30;
           data3[2] =  8'h08;
           data3[3] =  8'h08;
           data3[4] =  8'h08;
           data3[5] =  8'h88;
           data3[6] =  8'h70;
           data3[7] =  8'h00;
           data3[8] =  8'h00;
           data3[9] =  8'h18;
           data3[10] =  8'h20;
           data3[11] =  8'h21;
           data3[12] =  8'h21;
           data3[13] =  8'h22;
           data3[14] =  8'h1C;
           data3[15] =  8'h00;
		end
	end
end

//上字1
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
	    if(State == 3'b000)
	    begin//时
		  data4[0] = 8'h00;
		  data4[1] = 8'hFC;
		  data4[2] = 8'h84;
		  data4[3] = 8'h84;
		  data4[4] = 8'h84;
		  data4[5] = 8'hFC;
		  data4[6] = 8'h00;
		  data4[7] = 8'h10;
		  data4[8] = 8'h10;
		  data4[9] = 8'h10;
		  data4[10] = 8'h10;
		  data4[11] = 8'h10;
		  data4[12] = 8'hFF;
		  data4[13] = 8'h10;
		  data4[14] = 8'h10;
		  data4[15] = 8'h00;  
		  data4[16] = 8'h00;
		  data4[17] = 8'h3F;
		  data4[18] = 8'h10;
		  data4[19] = 8'h10; 
		  data4[20] = 8'h10;
		  data4[21] = 8'h3F;
		  data4[22] = 8'h00;
		  data4[23] = 8'h00;
		  data4[24] = 8'h01;
		  data4[25] = 8'h06;
		  data4[26] = 8'h40; 
		  data4[27] = 8'h80;
          data4[28] = 8'h7F; 
          data4[29] = 8'h00;
          data4[30] = 8'h00;
          data4[31] = 8'h00;
		end
		
		else if(State == 3'b001) begin //日
		   data4[0] = 8'h00;
		   data4[1] = 8'h00;
		   data4[2] = 8'h00;
		   data4[3] = 8'hFE;
		   data4[4] = 8'h82;
		   data4[5] = 8'h82;
		   data4[6] = 8'h82;
		   data4[7] = 8'h82;
		   data4[8] = 8'h82;
		   data4[9] = 8'h82;
		   data4[10] = 8'h82;
		   data4[11] = 8'hFE;
		   data4[12] = 8'h00;
		   data4[13] = 8'h00;
		   data4[14] = 8'h00;
		   data4[15] = 8'h00;
           data4[16] = 8'h00;
           data4[17] = 8'h00;
           data4[18] = 8'h00;
           data4[19] = 8'hFF;
           data4[20] = 8'h40;
           data4[21] = 8'h40;
           data4[22] = 8'h40;
           data4[23] = 8'h40;
           data4[24] = 8'h40;
           data4[25] = 8'h40;
           data4[26] = 8'h40;
           data4[27] = 8'hFF;
           data4[28] = 8'h00;
           data4[29] = 8'h00;
           data4[30] = 8'h00;
           data4[31] = 8'h00;
		end
		else if( State == 3'b010 || State == 3'b011 || State == 3'b100 ) begin //闹
		   data4[0] = 8'h00;
		   data4[1] = 8'hF8;
		   data4[2] = 8'h01;
		   data4[3] = 8'h22;
		   data4[4] = 8'h20;
		   data4[5] = 8'h22;
		   data4[6] = 8'h2A;
		   data4[7] = 8'hF2;
		   data4[8] = 8'h22;
		   data4[9] = 8'h22;
		   data4[10] = 8'h22;
		   data4[11] = 8'h22;
		   data4[12] = 8'h02;
		   data4[13] = 8'hFE;
		   data4[14] = 8'h00;
		   data4[15] = 8'h00;
           data4[16] = 8'h00;
           data4[17] = 8'hFF;
           data4[18] = 8'h00;
           data4[19] = 8'h00;
           data4[20] = 8'h1F;
           data4[21] = 8'h01;
           data4[22] = 8'h01;
           data4[23] = 8'h7F;
           data4[24] = 8'h09;
           data4[25] = 8'h11;
           data4[26] = 8'h0F;
           data4[27] = 8'h40;
           data4[28] = 8'h80;
           data4[29] = 8'h7F;
           data4[30] = 8'h00;
           data4[31] = 8'h00;
		end
		
		else begin //倒
		   data4[0] = 8'h80;
		   data4[1] = 8'h60;
		   data4[2] = 8'hF8;
		   data4[3] = 8'h07;
		   data4[4] = 8'h04;
		   data4[5] = 8'h64;
		   data4[6] = 8'h5C;
		   data4[7] = 8'hC4;
		   data4[8] = 8'h64;
		   data4[9] = 8'h44;
		   data4[10] = 8'h00;
		   data4[11] = 8'hF8;
		   data4[12] = 8'h00;
		   data4[13] = 8'hFF;
		   data4[14] = 8'h00;
		   data4[15] = 8'h00;
           data4[16] = 8'h00;
           data4[17] = 8'h00;
           data4[18] = 8'hFF;
           data4[19] = 8'h00;
           data4[20] = 8'h20;
           data4[21] = 8'h62;
           data4[22] = 8'h22;
           data4[23] = 8'h1F;
           data4[24] = 8'h12;
           data4[25] = 8'h12;
           data4[26] = 8'h00;
           data4[27] = 8'h4F;
           data4[28] = 8'h80;
           data4[29] = 8'h7F;
           data4[30] = 8'h00;
           data4[31] = 8'h00;
		end
	end
end

//上字2
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
	   if(State == 3'b0 || State == 3'b010 || State == 3'b011 || State == 3'b100 )
	      begin //钟
          data5[0] = 8'h20;
          data5[1] = 8'h10;
          data5[2] = 8'h2C;
          data5[3] = 8'hE7;
          data5[4] = 8'h24;
          data5[5] = 8'h24;
          data5[6] = 8'h00;
          data5[7] = 8'hF0;
          data5[8] = 8'h10;
          data5[9] = 8'h10;
          data5[10] = 8'hFF;
          data5[11] = 8'h10;
          data5[12] = 8'h10;
          data5[13] = 8'hF0;
          data5[14] = 8'h00;
          data5[15] = 8'h00;
          data5[16] = 8'h01;
          data5[17] = 8'h01;
          data5[18] = 8'h01;
          data5[19] = 8'h7F;
          data5[20] = 8'h21;
          data5[21] = 8'h11;
          data5[22] = 8'h00;
          data5[23] = 8'h07;
          data5[24] = 8'h02;
          data5[25] = 8'h02;
          data5[26] = 8'hFF;
          data5[27] = 8'h02;
          data5[28] = 8'h02;
          data5[29] = 8'h07;
          data5[30] = 8'h00;
          data5[31] = 8'h00;   
		  end 
		  
		  //期
		  else if(State == 3'b001) begin
		      data5[0] = 8'h00;
		      data5[1] = 8'h04;
		      data5[2] = 8'hFF;
		      data5[3] = 8'h24;
		      data5[4] = 8'h24;
		      data5[5] = 8'h24;
		      data5[6] = 8'hFF;
		      data5[7] = 8'h04;
		      data5[8] = 8'h00;
		      data5[9] = 8'hFE;
		      data5[10] = 8'h22;
		      data5[11] = 8'h22;
		      data5[12] = 8'h22;
		      data5[13] = 8'hFE;
		      data5[14] = 8'h00;
		      data5[15] = 8'h00;
              data5[16] = 8'h88;
              data5[17] = 8'h48;
              data5[18] = 8'h2F;
              data5[19] = 8'h09;
              data5[20] = 8'h09;
              data5[21] = 8'h19;
              data5[22] = 8'hAF;
              data5[23] = 8'h48;
              data5[24] = 8'h30;
              data5[25] = 8'h0F;
              data5[26] = 8'h02;
              data5[27] = 8'h42;
              data5[28] = 8'h82;
              data5[29] = 8'h7F;
              data5[30] = 8'h00;
              data5[31] = 8'h00;
		  end
		  //时
		  else begin
		      data5[0] = 8'h00;
		      data5[1] = 8'hFC;
		      data5[2] = 8'h84;
		      data5[3] = 8'h84;
		      data5[4] = 8'h84;
		      data5[5] = 8'hFC;
		      data5[6] = 8'h00;
		      data5[7] = 8'h10;
		      data5[8] = 8'h10;
		      data5[9] = 8'h10;
		      data5[10] = 8'h10;
		      data5[11] = 8'h10;
		      data5[12] = 8'hFF;
		      data5[13] = 8'h10;
		      data5[14] = 8'h10;
		      data5[15] = 8'h00;  
		      data5[16] = 8'h00;
		      data5[17] = 8'h3F;
		      data5[18] = 8'h10;
		      data5[19] = 8'h10; 
		      data5[20] = 8'h10;
		      data5[21] = 8'h3F;
		      data5[22] = 8'h00;
		      data5[23] = 8'h00;
		      data5[24] = 8'h01;
		      data5[25] = 8'h06;
		      data5[26] = 8'h40; 
		      data5[27] = 8'h80;
              data5[28] = 8'h7F; 
              data5[29] = 8'h00;
              data5[30] = 8'h00;
              data5[31] = 8'h00;
		  end
	end
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
	     if(State == 3'b0 || State == 3'b010 || State == 3'b011 || State == 3'b100 || State == 3'b101 )
	      begin //时
		      data6[0] = 8'h00;
		      data6[1] = 8'hFC;
		      data6[2] = 8'h84;
		      data6[3] = 8'h84;
		      data6[4] = 8'h84;
		      data6[5] = 8'hFC;
		      data6[6] = 8'h00;
		      data6[7] = 8'h10;
		      data6[8] = 8'h10;
		      data6[9] = 8'h10;
		      data6[10] = 8'h10;
		      data6[11] = 8'h10;
		      data6[12] = 8'hFF;
		      data6[13] = 8'h10;
		      data6[14] = 8'h10;
		      data6[15] = 8'h00;  
		      data6[16] = 8'h00;
		      data6[17] = 8'h3F;
		      data6[18] = 8'h10;
		      data6[19] = 8'h10; 
		      data6[20] = 8'h10;
		      data6[21] = 8'h3F;
		      data6[22] = 8'h00;
		      data6[23] = 8'h00;
		      data6[24] = 8'h01;
		      data6[25] = 8'h06;
		      data6[26] = 8'h40; 
		      data6[27] = 8'h80;
              data6[28] = 8'h7F; 
              data6[29] = 8'h00;
              data6[30] = 8'h00;
              data6[31] = 8'h00; 
		end
		//年
		else if( State == 3'b001 ) begin
		      data6[0] = 8'h00;
		      data6[1] = 8'h20;
		      data6[2] = 8'h18;
		      data6[3] = 8'hC7;
		      data6[4] = 8'h44;
		      data6[5] = 8'h44;
		      data6[6] = 8'h44;
		      data6[7] = 8'h44;
		      data6[8] = 8'hFC;
		      data6[9] = 8'h44;
		      data6[10] = 8'h44;
		      data6[11] = 8'h44;
		      data6[12] = 8'h44;
		      data6[13] = 8'h04;
		      data6[14] = 8'h00;
		      data6[15] = 8'h00;
              data6[16] = 8'h04;
              data6[17] = 8'h04;
              data6[18] = 8'h04;
              data6[19] = 8'h07;
              data6[20] = 8'h04;
              data6[21] = 8'h04;
              data6[22] = 8'h04;
              data6[23] = 8'h04;
              data6[24] = 8'hFF;
              data6[25] = 8'h04;
              data6[26] = 8'h04;
              data6[27] = 8'h04;
              data6[28] = 8'h04;
              data6[29] = 8'h04;
              data6[30] = 8'h04;
              data6[31] = 8'h00;
		end
		else ;
	end
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
	   //分
	    if(State == 3'b0 || State == 3'b010 || State == 3'b011 || State == 3'b100 || State == 3'b101 )
	       begin
	           data7[0] = 8'h80;
	           data7[1] = 8'h40;
	           data7[2] = 8'h20;
	           data7[3] = 8'h90;
	           data7[4] = 8'h88;
	           data7[5] = 8'h86;
	           data7[6] = 8'h80;
	           data7[7] = 8'h80;
	           data7[8] = 8'h80;
	           data7[9] = 8'h83;
	           data7[10] = 8'h8C;
	           data7[11] = 8'h10;
	           data7[12] = 8'h20;
	           data7[13] = 8'h40;
	           data7[14] = 8'h80;
	           data7[15] = 8'h00;
               data7[16] = 8'h00;
               data7[17] = 8'h80;
               data7[18] = 8'h40;
               data7[19] = 8'h20;
               data7[20] = 8'h18;
               data7[21] = 8'h07;
               data7[22] = 8'h00;
               data7[23] = 8'h40;
               data7[24] = 8'h80;
               data7[25] = 8'h40;
               data7[26] = 8'h3F;
               data7[27] = 8'h00;
               data7[28] = 8'h00;
               data7[29] = 8'h00;
               data7[30] = 8'h00;
               data7[31] = 8'h00;
		   end
		   //份
	   else if( State == 3'b001 ) begin     
               data7[0] = 8'h00;
               data7[1] = 8'h80;
               data7[2] = 8'h60;
               data7[3] = 8'hF8;
               data7[4] = 8'h07;
               data7[5] = 8'h80;
               data7[6] = 8'h40;
               data7[7] = 8'hB0;
               data7[8] = 8'h8E;
               data7[9] = 8'h80;
               data7[10] = 8'h80;
               data7[11] = 8'h87;
               data7[12] = 8'h98;
               data7[13] = 8'h60;
               data7[14] = 8'h80;
               data7[15] = 8'h00;
               data7[16] = 8'h01;
               data7[17] = 8'h00;
               data7[18] = 8'h00;
               data7[19] = 8'hFF;
               data7[20] = 8'h00;
               data7[21] = 8'h80;
               data7[22] = 8'h40;
               data7[23] = 8'h30;
               data7[24] = 8'h0F;
               data7[25] = 8'h00;
               data7[26] = 8'h40;
               data7[27] = 8'h80;
               data7[28] = 8'h7F;
               data7[29] = 8'h00;
               data7[30] = 8'h00;
               data7[31] = 8'h00;
	       end else ;
		   
	end
end

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		if(State == 3'b0 || State == 3'b010 || State == 3'b011 || State == 3'b100 || State == 3'b101 )
	       begin//秒
	       data8[0] = 8'h24;
	       data8[1] = 8'h24;
	       data8[2] = 8'hA4;
	       data8[3] = 8'hFE;
	       data8[4] = 8'h23;
	       data8[5] = 8'h22;
	       data8[6] = 8'h00;
	       data8[7] = 8'hC0;
	       data8[8] = 8'h38;
	       data8[9] = 8'h00;
	       data8[10] = 8'hFF;
	       data8[11] = 8'h00;
	       data8[12] = 8'h08;
	       data8[13] = 8'h10;
	       data8[14] = 8'h60;
	       data8[15] = 8'h00;
           data8[16] = 8'h08;
           data8[17] = 8'h06;
           data8[18] = 8'h01;
           data8[19] = 8'hFF;
           data8[20] = 8'h01;
           data8[21] = 8'h06;
           data8[22] = 8'h81;
           data8[23] = 8'h80;
           data8[24] = 8'h40;
           data8[25] = 8'h40;
           data8[26] = 8'h27;
           data8[27] = 8'h10;
           data8[28] = 8'h0C;
           data8[29] = 8'h03;
           data8[30] = 8'h00;
           data8[31] = 8'h00;
	       end
	       //月
	       else if( State == 3'b001 ) begin 
	        data8[0] = 8'h00;
	        data8[1] = 8'h00;
	        data8[2] = 8'h00;
	        data8[3] = 8'hFE;
	        data8[4] = 8'h22;
	        data8[5] =  8'h22;
	        data8[6] = 8'h22;
	        data8[7] = 8'h22;
	        data8[8] = 8'h22;
	        data8[9] = 8'h22;
	        data8[10] = 8'h22;
	        data8[11] = 8'h22;
	        data8[12] = 8'hFE;
	        data8[13] = 8'h00;
	        data8[14] = 8'h00;
	        data8[15] = 8'h00;
            data8[16] = 8'h80;
            data8[17] = 8'h40;
            data8[18] = 8'h30;
            data8[19] = 8'h0F;
            data8[20] = 8'h02;
            data8[21] = 8'h02;
            data8[22] = 8'h02;
            data8[23] = 8'h02;
            data8[24] = 8'h02;
            data8[25] = 8'h02;
            data8[26] = 8'h42;
            data8[27] = 8'h82;
            data8[28] = 8'h7F;
            data8[29] = 8'h00;
            data8[30] = 8'h00;
            data8[31] = 8'h00;
	       end else ;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		if(State == 3'b0 || State == 3'b010 || State == 3'b011 || State == 3'b100 || State == 3'b101 )
	       begin//毫
				data9[0] = 8'h00;
				data9[1] =8'h82;
				data9[2] =8'h82;
				data9[3] =8'hBA;
				data9[4] =8'hAA;
				data9[5] =8'hAA;
				data9[6] =8'hAA;
				data9[7] =8'hAB;
				data9[8] =8'hAA;
				data9[9] =8'hAA;
				data9[10] =8'hAA;
				data9[11] =8'hBA;
				data9[12] =8'h82;
				data9[13] =8'h82;
				data9[14] =8'h80;
				data9[15] =8'h00;
				data9[16] =8'h02;
				data9[17] =8'h41;
				data9[18] =8'h54;
				data9[19] =8'h54;
				data9[20] =8'h54;
				data9[21] =8'h54;
				data9[22] =8'h7C;
				data9[23] =8'hAA;
				data9[24] =8'hAA;
				data9[25] =8'hAA;
				data9[26] =8'hAA;
				data9[27] =8'hAA;
				data9[28] =8'hA0;
				data9[29] =8'h82;
				data9[30] =8'hE1;
				data9[31] =8'h00;
	       end
	       
	       //日
	       else if( State == 3'b001 ) begin
				data9[0] = 8'h00;
				data9[1] = 8'h00;
				data9[2] = 8'h00;
				data9[3] = 8'hFE;
				data9[4] = 8'h82;
				data9[5] = 8'h82;
				data9[6] = 8'h82;
				data9[7] = 8'h82;
				data9[8] = 8'h82;
				data9[9] = 8'h82;
				data9[10] = 8'h82;
				data9[11] = 8'hFE;
				data9[12] = 8'h00;
				data9[13] = 8'h00;
				data9[14] = 8'h00;
				data9[15] = 8'h00;
				data9[16] = 8'h00;
				data9[17] = 8'h00;
				data9[18] = 8'h00;
				data9[19] = 8'hFF;
				data9[20] = 8'h40;
				data9[21] = 8'h40;
				data9[22] = 8'h40;
				data9[23] = 8'h40;
				data9[24] = 8'h40;
				data9[25] = 8'h40;
				data9[26] = 8'h40;
				data9[27] = 8'hFF;
				data9[28] = 8'h00;
				data9[29] = 8'h00;
				data9[30] = 8'h00;
				data9[31] = 8'h00;
	       end else ;
	       
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data10[0] = 8'h00;
		data10[1] = 8'h00;
		data10[2] = 8'h00;
		data10[3] = 8'h00;
		data10[4] = 8'h00;
		data10[5] = 8'h00;
		data10[6] = 8'h00;
		data10[7] = 8'h00;
		data10[8] = 8'h00;
		data10[9] = 8'h30;
		data10[10] = 8'h30;
		data10[11] = 8'h00;
		data10[12] = 8'h00;
		data10[13] = 8'h00;
		data10[14] = 8'h00;
		data10[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data11[0] = 8'h00;
		data11[1] = 8'h00;
		data11[2] = 8'h00;
		data11[3] = 8'h00;
		data11[4] = 8'h00;
		data11[5] = 8'h00;
		data11[6] = 8'h00;
		data11[7] = 8'h00;
		data11[8] = 8'h00;
		data11[9] = 8'h00;
		data11[10] = 8'h00;
		data11[11] = 8'h00;
		data11[12] = 8'h00;
		data11[13] = 8'h00;
		data11[14] = 8'h00;
		data11[15] = 8'h00;
		data11[16] = 8'h00;
		data11[17] = 8'h00;
		data11[18] = 8'h00;
		data11[19] = 8'h00;
		data11[20] = 8'h00;
		data11[21] = 8'h00;
		data11[22] = 8'h00;
		data11[23] = 8'h00;
		data11[24] = 8'h00;
		data11[25] = 8'h00;
		data11[26] = 8'h00;
		data11[27] = 8'h00;
		data11[28] = 8'h00;
		data11[29] = 8'h00;
		data11[30] = 8'h00;
		data11[31] = 8'h00;
	end
end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data12[0] = 8'h00;
		data12[1] = 8'h00;
		data12[2] = 8'h00;
		data12[3] = 8'h00;
		data12[4] = 8'h00;
		data12[5] = 8'h00;
		data12[6] = 8'h00;
		data12[7] = 8'h00;
		data12[8] = 8'h00;
		data12[9] = 8'h00;
		data12[10] = 8'h00;
		data12[11] = 8'h00;
		data12[12] = 8'h00;
		data12[13] = 8'h00;
		data12[14] = 8'h00;
		data12[15] = 8'h00;
	end


end
always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
	begin
		data13[0] = 8'h00;
		data13[1] = 8'h00;
		data13[2] = 8'h00;
		data13[3] = 8'h00;
		data13[4] = 8'h00;
		data13[5] = 8'h00;
		data13[6] = 8'h00;
		data13[7] = 8'h00;
		data13[8] = 8'h00;
		data13[9] = 8'h00;
		data13[10] = 8'h00;
		data13[11] = 8'h00;
		data13[12] = 8'h00;
		data13[13] = 8'h00;
		data13[14] = 8'h00;
		data13[15] = 8'h00;
	end

end

endmodule 