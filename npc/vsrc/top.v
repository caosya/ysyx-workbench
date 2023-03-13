module top(
    input clk,
    input rst_n,
    output reg [63:0] pc,
    input reg [31:0] instr
);
IFU ifu_inst(
    .clk(clk),
    .rst_n(rst_n),
    .pc(pc),
    .instr(instr)
);

import "DPI-C" function void break_exec(int code);

always @(*) begin
    if(instr == 32'h00100073) break_exec(0);
end


endmodule
