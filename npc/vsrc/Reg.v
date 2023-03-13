`include "vsrc/define.v"	
module Reg(
	input clk,
	input W_en,
	input MemtoReg,
	input [63:0]mem_rd_data,
	input auipc,
	input jal,
	input jalr,
	input [63:0]pc,
	input shift,
	input [63:0]alu_out,
	input state_reg_op,//状态寄存器操作？
	input [4:0]Rs1, //源寄存器1
	input [4:0]Rs2, //源寄存器2
	input [4:0]Rd,  //目标寄存器
	output [63:0]Rs1_data,  //源寄存器1的数据 
	output [63:0]Rs2_data,   //源寄存器2的数据
	output [63:0]mepc,
	output [63:0]mtvec,
	input [63:0]src1,
	input [63:0]src2,
	input [2:0]func3,
	input ecall,
	input mret,
	input ebreak
);	

	reg [63:0] regs[31:0];//通用寄存器

	reg [63:0] state_regs[3:0]; //分别是mepc,mtvec,mcause,mstatus
	wire [1:0] csr_id;
	wire [63:0] csr_tmp;

	assign csr_id = src2 == 64'h341 ? 2'b00:
				 src2 == 64'h305 ? 2'b01:
				 src2 == 64'h342 ? 2'b10:
				 src2 == 64'h342 ? 2'b11:0;

	assign csr_tmp = state_regs[csr_id];

	reg [63:0] data;
	
	assign data = auipc ? alu_out + pc : 
				  jal | jalr  ? pc + 4 : 
				  MemtoReg ? mem_rd_data : alu_out; 

	assign mepc = state_regs[0];
	assign mtvec = state_regs[1];

	wire csr_op;
	assign csr_op = state_reg_op & (!ecall) & (!mret) & (!ebreak);
	
	// Register #(64, 5) r0 (clk, );
///write
	always@( posedge clk )
		begin
			if(shift) begin
				if(csr_op ) begin
					// $display("---------pc = %x------------",pc);
					// $display("---------Rd = %x------------",Rd);
					state_regs[csr_id] = func3 == 3'b010 ? (csr_tmp | src1) : src1;  //Todo 
					regs[Rd] = csr_tmp;
				end
				else if (ecall) begin 
					// $display("---------pc = %x------------",pc);
					// $display("---------r17 = %x------------",regs[17]);	
					// $display("---------mtvec = %x------------",mtvec);	
					state_regs[0] = pc;
					state_regs[2] = regs[17];
				end
				else if((W_en && Rd!=0))	begin
	 				regs[Rd] <= data;
					// if(Rd == 5'd2)
					// $display("alu_out: %x",alu_out);
					// $display("pc: %x",pc);
					// $display("wr_data: %x",data);
					// $display("wr_Rd: %x",Rd);
					// $display("");
					// $display("");
				end
			end else 
				// $display("====wr_data: %x",data);
			begin end;
		end
		
//read
	assign Rs1_data=(Rs1==5'b0)? `zero_word : regs[Rs1];
	assign Rs2_data=(Rs2==5'b0)? `zero_word : regs[Rs2];

import "DPI-C" function void break_exec(int code);

always @(*) begin
    if(ebreak) break_exec(regs[10][31:0]);
end

import "DPI-C" function void set_gpr_ptr(input [63:0] a []);
initial set_gpr_ptr(regs);  // rf为通用寄存器的二维数组变量

	
endmodule


