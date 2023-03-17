module top(
    input clk,
    input rst_n,
    output rd_wr,
    output reg [63:0] pc,
    output reg [31:0] instr
);


assign rd_wr = MemRead | MemWrite;
//控制信号
wire [6:0]opcode;
wire [2:0]func3;
wire [4:0]Rs1;
wire [4:0]Rs2;
wire [4:0]Rd;
wire [63:0]imme;

wire TYPE_U;
wire TYPE_I;
wire TYPE_S;
wire TYPE_B;
wire TYPE_R;

wire jal;
wire jalr;
wire auipc;

wire ebreak;
wire ecall;
wire mret;

wire load;
wire MemRead;
wire MemWrite;

wire RegWrite;
wire state_reg_op;
wire [3:0]ALUctrl;  

wire word; //字操作
wire un_sign; //无符号

wire beq;
wire bne;
wire bge;
wire blt;
wire bltu;
wire bgeu;



//alu
wire [63:0]alu_out;
wire [63:0]src1;
wire [63:0]src2;

//reg
wire shift ;//pc是否变换
assign shift = 1'b1; //先暂时单周期

wire [63:0]Rs1_data;
wire [63:0]Rs2_data;
wire [63:0]mepc; //保存中断前pc
wire [63:0]mtvec; //保存中断后程序pc

//mem
wire [63:0]mem_rd_data;


//下一个PC

wire ordinary_jump;
wire special_jump;

assign ordinary_jump = jal | ((alu_out==0) & (beq | bge | bgeu)) | ((alu_out!=0) & (bne | blt | bltu));
assign special_jump =  jalr | ecall | mret ;

wire [63:0] pc_special;
assign pc_special = jalr ? (alu_out) & (~1) :
                    ecall ? mtvec : 
                    mret ? mepc : 0; 

wire [63:0] pc_next;
assign pc_next = special_jump ? pc_special :
                 ordinary_jump ? pc + imme: pc + 4;

IFU ifu_inst(
    .clk(clk),
    .rst_n(rst_n),
    .pc_next(pc_next),
    .pc(pc),
    .instr(instr)
);

IDU idu_inst(
    .instr(instr),
    .opcode(opcode),
    .func3(func3),
    .Rs1(Rs1),
    .Rs2(Rs2),
    .Rd(Rd),
    .imme(imme),
    .TYPE_S(TYPE_S),
    .TYPE_B(TYPE_B),
    .TYPE_R(TYPE_R),
    .TYPE_U(TYPE_U),
    .TYPE_I(TYPE_I),
    .jal(jal),
    .ecall(ecall),
    .mret(mret),
    .ebreak(ebreak),
    .load(load),
    .MemRead(MemRead),
    .MemWrite(MemWrite),
    .RegWrite(RegWrite),
    .state_reg_op(state_reg_op),
    .word(word),
    .un_sign(un_sign),
    .ALUctrl(ALUctrl),
    .auipc(auipc),
    .jalr(jalr),
    .beq(beq),
    .bne(bne),
    .bge(bge),
    .blt(blt),
    .bltu(bltu),
    .bgeu(bgeu)
);

ALU alu_inst(
    .clk(clk),
    .instr_25(instr[25]),
    .ALUctrl(ALUctrl),  
    .word(word),
    .un_sign(un_sign),
    .imme(imme),
    .Rs1_data(Rs1_data),
    .Rs2_data(Rs2_data),
    .TYPE_S(TYPE_S),
    .TYPE_B(TYPE_B),
    .TYPE_R(TYPE_R),
    .jal(jal),
    .TYPE_I(TYPE_I),
    .TYPE_U(TYPE_U),
    .alu_out(alu_out),
    .src1(src1),
    .src2(src2)
);

Reg reg_inst(
    .clk(clk),
    .W_en(RegWrite),
    .MemtoReg(MemRead),
    .mem_rd_data(mem_rd_data),
    .auipc(auipc),
    .jal(jal),
    .jalr(jalr),
    .pc(pc),
    .shift(shift),
    .alu_out(alu_out),
    .state_reg_op(state_reg_op),
    .Rs1(Rs1),
    .Rs2(Rs2),
    .Rd(Rd),
    .Rs1_data(Rs1_data),
    .Rs2_data(Rs2_data),
    .mepc(mepc),
    .mtvec(mtvec),
    .src1(src1),
    .src2(src2),
    .func3(func3),
    .ecall(ecall),
    .mret(mret),
    .ebreak(ebreak)
);

Mem my_mem(
    .clk(clk),
    .MemRead(MemRead),
    .MemWrite(MemWrite),
    .func3(func3),
    .w_data(src2),
    .addr(alu_out),
    .r_data(mem_rd_data)
);



endmodule
