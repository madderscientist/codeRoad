



//刷新模块，将oled屏幕全部变为黑色
module OLED_Refresh(
	input					sys_clk,
	input					rst_n,
	
	input					refresh_req,				//初始化请求
	input					write_done,				//一组初始化数据完成信号
	
	output				refresh_finish,			//初始化完成输出

	output[23:0]		refresh_data		//初始化的数据
);

reg[23:0]	refresh_data_reg;
reg[10:0]	refresh_index;
reg[2:0]		page;
assign refresh_data = refresh_data_reg;
assign refresh_finish = (page == 'd7 && refresh_index == 'd130 && write_done == 1'b1) ? 1'b1 : 1'b0;

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		refresh_index <= 'd0;
	else if(refresh_index == 'd130 && write_done == 1'b1)
		refresh_index <= 'd0;
	else if(write_done == 1'b1)
		refresh_index <= refresh_index + 1'b1;
	else
		refresh_index <= refresh_index;
end


always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		page <= 'd0;
	else if(refresh_index == 'd130 && write_done == 1'b1)
		page <= page + 1'b1;
	else
		page <= page;
end


always@(*)
begin
	case(refresh_index)
	'd0:  refresh_data_reg <= {8'h78,8'h00,8'hB0 + page};
	'd1:  refresh_data_reg <= {8'h78,8'h00,8'h00};
	'd2:  refresh_data_reg <= {8'h78,8'h00,8'h10};
	default: refresh_data_reg <= {8'h78,8'h40,8'h00};
	endcase
end




endmodule 

