`include "vsrc/define.v"	
module ALU(
    input instr_25,
    input [3:0]ALUctrl,
    input word,
    input un_sign,
    input [63:0]imme,
    input [63:0]Rs1_data,
    input [63:0]Rs2_data,
    input TYPE_S, 
    input jal,      //J
    input TYPE_I,
    input TYPE_U,
    input TYPE_R,
    input TYPE_B,
    input clk,
    output [63:0]alu_out,
    output [63:0]src1,
    output [63:0]src2
);
//处理数据

reg [63:0]tmp;

assign src1 = (TYPE_I | TYPE_S | TYPE_B | TYPE_R ) ? Rs1_data:  //I、S、B、R
              (TYPE_U | jal) ? imme : 64'b0;


assign src2 =   (TYPE_I ) ? imme : 
                (TYPE_S | TYPE_B | TYPE_R ) ? Rs2_data : 64'b0; // S、B、R

// assign csr_id = src2 == 64'h341 ? 2'b00:
// 			    src2 == 64'h305 ? 2'b01:
// 			    src2 == 64'h342 ? 2'b10:
// 			    src2 == 64'h342 ? 2'b11:0;


reg [63:0] res;

wire [63:0]contrary;
assign contrary = ALUctrl == `SLT ? ((src2 ^ `UMAX )+1) : (src2 ^ `UMAX); //补码

wire c; //借位
wire [63:0]sub_result;

assign {c,sub_result} = ALUctrl == `SLT ? src1 + contrary : src1 + contrary +1;

always @(*) begin
    case (ALUctrl)
        `ADD : tmp = src1 + src2;
        `SUB : tmp = sub_result;
        `MUL : begin 
            if(word) tmp[31:0] = src1[31:0] * src2[31:0];
            else    tmp = src1 * src2;
        end
        `DIV : begin
            if(word) tmp[31:0] =  un_sign ? src1[31:0] / src2[31:0] : $signed(src1[31:0]) / $signed(src2[31:0]);
            else    tmp = un_sign ?  src1 / src2 : $signed(src1) / $signed(src2) ;
        end
        `AND : tmp = src1 & src2;
        `OR  : tmp = src1 | src2;
        `XOR : tmp = src1 ^ src2;
        `REM : begin
            if(word) tmp[31:0] =  un_sign ? src1[31:0] % src2[31:0] : $signed(src1[31:0]) % $signed(src2[31:0]);
            else    tmp = un_sign ?  src1 % src2 : $signed(src1) % $signed(src2) ;
        end
        `SLTU : begin
            //无符号比较大小：俩者相减，最高位无进位则小于,此时c = 0;
            tmp = {{63{1'b0}},!c};
        end
        `SLT : begin 
            //有符号比较大小：小于时符号为1,
            tmp = {{63{1'b0}},sub_result[63]};
        end
        `SRA : begin
            if (word)
                tmp[31:0] = (TYPE_I & instr_25) ? 32'b0 : {32{src1[31]}} << (32-src2[4:0]) | src1[31:0] >> src2[4:0];
            else
                tmp = {64{src1[63]}} << (64-src2[5:0]) | src1 >> src2[5:0];
        end
        `SRL : begin
            if (word)
                tmp[31:0] = (TYPE_I & instr_25) ?  32'b0 : src1[31:0] >> src2[4:0];
            else 
                tmp = src1 >> src2[5:0];
        end
        `SLL : begin
            if (word)
                tmp[31:0] = (TYPE_I & instr_25) ?  32'b0 : src1[31:0] << src2[4:0];
            else
                tmp = src1 << src2[5:0];
        end
        default:tmp = src1 + src2;
    endcase
    res = word ? {{32{tmp[31]}}, tmp[31:0]} : tmp[63:0];
    // $display("Rs1Data: %x",Rs1_data);
    // $display("src1: %x",src1);
    // $display("src2: %x",src2);
    // $display("imme: %x",imme);
    // $display("tmp: %x",tmp);
    // $display("c: %x",c);
    // $display("src1[63]: %d",src1[63]);
    // $display("contrary: %x",contrary);
    // $display("contrary[63]: %d",contrary[63]);
    // $display("sub_result[63]: %d",sub_result[63]);
    // $display("alu_out1: %x",{{32{tmp[31]}}, tmp[31:0]});
    // alu_out_tmp = 
end
assign alu_out = (TYPE_U | jal)  ?  src1 :
              TYPE_S ? src1 + imme : 
              res;
// always @(posedge clk)
//     alu_out_old <= alu_out;

// always @(*) 
//     $display("alu_out: %x",alu_out);
endmodule




