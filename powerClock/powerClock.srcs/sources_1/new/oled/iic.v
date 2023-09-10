`timescale 1ns/1ps




/*
    IIC 主通信 : 8bit的数据 + 8bit的寄存器地址 + 8bit的从机地址
    作者：  FPGA之旅
    欢迎关注微信公众号 FPGA之旅
*/

module  IIC_Driver(
    input       sys_clk,           /*系统时钟*/
    input       rst_n,             /*系统复位*/

    output      IICSCL,            /*IIC 时钟输出*/
    inout       IICSDA,             /*IIC 数据线*/


    input[15:0] IICSlave,           /*从机 8bit的寄存器地址 + 8bit的从机地址*/

    input       IICWriteReq,       /*IIC写寄存器请求*/
    output      IICWriteDone,      /*IIC写寄存器完成*/
    input[7:0]  IICWriteData,       /*IIC发送数据  8bit的数据*/

    input       IICReadReq,        /*IIC读寄存器请求*/
    output      IICReadDone,       /*IIC读寄存器完成*/
    output[7:0] IICReadData        /*IIC读取数据*/
);


/*IIC 状态*/
localparam IIC_IDLE       =   6'b000_001;  /*空闲态*/
localparam IIC_START      =   6'b000_010;  /*起始态*/
localparam IIC_WRDATA     =   6'b000_100;  /*写数据态*/
localparam IIC_RDDATA     =   6'b001_000;  /*读数据态*/
localparam IIC_ACK        =   6'b010_000;  /*应答态*/
localparam IIC_STOP       =   6'b100_000;  /*停止态*/



localparam  IIC_Pre       =   'd100;     /*iiC分频*/
    
reg[5:0]   state , next_state;
reg[21:0]  IICCnt;              /*IIC计数器*/
reg[3:0]   IICBitCnt;           /*IIC数据发送个数计数*/
reg[1:0]   IICACKStopCnt;       /*IIC ack stop应答计数*/
reg[2:0]   IICSendBytes;        /*IIC 发送字节计数*/ 


reg[15:0]   IICSlaveReg;           /*从机地址+寄存器数据*/
reg[7:0]   IICReadDataReg;         /*读取到的数据*/


reg 	IICWriteReqReg;

reg   iictx;     /*iic发送数据引脚*/
reg   iicCLK;    /*iic时钟信号引脚*/

assign IICSDA = (state == IIC_RDDATA || (state == IIC_ACK)) ? 1'bz : iictx;  /*iic为读数据或者应答的时候，输出为高阻态*/
assign IICSCL =  iicCLK;

assign IICReadData = IICReadDataReg;

assign IICReadDone  = (state != next_state && state == IIC_STOP) ? 1'b1 : 1'b0;
assign IICWriteDone = (state != next_state && state == IIC_STOP) ? 1'b1 : 1'b0;



always@(posedge sys_clk or negedge rst_n)
begin
	if(rst_n == 1'b0)
		IICWriteReqReg <= 1'b0;
	else if(IICWriteDone == 1'b1)
		IICWriteReqReg <= 1'b0;
	else if(IICWriteReq == 1'b1)
	   IICWriteReqReg <= 1'b1;
	else
		IICWriteReqReg <= IICWriteReqReg;

end

always @(posedge sys_clk or negedge rst_n) 
begin
    if(rst_n == 1'b0)
        state <= IIC_IDLE;
    else
        state <= next_state;   
end

/*状态机*/
always @(*)
begin
    case(state)
    IIC_IDLE: 
        if(IICWriteReq == 1'b1 || IICReadReq == 1'b1)
            next_state <= IIC_START;
        else
            next_state <= IIC_IDLE;
    IIC_START:
        if(IICCnt == (IIC_Pre * 'd2))
            next_state <= IIC_WRDATA;
        else
            next_state <= IIC_START;
    IIC_WRDATA:
        if(IICBitCnt == 'd8 /*&& IICCnt == IIC_Pre /4 */&& iicCLK == 1'b0)
            next_state <= IIC_ACK;
        else
            next_state <= IIC_WRDATA;
    IIC_RDDATA:
        if(IICBitCnt == 'd8 && IICCnt == IIC_Pre /4 && iicCLK == 1'b0)
            next_state <= IIC_ACK;
        else
            next_state <= IIC_RDDATA;
    IIC_ACK:
        if(IICACKStopCnt == 'd1 /*&& IICCnt == IIC_Pre /4 */&& iicCLK == 1'b0)
            if(IICSendBytes == 'd2) 
                if(/*IICWriteReq*/IICWriteReqReg == 1'b1)             /*三个字节发送完成，进入停止态*/
                    next_state <= IIC_STOP;
                else 
                    next_state <= IIC_RDDATA;
            else if(IICSendBytes == 'd2 && IICReadReq == 1'b1)
                next_state <= IIC_START;
            else if(IICSendBytes == 'd4)
                next_state <= IIC_STOP;
            else
                next_state <= IIC_WRDATA;
        else
            next_state <= IIC_ACK;
    IIC_STOP:
        if(IICACKStopCnt == 'd1 && IICCnt == IIC_Pre/4 && iicCLK == 1'b1)
            next_state <= IIC_IDLE;
        else
            next_state <= IIC_STOP;
    default:  next_state <= IIC_IDLE;
    endcase
end



/*IIC 发送字节计数*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICSendBytes <= 'd0;
    else if(state == IIC_IDLE)
        IICSendBytes <= 'd0;
    else if(state == IIC_ACK)
        if(next_state != state)
            IICSendBytes <= IICSendBytes + 1'b1;
        else
            IICSendBytes <= IICSendBytes;
    else
        IICSendBytes <= IICSendBytes;

end

/*IIC分频计数*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICCnt <= 'd0;
    else if(IICCnt == IIC_Pre && state != IIC_START)
        IICCnt <= 'd0;
    else if(IICCnt == (IIC_Pre *'d2) && state != IIC_START)
        IICCnt <= 'd0;
    else if(state != next_state)
        IICCnt <= 'd0;
    else if(state == IIC_START)
        IICCnt <= IICCnt + 1'b1;
    else if(state == IIC_WRDATA )
        IICCnt <= IICCnt + 1'b1;
    else if(state == IIC_RDDATA)
        IICCnt <= IICCnt + 1'b1;
    else if(state == IIC_ACK)
        IICCnt <= IICCnt + 1'b1;
    else if(state == IIC_STOP)
        IICCnt <= IICCnt + 1'b1;
end

/*IIC发送bit计数*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICBitCnt <= 'd0;
    else if(state == IIC_IDLE || state == IIC_ACK)
        IICBitCnt <= 'd0;
    else if(state == IIC_WRDATA && IICCnt == (IIC_Pre /2))
        if(iicCLK == 1'b1)
            IICBitCnt <= IICBitCnt + 1'b1;
        else
            IICBitCnt <= IICBitCnt;
    else if(state == IIC_RDDATA && IICCnt == (IIC_Pre /2))
        if(iicCLK == 1'b1)
            IICBitCnt <= IICBitCnt + 1'b1;
        else
            IICBitCnt <= IICBitCnt;
    else
        IICBitCnt <= IICBitCnt;
end


/*IIC ack stop应答计数*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICACKStopCnt <= 'd0;
    else if(state != next_state)
        IICACKStopCnt <= 'd0;
    else if((state == IIC_ACK || state == IIC_STOP) && IICCnt == (IIC_Pre /2))
        if(iicCLK == 1'b1)
            IICACKStopCnt <= IICACKStopCnt + 1'b1;
        else
            IICACKStopCnt <= IICACKStopCnt;
    else if(state == IIC_ACK || state == IIC_STOP)
        IICACKStopCnt <= IICACKStopCnt;
    else
        IICACKStopCnt <= 'd0;
end

/*IIC从机信息控制*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICSlaveReg <= 'd0;
    else if((IICWriteReq == 1'b1 || IICReadReq == 1'b1) && state == IIC_IDLE)  /*请求来时，保存信息*/
        IICSlaveReg <= IICSlave;
    else if(state == IIC_ACK && state != next_state)   /*每发送完成一字节，就调换数据，始终发送的是低8位*/
        if(IICSendBytes == 'd2)
            IICSlaveReg <= {IICSlaveReg[7:0],IICSlaveReg[15:8]} + 1'b1;
        else
            IICSlaveReg <= {IICSlaveReg[7:0],IICSlaveReg[15:8]};
    else
        IICSlaveReg <= IICSlaveReg;
end


/*IIC 时钟控制*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        iicCLK <= 1'b1;
    else if(state == IIC_START && IICCnt == (IIC_Pre*2))      /*开始结束时，拉低时钟线*/
        iicCLK <= 1'b0;
    else if(state == IIC_START && IICCnt > IIC_Pre)
        iicCLK <= 1'b1;
    else if(state == IIC_START && IICCnt == IIC_Pre)
        iicCLK <= iicCLK;
    else if(state == IIC_WRDATA && IICCnt == IIC_Pre)     /*发送数据，依次取反时钟线*/
        iicCLK <= ~iicCLK;
    else if(state == IIC_RDDATA && IICCnt == IIC_Pre)     /*接收数据，依次取反时钟线*/
        iicCLK <= ~iicCLK;
    else if(state == IIC_ACK && IICCnt == IIC_Pre)
        iicCLK <= ~iicCLK;
    else if(state == IIC_STOP && IICCnt == IIC_Pre)  /*在停止态时，IICCnt时，直接拉高iicCLK*/
        iicCLK <= 1'b1;
end


/*iic 发送控制*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)   
        iictx <= 1'b1;
    else if(state == IIC_START && IICCnt == (IIC_Pre/2))      /*开始，iic数据线拉低*/
        iictx <= 1'b0;
    else if(state == IIC_START && IICCnt == IIC_Pre/4)      /*开始，iic数据线拉低*/
        iictx <= 1'b1;
    else if(state == IIC_WRDATA && IICCnt == IIC_Pre / 2)     /*iic发送数据*/
        if(iicCLK == 1'b0 && IICSendBytes == 'd2 && (IICWriteReq == 1'b1 || IICWriteReqReg == 1'b1))
            iictx <= IICWriteData['d7-IICBitCnt];
        else if(iicCLK == 1'b0)
            iictx <= IICSlaveReg['d7-IICBitCnt];
        else
            iictx <= iictx;
    else if(state == IIC_ACK)
        iictx <= 1'b0;

    else if(state == IIC_STOP && IICCnt == (IIC_Pre))
        if(iicCLK == 1'b1)
            iictx <= 1'b1;
        else
            iictx <= iictx;
    else if(state == IIC_IDLE)
        iictx <= 1'b1;
end


/*iic 读取数据控制*/
always@(posedge sys_clk or negedge rst_n)
begin
    if(rst_n == 1'b0)
        IICReadDataReg <= 'd0;
    else if(state == IIC_RDDATA && IICCnt == IIC_Pre / 2)
        if(iicCLK == 1'b1)
            IICReadDataReg <= {IICReadDataReg[6:0],IICSDA};
        else
            IICReadDataReg <= IICReadDataReg;
    else
        IICReadDataReg <= IICReadDataReg;
end


endmodule

