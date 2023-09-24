register pP {  
    # our own internal register. P_pc is its output, p_pc is its input.
	pc:64 = 0; # 64-bits wide; 0 is its default value.
} 

register cC {
	SF:1 = 0;
     	ZF:1 = 1;
};

# "pc" is a pre-defined input to the instruction memory and is the 
# address to fetch 10 bytes from (into pre-defined output "i10bytes").
pc = P_pc;

# we can define our own input/output "wires" of any number of 0<bits<=80
wire opcode:8, icode:4, ifun:4;
wire rA: 4, rB : 4;
wire valC: 64;
wire valE: 64;
wire valP: 64;
wire conditionsMet: 1;


opcode = i10bytes[0..8];   # first byte read from instruction memory
icode = opcode[4..8];      # top nibble of that byte
ifun = opcode[0..4];
rA = i10bytes[12..16];
rB = i10bytes[8..12];
valC = [
	icode == JXX || icode == CALL : i10bytes[8..72];
	1 : i10bytes[16..80];
];


reg_srcA = rA;
reg_srcB = [
	icode == PUSHQ || icode == POPQ || icode == CALL || icode == RET : 4;
	1 : rB;
];
reg_dstE = [
	!conditionsMet && icode == CMOVXX : REG_NONE;	
	icode == IRMOVQ || icode == OPQ || icode == CMOVXX : rB;
	icode == MRMOVQ	: rA;
	icode == PUSHQ || icode == POPQ || icode == CALL || icode == RET : 4;
	1 : REG_NONE;
];
reg_inputE = [
	icode == IRMOVQ : valC;
	icode == CMOVXX : reg_outputA;
	icode == OPQ || icode == PUSHQ || icode == POPQ || icode == CALL || icode == RET : valE;
	icode == MRMOVQ : mem_output;
	1 : 0;
];
reg_dstM = [
	icode == POPQ : rA;
	1 : REG_NONE;
];
reg_inputM = [
	icode == POPQ : mem_output;
	1 : 0;
];

valE = [
	icode == OPQ && ifun == 0 : reg_outputA + reg_outputB;
	icode == OPQ && ifun == 1 : reg_outputB - reg_outputA;
	icode == OPQ && ifun == 2 : reg_outputA & reg_outputB;
	icode == OPQ && ifun == 3 : reg_outputA ^ reg_outputB;
	(icode == RMMOVQ || icode == MRMOVQ) : reg_outputB + valC;
	icode == PUSHQ || icode == CALL : -8 + reg_outputB;
	icode == POPQ || icode == RET : 8 + reg_outputB;
	1 : 0;
];

valP = [
	icode == CALL : P_pc + 9;
	1 : 0;
];

mem_input = [
	icode == CALL : valP;
	1 : reg_outputA;
];
mem_addr = [
	icode == POPQ || icode == RET : reg_outputB;
	1 : valE;
];
mem_readbit = [
	icode == MRMOVQ || icode == POPQ || icode == RET : 1;
	1 : 0;
];
mem_writebit = [
	icode == RMMOVQ || icode == PUSHQ || icode == CALL: 1;
	1 : 0;
];

c_ZF = [
     icode == OPQ : (valE == 0);
     1 : C_ZF;
];

c_SF = [
     icode == OPQ : (valE >= 0x8000000000000000);
     1 : C_SF;
];

conditionsMet = [
	ifun == 0 : 1;
	ifun == 1 : C_SF || C_ZF;
	ifun == 2 : C_SF;
	ifun == 3 : C_ZF;
	ifun == 4 : !C_ZF;
	ifun == 5 : !C_SF;
	ifun == 6 : !C_SF && !C_ZF;
	1 : 0;
];

# Stat is a built-in output; STAT_HLT means "stop", STAT_AOK means 
# "continue".  The following uses the mux syntax described in the 
# textbook
Stat = [
	icode == HALT : STAT_HLT;
	icode > 0xb : STAT_INS;
	1             : STAT_AOK;
];


# to make progress, we have to update the PC...
p_pc = [
	(icode == HALT || icode == NOP) : P_pc + 1; 
	(icode == 2 || icode == OPQ || icode == 10 || icode == 11) : P_pc + 2;
	(icode == 3 || icode == 4 || icode == 5) : P_pc + 10;
	(icode == JXX && conditionsMet) : valC;
	icode == CALL : valC;
	icode == RET : mem_output;
	1 : P_pc + 9;
];
