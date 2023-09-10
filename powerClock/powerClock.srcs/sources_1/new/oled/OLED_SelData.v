





module OLED_SelData(
	
	input				sys_clk,
	input				rst_n,
	
	
	input				init_req,
	input[23:0]		init_data,
	
	input				refresh_req,
	input[23:0]		refresh_data,
	
	input				showfont_req,
	input[23:0]	   showfont_data,
	
	input				showdata_req,
	input[23:0]		showdata_data, 
	
	output			IICWriteReq,
	output[23:0]	IICWriteData
);

reg       IICWriteReqReg;
reg[23:0] IICWriteDataReg; 

assign IICWriteReq  = init_req | showfont_req | refresh_req | showdata_req;
assign IICWriteData = (init_req == 1'b1) ? init_data : (refresh_req == 1'b1) ? refresh_data : (showfont_req == 1'b1) ? showfont_data : showdata_data;


endmodule 