module IFU (
    input clk,
    input rst_n,
    output reg [63:0] pc,
    input [31:0]instr
);

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        pc = 64'h80000000;
        $display("-------复位-------");
    end else begin
        pc = pc + 4;
        $display("-------instr= %x----", instr);
    end
end





endmodule