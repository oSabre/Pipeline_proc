#include <systemc.h>
#include <stack>
#include <queue>
#include <iostream>

using namespace std;

SC_MODULE(alu){
	sc_in<int> op;
	sc_in<int> op1, op2;
	sc_out<int> res;

	void opera(){
		switch(op){
			case 1:// and
				cout << "and" << endl;
				res.write(op1 && op2);
				break;
			case 2:// or
				cout << "or" << endl;
				res.write(op1 || op2);
				break;
			case 3:// xor
				cout << "xor" << endl;
				res.write(op1^op2);
				break;
			case 4:// not
				cout << "not" << endl;
				res.write(!op1);
				break;
			case 5:// comparação
				cout << "Compara" << endl;
				res.write(op1 == op2);
				break;
			case 6:// soma
				cout << "soma: " << op1 << " + " << op2 << endl;
				res.write(op1 + op2);
				break;
			case 7:// Subtrai
				cout << "Subtrai: "<< op1 << " - " << op2 << endl;
				res.write(op1 - op2);
				break;
		}
	}

	SC_CTOR(alu){
		SC_METHOD(opera);
			sensitive << op;
			sensitive << op1;
	}
};

SC_MODULE(m21){
	sc_in<int> e1;
	sc_in<int> e2;
	sc_in<int> seletor;
	sc_out<int> saida;

	void action(){
		switch(seletor){
			case 1:
				saida.write(e1);
				break;
			case 2:
				saida.write(e2);
				break;
		}
	}

	SC_CTOR(m21){
		SC_METHOD(action);
			sensitive << e1;
			sensitive << e2;
			sensitive << seletor;
	}
};

SC_MODULE(pc){
	sc_in<int> palavra[4];
	sc_in_clk clock;
	sc_in<int> endereco;
	sc_out<int> endS;
	sc_out<int> saida[4];

	void manda(){
		for(int i = 0; i < 4; i++){
			saida[i].write(palavra[i]);
		}
		if(endereco < 19){
			endS.write(endereco+1);
		}
	}

	SC_CTOR(pc){
		SC_METHOD(manda);
			sensitive << clock.pos();
			sensitive << endereco;
	}
};


SC_MODULE(MemIns){
	sc_in<int> palavra[4];
	sc_in<int> endereco;
	sc_in_clk clock;
	int memoria[80]; // Cabem 20 (ou 18, sei lá pq) instruções aqui.
	sc_out<int> envio[4];

	void escreve(){
		for(int i = 0; i < 4; i++){
			//cout << endereco << endl;
			memoria[(endereco*4)+i] = palavra[i];
			cout << endereco*4+i << " " << memoria[(endereco*4)+i] << endl;
		}
	}

	void envia(){
		for(int i = 0; i < 4; i++){
			envio[i].write(memoria[endereco*4+i]);
			//cout << memoria[endereco*4+i] << endl;
		}
		
	}

	SC_CTOR(MemIns){
		SC_METHOD(escreve);
			sensitive << endereco;
			sensitive << palavra[0];
			sensitive << palavra[1];
			sensitive << palavra[2];
			sensitive << palavra[3];
		SC_METHOD(envia);
			sensitive << clock.neg();
	}
};

SC_MODULE(IFID){
	sc_in<int> palavra[4];
	sc_in_clk clock;
	sc_out<int> op1,op2,op, memP;
	int x[4];

	void nafila(){
		for(int i = 0; i < 4 ; i++){
			x[i] = palavra[i];
		}
	}

	void fora(){
		op.write(x[0]);
		op1.write(x[1]);
		op2.write(x[2]);
		memP.write(x[3]);
	}

	SC_CTOR(IFID){
		SC_METHOD(nafila);
			sensitive << palavra[0];
			sensitive << palavra[1];
			sensitive << palavra[2];
			sensitive << palavra[3];
		SC_METHOD(fora);
			sensitive << clock.neg();
	}
};

SC_MODULE(JumpControl){
	sc_in<int> ope;
	sc_in<int> instrucao;
	sc_out<int> pule;

	void pula(){
		int x = instrucao;
		if(ope == 10){
			pule.write(1);
			cout << x << " " <<pule <<endl;
		}
	}

	SC_CTOR(JumpControl){
		SC_METHOD(pula);
			sensitive << ope;
			sensitive << instrucao;
	}
};

