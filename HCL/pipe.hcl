########## the PC and condition codes registers #############
register fF { 
	predPC:64 = 0; 
}

register cC {
	SF:1 = 0;
     	ZF:1 = 1;
};

########## Fetch #############
register fD {
	icode : 4 = NOP;
	ifun : 4 = 0;
	rA : 4 = REG_NONE;
	rB : 4 = REG_NONE;
	valC : 64 = 0;
	valP : 64 = 0;
	Stat : 3 = STAT_AOK;
}

f_icode = i10bytes[4..8];
f_ifun = i10bytes[0..4];

f_rA = [
	f_icode in {RRMOVQ, OPQ, RMMOVQ, MRMOVQ, PUSHQ, POPQ} : i10bytes[12..16];
	1 : REG_NONE;
];
f_rB = [
	f_icode in {RRMOVQ, IRMOVQ, OPQ, RMMOVQ, MRMOVQ} : i10bytes[8..12];
	1 : REG_NONE;
];

f_valC = [
	f_icode == JXX || f_icode == CALL : i10bytes[8..72];
	1 : i10bytes[16..80];
];

f_valP = [
	f_icode in { HALT, NOP, RET } : pc + 1;
	f_icode in { RRMOVQ, OPQ, PUSHQ, POPQ } : pc + 2;
	f_icode in { JXX, CALL } : pc + 9;
	1 : pc + 10;
];

f_Stat = [
	f_icode == HALT : STAT_HLT;
	f_icode > 0xb : STAT_INS;
	1 : STAT_AOK;
];

f_predPC = [
	f_Stat != STAT_AOK : pc;
	f_icode in {JXX} : f_valC;
	f_icode in {CALL} : f_valC;
	1 : f_valP;
];

pc = [
    	!m_conditionsMet && m_icode in {JXX} : m_valP ;
	W_icode == RET : W_valM;
    	1: F_predPC ;
];

stall_F = loadUse || (f_Stat != STAT_AOK) || RET in {D_icode, E_icode, M_icode};


########## Decode #############
register dE {
	icode : 4 = NOP;
	ifun : 4 = 0;
	rA : 4 = REG_NONE;
	rB : 4 = REG_NONE;
	dstE : 4 = REG_NONE;
	dstM : 4 = REG_NONE;
	valA : 64 = 0;
	valB : 64 = 0;
	valC : 64 = 0;
	valP : 64 = 0;
	Stat : 3 = STAT_AOK;
}

d_icode = D_icode;
d_ifun = D_ifun;
d_rA = D_rA;
d_rB = D_rB;
d_valC = D_valC;
d_valP = D_valP;
d_Stat = D_Stat;

reg_srcA = [
	d_icode in {RRMOVQ, OPQ, RMMOVQ, PUSHQ, POPQ} : d_rA;
	1 : REG_NONE;
];

reg_srcB = [
	d_icode in {RRMOVQ, OPQ, RMMOVQ, MRMOVQ} : d_rB;
	d_icode in {PUSHQ, POPQ, CALL, RET} : 4;
	1 : REG_NONE;
];

d_valA = [
	e_icode != MRMOVQ && reg_srcA == e_dstE && reg_srcA != REG_NONE : e_valE;
	m_icode == MRMOVQ && reg_srcA == m_dstE && reg_srcA != REG_NONE : m_valM;
	W_icode == MRMOVQ && reg_srcA == W_dstE && reg_srcA != REG_NONE : W_valM;
	reg_srcA == m_dstE && reg_srcA != REG_NONE : m_valE;
	reg_srcA == m_dstM && reg_srcA != REG_NONE : m_valM;
	reg_srcA == W_dstM && reg_srcA != REG_NONE : W_valM;
	reg_srcA == reg_dstE && reg_srcA != REG_NONE : reg_inputE;
	reg_srcA == reg_dstM && reg_srcA != REG_NONE : reg_inputM;
	1 : reg_outputA;
];

d_valB = [
	e_icode != MRMOVQ && reg_srcB == e_dstE && reg_srcB != REG_NONE : e_valE;
	m_icode == MRMOVQ && reg_srcB == m_dstE && reg_srcB != REG_NONE : m_valM;
	W_icode == MRMOVQ && reg_srcB == W_dstE && reg_srcB != REG_NONE : W_valM;
	reg_srcB == m_dstE && reg_srcB != REG_NONE : m_valE;
	reg_srcB == reg_dstE && reg_srcB != REG_NONE : reg_inputE;
	1 : reg_outputB;
];

d_dstE = [
	d_icode in {IRMOVQ, RRMOVQ, OPQ} : d_rB;
	d_icode in {MRMOVQ} : d_rA;
	d_icode in {PUSHQ, POPQ, CALL, RET} : 4;
	1 : REG_NONE;
];

d_dstM = [
	d_icode == POPQ : d_rA;
	1 : REG_NONE;
];

