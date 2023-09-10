//OLED顶层模块
module OLED_Top(

	input			sys_clk,
	input			rst_n,

	//DHT11数值显示
	input				dht11_done,
	input[7:0]	   tempH,	
	input[7:0]	   tempL,
	input[7:0]	   humidityH,
	input[7:0]	   humidityL,
	input[2:0]    State,
	
	
	//OLED IIC
	output		OLED_SCL,
	inout			OLED_SDA
	
);



localparam	OLED_INIT 		=  'd0;			//初始化
localparam	OLED_Refresh	=  'd1;			//刷新，将oled全部写0
localparam  OLED_ShowFont	=	'd2;			//显示字符
localparam	OLED_IDLE 		=  'd3;			//空闲
localparam	OLED_ShowData	=	'd4;			//显示数据




reg[4:0]	state , next_state;

//IIC相关信号
wire			IICWriteReq;
wire[23:0]  IICWriteData;
wire			IICWriteDone;

//初始化相关信号
wire			init_finish;
wire[23:0]  Init_data;
wire 			init_req;

//refresh相关信号
wire			refresh_finish;
wire[23:0]	refresh_data;
wire		   refresh_req;


//字符显示相关信号
wire			showfont_finish;
wire[23:0]	showfont_data;
wire			showfont_req;


//显示数据相关信号
wire			showdata_finish;
wire[23:0]	showdata_data;
wire			showdata_req;

assign init_req     = (state == OLED_INIT)     ? 1'b1 : 1'b0;

assign refresh_req  = (state == OLED_Refresh)  ? 1'b1 : 1'b0;

assign showfont_req = (state == OLED_ShowFont) ? 1'b1 : 1'b0;

assign showdata_req = (state == OLED_ShowData) ? 1'b1 : 1'b0;

always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		state <= OLED_INIT;
	else
		state <= next_state;
end

always@(*)
begin
	case(state)
	OLED_INIT:
		if(init_finish == 1'b1)
			next_state <= OLED_Refresh;
		else
			next_state <= OLED_INIT;
	OLED_Refresh:
		if(refresh_finish == 1'b1)
			next_state <= OLED_ShowFont;
		else
			next_state <= OLED_Refresh;
	OLED_ShowFont:
		if(showfont_finish == 1'b1)
			next_state <= OLED_IDLE;
		else
			next_state <= OLED_ShowFont;
	OLED_IDLE:
		if(dht11_done == 1'b1)
			next_state <= OLED_ShowData;
		else
			next_state <= OLED_IDLE;
	OLED_ShowData:
		if(showdata_finish == 1'b1)
			next_state <= OLED_IDLE;
		else
			next_state <= OLED_ShowData;
	default: next_state <= OLED_INIT;
	endcase
end





OLED_Init OLED_InitHP(
	
	.sys_clk				(sys_clk),
	.rst_n				(rst_n),
	
	.init_req			(init_req),				//初始化请求
	.write_done			(IICWriteDone),			//一组初始化数据完成信号
	
	.init_finish		(init_finish),			//初始化完成输出

	.Init_data			(Init_data)//初始化的数据
);


OLED_Refresh(
	.sys_clk				(sys_clk),
	.rst_n				(rst_n),
	
	.refresh_req		(refresh_req),				//初始化请求
	.write_done			(IICWriteDone),				//一组初始化数据完成信号
	
	.refresh_finish	(refresh_finish),			//初始化完成输出

	.refresh_data		(refresh_data)	//初始化的数据
);


OLED_ShowFont OLED_ShowFont_HP(

	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	
	.ShowFont_req	(showfont_req),		    //字符显示请求
	.write_done		(IICWriteDone),			 //iic一组数据写完成
	.State          (State),
	.ShowFont_Data		(showfont_data),		 //字符显示数据
	
	.ShowFont_finish(showfont_finish)       //字符显示完成
);

OLED_ShowData OLED_ShowDataHP(
	
	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	
	.dht11_done		(dht11_done),
	.tempH			(tempH),			//温度数据整数
	.tempL			(tempL),			//温度数据小数
	.humidityH		(humidityH),		//温度数据整数
	.humidityL		(humidityL),		//温度数据小数
	
	
	.ShowData_req	(showdata_req),		 //字符显示请求
	.write_done		(IICWriteDone),			 //iic一组数据写完成
	
	.ShowData_Data	(showdata_data),		 //字符显示数据
	
	.ShowData_finish	(showdata_finish)   //字符显示完成

);


//数据选择
OLED_SelData OLED_SelDataHP(
	
	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	
	
	.init_req		(init_req),
	.init_data		(Init_data),
	
	.refresh_req	(refresh_req),
	.refresh_data	(refresh_data),
		
	.showfont_req	(showfont_req),
	.showfont_data	(showfont_data),
	
	.showdata_req	(showdata_req),
	.showdata_data	(showdata_data),
	
	
	.IICWriteReq	(IICWriteReq),	
	.IICWriteData	(IICWriteData)
);



IIC_Driver IIC_DriverHP_OLED(
   .sys_clk				(sys_clk),           /*系统时钟*/
   .rst_n				(rst_n),             /*系统复位*/

    .IICSCL				(OLED_SCL),            /*IIC 时钟输出*/
    .IICSDA				(OLED_SDA),             /*IIC 数据线*/


    .IICSlave			({IICWriteData[15:8],IICWriteData[23:16]}),           /*从机 8bit的寄存器地址 + 8bit的从机地址*/

    .IICWriteReq		(IICWriteReq),       /*IIC写寄存器请求*/
    .IICWriteDone		(IICWriteDone),      /*IIC写寄存器完成*/
    .IICWriteData		(IICWriteData[7:0]),       /*IIC发送数据  8bit的数据*/

    .IICReadReq			(1'b0),        /*IIC读寄存器请求*/
    .IICReadDone			(),       /*IIC读寄存器完成*/
    .IICReadData    		()    /*IIC读取数据*/
);



endmodule 