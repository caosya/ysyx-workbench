`include "vsrc/define.v"	
module IDU(
    input [31:0] instr,

    output [6:0]opcode,
	output [2:0]func3,
	output [4:0]Rs1,
	output [4:0]Rs2,
	output [4:0]Rd,
	output [63:0]imme,
    output TYPE_I,
    output TYPE_R,
    output TYPE_B,
    output TYPE_S,
    output TYPE_U,
    output jal,
    output load,
    output ecall,
    output mret,
    output ebreak,
    output word,
	output un_sign,
    output [3:0]ALUctrl,
    output MemRead,
	output MemWrite,
	output RegWrite,
	output state_reg_op,
	output beq,
	output bne,
	output bge,
	output blt,
	output bltu,
	output bgeu,
    output auipc,
	output jalr

);


wire [63:0]immI;
wire [63:0]immU;
wire [63:0]immS;
wire [63:0]immB;
wire [63:0]immJ;


assign  opcode  = instr[6:0];
assign  Rs1 = instr[19:15];
assign  Rs2 = instr[24:20];
assign  Rd  = instr[11:7];
assign  func3  = instr[14:12];

assign immI = {{52{instr[31]}}, instr[31:20]};
assign immU = {{32{instr[31]}}, instr[31:12], 12'b0};
assign immS = {{52{instr[31]}}, instr[31:25], instr[11:7]};
assign immB = {{52{instr[31]}}, instr[7], instr[30:25], instr[11:8], 1'b0};
assign immJ = {{44{instr[31]}}, instr[19:12], instr[20], instr[30:21], 1'b0};

assign TYPE_I = (opcode==`I_type1) | (opcode==`I_type2) | (opcode==`I_type3) | (opcode==`load) | (opcode==`jalr);
assign TYPE_U = (opcode==`lui) | (opcode==`auipc);
assign TYPE_S = (opcode==`store);
assign jal = (opcode==`jal);
assign TYPE_B = (opcode==`B_type);
assign TYPE_R = (opcode==`R_type1) | (opcode==`R_type2);

assign imme = TYPE_I ? immI:
              TYPE_B ? immB:
              TYPE_S ? immS:
              TYPE_U ? immU:
              jal ? immJ: 64'b0;
assign ebreak = (instr==`ebreak);
assign ecall = (instr==`ecall);
assign mret = (instr==`mret);
assign load = (opcode == `load);
assign auipc = (opcode == `auipc) ? 1'b1 : 1'b0;
assign jalr = (opcode == `jalr) ? 1'b1 : 1'b0;


assign MemRead = (opcode == `load) ? 1'b1 : 1'b0;   
assign MemWrite = TYPE_S; 

assign RegWrite = TYPE_U | TYPE_I | TYPE_R | jal;    //除了B、S类型其他都要用到寄存器写
assign state_reg_op = (opcode == `I_type3) ? 1'b1 : 1'b0;

wire [3:0]branchop;
reg  [3:0]RIop;

always@(*)
	begin
		case(func3)
			3'b000: RIop = (TYPE_R & instr[30]) ? `SUB:
						   (TYPE_R & instr[25]) ? `MUL :`ADD;
			3'b001: RIop =`SLL;
			3'b010: RIop =`SLT;
			3'b011: RIop =`SLTU;
			3'b100: RIop = (TYPE_R & instr[25]) ? `DIV :`XOR ;
			3'b101: RIop = (TYPE_R & instr[25]) ? `DIV : 
							instr[30] ? `SRA : `SRL;
			3'b110: RIop = (TYPE_R & instr[25]) ? `REM :`OR;
			3'b111: RIop = (TYPE_R & instr[25]) ? `REM :`AND;
			default:RIop =`ADD;
		endcase
	end
assign branchop=(func3[2] & func3[1])? `SLTU : (func3[2] ^ func3[1])? `SLT : `SUB;

assign ALUctrl = (TYPE_R | (TYPE_I & (!MemRead) & (!jal))) ? RIop:  //I或者R类型,I类型除了jal,load
				(TYPE_B) ? branchop : `ADD;	


assign word = opcode[3];
assign un_sign = func3[0];

assign beq = TYPE_B & (func3 == 3'b000);
assign bne = TYPE_B & (func3 == 3'b001);
assign bge = TYPE_B & (func3 == 3'b101);
assign blt = TYPE_B & (func3 == 3'b100);
assign bltu = TYPE_B & (func3 == 3'b110);
assign bgeu = TYPE_B & (func3 == 3'b111);



endmodule

