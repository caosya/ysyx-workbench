module Mem(
    input MemRead,
    input MemWrite,
    input [2:0]func3,
    input [63:0]w_data,
    input [63:0]addr,
    output [63:0]r_data
);

wire [63:0]rdata;

//掩码
wire [7:0]bytes;
assign bytes =  MemWrite ? (func3[1:0] == 2'b00 ? 8'b00000001 :
						                func3[1:0] == 2'b01 ? 8'b00000011 :
						                func3[1:0] == 2'b10 ? 8'b00001111 :
						                func3[1:0] == 2'b11 ? 8'b11111111 : 0 ) : 0;




import "DPI-C" function void pmem_read(input longint raddr, output longint rdata);
import "DPI-C" function void pmem_write(input longint waddr, input longint wdata, input byte wmask);

always @(*) begin
    // $display("=======read_addr=%x=====",addr);
    // $display("=======bytes=%x=====",bytes);
    // if(MemWrite)
    
    // if(MemRead)
        pmem_read(addr, rdata);
    // else if(MemWrite)
        pmem_write(addr, w_data, bytes);
    // else
    //     $display("没有内存读写操作");    
end
assign r_data = func3[1:0] == 2'b11 ? rdata :
                func3[1:0] == 2'b10 ? (func3[2] ? {{32{1'b0}},rdata[31:0]} : {{32{rdata[31]}},rdata[31:0]}) :
                func3[1:0] == 2'b01 ? (func3[2] ? {{48{1'b0}},rdata[15:0]} : {{48{rdata[15]}},rdata[15:0]}) : 
                                 (func3[2] ? {{56{1'b0}},rdata[7:0]} : {{56{rdata[7]}},rdata[7:0]});


endmodule


