#include <systemc.h>
#include <iostream>
#include "modules.h"

using namespace std;

/*
* Operações: 1-and 2-or 3-xor 4-not 5-comparação 6-soma 7-subtração
* 8-Lê memória 9-armazena memória
* Para instruções(1,2,3,5,6,7):
*	op - operação
*	opx- endereço registrador x
* 	mem- registrador em que vai ficar salvo
* Para instrução 4:
*	op - operação
*	op1 - endereço do registrador 1
*	op2 - pode por qualquer coisa.
*	mem - Registrador em que vai ficar salvo o valor
* Para instrução 8:
*	op - operação
*	op1 e op2 tanto faz. O registrador 31 é o que sempre vai ter o valor da leitura em memória.
*	mem - Posição da memória que tu quer ler.
* Para instrução 9:
*	op - operação
*	op1 - registrador cujo valor vai ser registrado em memória.
* 	op2 - tanto faz
* 	mem - Posição de memória que tu quer salvar o valor.
* Para instrução 10(jump incondicional):
	op - operação
	op1 - pra qual instrução tu quer ir
	op2 e mem = lixo
*/
int sc_main (int argc, char* argv[]) {
	sc_signal<int> op,op1,op2, mem;
	/*
	* op = operação
	* op1 = endereço registrador 1
	* op2 = endereço registrador 2
	* mem = Posição da memória/registrador em caso de Leitura ou escrita em memória ou para write-backs.
	*/
	// Saídas de cada componente
	sc_signal<int> spc1,spc2,spc3,spc4; 
	sc_signal<int> smi1,smi2,smi3,smi4;
	sc_signal<int> sbi1,sbi2,sbi3,sbi4;
	sc_signal<int> sbr1,sbr2;
	sc_signal<int> sie1,sie2,sie3;
	sc_signal<int> sem1,sem2,sem3,sem4;
	sc_signal<int> ulaRes;
	sc_signal<int> sme;
	sc_signal<int> swb1, swb2;
	sc_signal<int> addressE, addressS;
	sc_signal<bool> enableMI, globalEN;
	sc_clock relogio("clock", 1, SC_NS, 1, 1, SC_NS);



	pc pc1("Program_Counter");
		pc1.endereco(addressE);
		pc1.endS(addressS);
		pc1.clock(relogio);
		pc1.enable(globalEN);

	MemIns memIn("Memoria_de_Instrucao");
		memIn.palavra[0](op);
		memIn.palavra[1](op1);
		memIn.palavra[2](op2);
		memIn.palavra[3](mem);
		memIn.endereco(addressS);
		memIn.clock(relogio);
		memIn.envio[0](smi1);
		memIn.envio[1](smi2);
		memIn.envio[2](smi3);
		memIn.envio[3](smi4);
		memIn.enableLOAD(globalEN);

	JumpControl jc("Controle_de_pulo");
		jc.ope(sbi1);
		jc.instrucao(sbi2);
		jc.pule(addressE);
		jc.ende(addressS);
		jc.enable(globalEN);

	IFID bIFID("IF_ID");
		bIFID.palavra[0](smi1);
		bIFID.palavra[1](smi2);
		bIFID.palavra[2](smi3);
		bIFID.palavra[3](smi4);
		bIFID.enable(globalEN);
		bIFID.clock(relogio);
		bIFID.op(sbi1);
		bIFID.op1(sbi2);
		bIFID.op2(sbi3);
		bIFID.memP(sbi4);

	bREG reg("Banco_registradores");
		reg.rr1(sbi2);
		reg.rr2(sbi3);
		reg.wbR(swb2);
		reg.wbV(swb1);
		reg.rd1(sbr1); // Vai pra memória em caso de 9
		reg.rd2(sbr2);
		reg.enable(globalEN);
		reg.clock(relogio);

	IDEX bIDEX("ID_EX");
		bIDEX.op1(sbr1);
		bIDEX.op2(sbr2);
		bIDEX.op(sbi1);
		bIDEX.opS(sie1);
		bIDEX.s1(sie2); // Vai pra memória em caso de 9
		bIDEX.s2(sie3); 
		bIDEX.clock(relogio);
		bIDEX.enable(globalEN);

	alu ula("ULA");
		ula.op(sie1);
		ula.op1(sie2);
		ula.op2(sie3);
		ula.res(ulaRes);
		//ula.enable(globalEN);

	EXMEM em("EX_MEM");
		em.resALU(ulaRes);
		em.op(sie1);
		em.memP(sbi4);
		em.vR(sie2);
		em.clock(relogio);
		em.srALU(sem1);
		em.sop(sem2);
		em.smP(sem3);
		em.svR(sem4);
		em.enable(globalEN);

	MEM memo("Memoria");
		memo.memP(sem3);
		memo.valor(sem4);
		memo.op(sem2);
		memo.vS(sme);
		memo.enable(globalEN);

	MEMWB mwb("MEM_WB");
		mwb.vmem(sme);
		mwb.resALU(ulaRes);
		mwb.op(sem2);
		mwb.pR(sem3);
		mwb.clock(relogio);
		mwb.sV(swb1);
		mwb.sR(swb2);
		mwb.enable(globalEN);

	//addressE = 0;
	globalEN = false;
	enableMI = false;
	for(int i = 0; i < 20; i++){
		cout << i << endl;
		if(i == 0){
			enableMI = true;
			op = 6;
			op1 = 4;
			op2 = 5;
			mem = 10;
		}
		if(i==1){
			enableMI = true;
			op = 7;
			op1 = 6;
			op2 = 3;
			mem = 15;
		}
		
		if(i==2){
			enableMI = true;
			op = 10;
			op1 = 2;
			op2 = 0;
			mem =0;
		}

		if(i==3){
			globalEN = true;
		}
		sc_start(1, SC_NS);
		enableMI = false;
	}

}

