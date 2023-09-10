//OLED����ģ��
module OLED_Top(

	input			sys_clk,
	input			rst_n,

	//DHT11��ֵ��ʾ
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



localparam	OLED_INIT 		=  'd0;			//��ʼ��
localparam	OLED_Refresh	=  'd1;			//ˢ�£���oledȫ��д0
localparam  OLED_ShowFont	=	'd2;			//��ʾ�ַ�
localparam	OLED_IDLE 		=  'd3;			//����
localparam	OLED_ShowData	=	'd4;			//��ʾ����




reg[4:0]	state , next_state;

//IIC����ź�
wire			IICWriteReq;
wire[23:0]  IICWriteData;
wire			IICWriteDone;

//��ʼ������ź�
wire			init_finish;
wire[23:0]  Init_data;
wire 			init_req;

//refresh����ź�
wire			refresh_finish;
wire[23:0]	refresh_data;
wire		   refresh_req;


//�ַ���ʾ����ź�
wire			showfont_finish;
wire[23:0]	showfont_data;
wire			showfont_req;


//��ʾ��������ź�
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
	
	.init_req			(init_req),				//��ʼ������
	.write_done			(IICWriteDone),			//һ���ʼ����������ź�
	
	.init_finish		(init_finish),			//��ʼ��������

	.Init_data			(Init_data)//��ʼ��������
);


OLED_Refresh(
	.sys_clk				(sys_clk),
	.rst_n				(rst_n),
	
	.refresh_req		(refresh_req),				//��ʼ������
	.write_done			(IICWriteDone),				//һ���ʼ����������ź�
	
	.refresh_finish	(refresh_finish),			//��ʼ��������

	.refresh_data		(refresh_data)	//��ʼ��������
);


OLED_ShowFont OLED_ShowFont_HP(

	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	
	.ShowFont_req	(showfont_req),		    //�ַ���ʾ����
	.write_done		(IICWriteDone),			 //iicһ������д���
	.State          (State),
	.ShowFont_Data		(showfont_data),		 //�ַ���ʾ����
	
	.ShowFont_finish(showfont_finish)       //�ַ���ʾ���
);

OLED_ShowData OLED_ShowDataHP(
	
	.sys_clk			(sys_clk),
	.rst_n			(rst_n),
	
	.dht11_done		(dht11_done),
	.tempH			(tempH),			//�¶���������
	.tempL			(tempL),			//�¶�����С��
	.humidityH		(humidityH),		//�¶���������
	.humidityL		(humidityL),		//�¶�����С��
	
	
	.ShowData_req	(showdata_req),		 //�ַ���ʾ����
	.write_done		(IICWriteDone),			 //iicһ������д���
	
	.ShowData_Data	(showdata_data),		 //�ַ���ʾ����
	
	.ShowData_finish	(showdata_finish)   //�ַ���ʾ���

);


//����ѡ��
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
   .sys_clk				(sys_clk),           /*ϵͳʱ��*/
   .rst_n				(rst_n),             /*ϵͳ��λ*/

    .IICSCL				(OLED_SCL),            /*IIC ʱ�����*/
    .IICSDA				(OLED_SDA),             /*IIC ������*/


    .IICSlave			({IICWriteData[15:8],IICWriteData[23:16]}),           /*�ӻ� 8bit�ļĴ�����ַ + 8bit�Ĵӻ���ַ*/

    .IICWriteReq		(IICWriteReq),       /*IICд�Ĵ�������*/
    .IICWriteDone		(IICWriteDone),      /*IICд�Ĵ������*/
    .IICWriteData		(IICWriteData[7:0]),       /*IIC��������  8bit������*/

    .IICReadReq			(1'b0),        /*IIC���Ĵ�������*/
    .IICReadDone			(),       /*IIC���Ĵ������*/
    .IICReadData    		()    /*IIC��ȡ����*/
);



endmodule 