SC_MODULE(bREG){
	sc_in<int> rr1;
	sc_in<int> rr2;
	sc_in<int> wbR;
	sc_in<int> wbV;
	int banco[32] = {0,1,2,3,4,5,6,7,
					 8,7,6,5,4,3,2,1,
					 1,7,8,6,3,2,5,4,
					 6,2,1,3,4,5,8,7};
	sc_out<int> rd1;
	sc_out<int> rd2;

	void salva(){
		banco[wbR] = wbV;
		cout <<"Valor do registrador(WB) "<< wbR << ": "<< banco[wbR] << endl;
	}

	void writeR(){
		rd1.write(banco[rr1]);
		cout <<"Valor do registrador " << rr1 << ": " << banco[rr1] <<endl;
 		rd2.write(banco[rr2]);
 		cout <<"Valor do registrador " << rr2 << ": " << banco[rr2] <<endl;
	}

	SC_CTOR(bREG){
		SC_METHOD(writeR);
			sensitive << rr1;
			sensitive << rr2;
		SC_METHOD(salva);
			sensitive << wbR;
			sensitive << wbV;
	}
};

SC_MODULE(IDEX){
	sc_in<int> op1;
	sc_in<int> op2;
	sc_in<int> op;
	sc_out<int> s1;
	sc_out<int> s2;
	sc_out<int> opS;
	int x[3];
	sc_in_clk clock;

	void prafila(){
		x[0] = op1;
		x[1] = op2;
		x[2] = op;
	}

	void writeIDEX(){
		opS.write(x[2]);
		s1.write(x[0]);
		s2.write(x[1]);	
	}

	SC_CTOR(IDEX){
		SC_METHOD(prafila);
			sensitive << op1;
			sensitive << op2;
			sensitive << op;
		SC_METHOD(writeIDEX);
			sensitive << clock.neg();
	}
};

SC_MODULE(EXMEM){
	sc_in<int> resALU;
	sc_in<int> op;
	sc_in<int> memP;
	sc_in<int> vR;
	sc_out<int> srALU;
	sc_out<int> sop;
	sc_out<int> smP;
	sc_out<int> svR;
	int x[4];
	sc_in_clk clock;

	void prafila(){
		x[0] = resALU;
		x[1] = op;
		x[2] = memP;
		x[3] = vR;
	}

	void writeEXMEM(){
		srALU.write(x[0]);
		sop.write(x[1]);
		smP.write(x[2]);
		svR.write(x[3]);
	}

	SC_CTOR(EXMEM){
		SC_METHOD(prafila);
			sensitive << resALU;
			sensitive << op;
			sensitive << memP;
			sensitive << vR;
		SC_METHOD(writeEXMEM);
			sensitive << clock.neg();
	}
};

SC_MODULE(MEM){
	sc_in<int> memP;
	sc_in<int> valor;
	sc_in<int> op;
	sc_out<int> vS;
	int memoria[32] = {0,1,2,3,4,5,6,7,
					   8,9,10,11,12,13,14,15,
					   16,17,18,19,20,21,22,23,
					   24,25,26,27,28,29,30,31};

	void armazena(){
		memoria[memP] = valor;
		cout << "Valor da memória(R) " << memP << ": "<< memoria[memP] << endl;
	}

	void lemem(){
		vS.write(memoria[memP]);
		cout << "Valor da memoria(L) " << memP << ": "<< memoria[memP] << endl;
	}

	void opera(){
		switch(op){
			case 8:
				lemem();
				break;
			case 9:
				armazena();
				break;
		}
	}

	SC_CTOR(MEM){
		SC_METHOD(opera);
			sensitive << op;
	}
};

SC_MODULE(MEMWB){
	sc_in<int> vmem; //
	sc_in<int> resALU; //resultado alu
	sc_in<int> op; // operação
	sc_in<int> pR; //para o registrador x
	sc_out<int> sV;
	sc_out<int> sR;
	int x[4];
	sc_in_clk clock;

	void writeMEMWB(){
		x[0] = vmem;
		x[1] = resALU;
		x[2] = op;
		x[3] = pR;
	}

	void dsaida(){
		if(op == 8){
			sV.write(x[0]);
			sR.write(31);
		}else{
			sV.write(x[1]);
			sR.write(x[3]);
		}
		
	}

	SC_CTOR(MEMWB){
		SC_METHOD(dsaida);
			sensitive << clock.neg();
		SC_METHOD(writeMEMWB);
			sensitive << vmem;
			sensitive << resALU;
			sensitive << op;
			sensitive << pR;
	}

};