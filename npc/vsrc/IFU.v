module IFU (
    input clk,
    input rst_n,
    input [63:0] pc_next,
    output reg [63:0] pc,
    output [31:0]instr
);
reg [63:0] rd_data;
import "DPI-C" function void pmem_read(input longint raddr, output longint rdata);


always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        pc <= 64'h80000000;
        $display("-------复位-------");
    end else begin
        pc <= pc_next;
        // $display("-------instr= %x----", instr);       
    end
end

always @(*) begin
    if(!rst_n) 
        rd_data = 0;
    else
        pmem_read(pc, rd_data);
end


assign instr = rd_data[31:0];
wire _unused_ok = &{rd_data[63:32]};






endmodule