wire loadUse : 1;
loadUse = [
	e_icode == MRMOVQ && reg_srcA == e_dstE && reg_srcA != REG_NONE : 1;
	e_icode == MRMOVQ && reg_srcB == e_dstE && reg_srcB != REG_NONE : 1;
	reg_srcA == e_dstM && reg_srcA != REG_NONE : 1;
	reg_srcB == e_dstM && reg_srcB != REG_NONE : 1;
	1 : 0;
];
wire mispredicted : 1;
mispredicted = [
	!e_conditionsMet && e_icode in {JXX} : 1;
	1 : 0;
];

stall_D = loadUse;
bubble_D = mispredicted || !loadUse && RET in {D_icode, E_icode, M_icode};

########## Execute #############
register eM {
	icode : 4 = NOP;
	ifun : 4 = 0;
	dstE : 4 = REG_NONE;
	dstM : 4 = REG_NONE;
	valA : 64 = 0;
	valB : 64 = 0;
	valC : 64 = 0;
	valE : 64 = 0;
	valP : 64 = 0;
	Stat : 3 = STAT_AOK;
	conditionsMet : 1 = 0;
}

e_icode = E_icode;
e_ifun = E_ifun;
e_dstE = [
	!e_conditionsMet && e_icode == CMOVXX : REG_NONE;
	1 : E_dstE;
];
e_dstM = E_dstM;
e_valA = E_valA;
e_valB = E_valB;
e_valC = E_valC;
e_valP = E_valP;
e_Stat = E_Stat;

e_valE = [
	e_icode in {IRMOVQ} : e_valC;
	e_icode in {RRMOVQ} : e_valA;
	e_icode == OPQ && e_ifun == 0 : e_valA + e_valB;
	e_icode == OPQ && e_ifun == 1 : e_valB - e_valA;
	e_icode == OPQ && e_ifun == 2 : e_valA & e_valB;
	e_icode == OPQ && e_ifun == 3 : e_valA ^ e_valB;
	e_icode == RMMOVQ || e_icode == MRMOVQ : e_valB + e_valC;
	e_icode == PUSHQ || e_icode == CALL : -8 + e_valB;
	e_icode == POPQ || e_icode == RET : 8 + e_valB;
	1 : 0; 
];

c_ZF = [
     	e_icode == OPQ : (e_valE == 0);
     	1 : C_ZF;
];

c_SF = [
	e_icode == OPQ : (e_valE >= 0x8000000000000000);
     	1 : C_SF;
];

e_conditionsMet = [
	e_ifun == 0 : 1;
	e_ifun == 1 : C_SF || C_ZF;
	e_ifun == 2 : C_SF;
	e_ifun == 3 : C_ZF;
	e_ifun == 4 : !C_ZF;
	e_ifun == 5 : !C_SF;
	e_ifun == 6 : !C_SF && !C_ZF;
	1 : 0;
];


########## Memory #############
register mW {
	icode : 4 = NOP;
	dstE : 4 = REG_NONE;
	dstM : 4 = REG_NONE;
	valA : 64 = 0;
	valB : 64 = 0;
	valC : 64 = 0;
	valE : 64 = 0;
	valM : 64 = 0;
	valP : 64 = 0;
	Stat : 3 = STAT_AOK;
	conditionsMet : 1 = 0;
}

m_icode = M_icode;
m_dstE = M_dstE;
m_dstM = M_dstM;
m_valA = M_valA;
m_valB = M_valB;
m_valC = M_valC;
m_valE = M_valE;
m_valP = M_valP;
m_Stat = M_Stat;
m_valM = mem_output;
m_conditionsMet = M_conditionsMet;

mem_input = [
	m_icode == CALL : m_valP;
	1 : m_valA;
];
mem_addr = [
	m_icode == POPQ || m_icode == RET : m_valB;
	1 : m_valE;
];
mem_readbit = [
	m_icode == MRMOVQ || m_icode == POPQ || m_icode == RET : 1;
	1 : 0;
];
mem_writebit = [
	m_icode == RMMOVQ || m_icode == PUSHQ || m_icode == CALL: 1;
	1 : 0;
];

bubble_E = loadUse || mispredicted;

########## Writeback #############


reg_dstE = [
	W_icode in {IRMOVQ, RRMOVQ, OPQ, MRMOVQ, PUSHQ, POPQ, CALL, RET} : W_dstE;
	1 : REG_NONE;
];

reg_dstM = [
	W_icode == POPQ : W_dstM;
	1 : REG_NONE;
];

reg_inputE = [ # unlike book, we handle the "forwarding" actions (something + 0) here
	W_icode in {RRMOVQ} : W_valA;
	W_icode in {IRMOVQ} : W_valC;
	W_icode in {OPQ, PUSHQ, POPQ, CALL, RET} : W_valE;
	W_icode == MRMOVQ : W_valM;
        1: 0xBADBADBAD;
];

reg_inputM = [
	W_icode == POPQ : W_valM;
	1: 0;
];


########## PC and Status updates #############
Stat = [
	## f_Stat == 1 || d_Stat == 1 || e_Stat == 1 || m_Stat == 1: 1;##
	1 : W_Stat;
];



