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
			case 11:// Pulo se falso
				cout << "Branch if negative: " << op1 << " < 0?" << endl;
				res.write(op1 < 0);
			case 12:// Pulo if zero
				cout << "Branch if 0: " << op1 << " == 0?" << endl;
				res.write(op1 == 0);
 		}
	}

	SC_CTOR(alu){
		SC_METHOD(opera);
			//sensitive << op;
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
	sc_in_clk clock;
	sc_in<int> endereco;
	sc_in<bool> enable;
	sc_out<int> endS;

	void manda(){
		if(endereco < 19 && enable){
			//cout << "PC Endereço: " << endereco <<endl;
			endS.write(endereco);
		}
	}

	SC_CTOR(pc){
		SC_METHOD(manda);
			sensitive << clock.neg();
			//sensitive << endereco;
	}
};


SC_MODULE(MemIns){
	sc_in<int> palavra[4];
	sc_in<bool> enableLOAD;
	sc_in<int> endereco;
	sc_in_clk clock;
	int memoria[80] = {0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0}; // Cabem 20 (ou 18, sei lá pq) instruções aqui.
	int i = 0;
	int count = 0;
	bool canWrite = true;
	sc_out<bool> stop;
	sc_out<int> envio[4];

	/*
	void escreve(){
		for(int i = 0; i < 4; i++){
			//cout << endereco << endl;
			memoria[(endereco*4)+i] = palavra[i];
			cout << endereco*4+i << " " << memoria[(endereco*4)+i] << endl;
		}
	}
	*/

	void envia(){
		if(enableLOAD == true){
			//cout << "Saida Mem Ins: ";
			for(int i = 0; i < 4; i++){
				if(canWrite){
					envio[i].write(memoria[endereco*4+i]);
				}
				//cout << memoria[endereco*4+i]<< " ";
			}
			//cout << endl;
			checks();
		}
		
	}

	void checks(){
		if(memoria[(endereco+1)*4] >= 10 && canWrite != false){
			canWrite = false;
			stop.write(canWrite);
			count = 3;
		}else if(count == 0){
			canWrite = true;
			stop.write(canWrite);
		}else{
			count = count - 1;
		}
		
		if(memoria[(endereco+2)*4+1] == memoria[(endereco)*4+3] || memoria[(endereco+2)*4+2] == memoria[(endereco)*4+3] && canWrite != false){
			canWrite = false;
			stop.write(canWrite);
			count = 1;
		}
		
		//cout << count << " Count"<< endl;
	}

	void test(){
		if(enableLOAD == false){
			//cout << i << " i" <<endl;
			for(int z = 0; z < 4; z++){
				memoria[(i*4)+z] = palavra[z];
			}
			i++;
			/*
			for(int z = 0; z < 80; z++){
				cout << memoria[z] << " ";
			}
			cout << endl;
			*/
		}
	}

	SC_CTOR(MemIns){
		/*SC_METHOD(escreve);
			sensitive << endereco;
			sensitive << palavra[0];
			sensitive << palavra[1];
			sensitive << palavra[2];
			sensitive << palavra[3];
			*/
		SC_METHOD(envia);
			sensitive << clock.neg();

		SC_METHOD(test);
			sensitive << clock.pos();
	}
};

SC_MODULE(IFID){
	sc_in<int> palavra[4];
	sc_in<bool> MemInCW;
	sc_in_clk clock;
	sc_out<int> op1,op2,op, memP;
	sc_in<bool> enable;
	sc_out<bool> sMemInCW;
	int x[4];

	void nafila(){
		//cout << enable << " IFID Aqui ";
		if(enable == true){
			for(int i = 0; i < 4 ; i++){
				x[i] = palavra[i];
				//cout << palavra[i] << " ";
			}
		}
	}

	void fora(){
		if(enable == true){
			op.write(x[0]);
			op1.write(x[1]);
			op2.write(x[2]);
			memP.write(x[3]);
			sMemInCW.write(MemInCW);
		}
	}

	SC_CTOR(IFID){
		SC_METHOD(nafila);
			sensitive << clock.pos();
		SC_METHOD(fora);
			sensitive << clock.neg();
	}
};

SC_MODULE(JumpControl){
	sc_in<int> ope;
	sc_in<int> ope2;
	sc_in<int> instrucao;
	sc_in<int> instrucao2;
	sc_out<int> pule;
	sc_in<int> ende;
	sc_in<bool> enable, CW;

	void pula(){
		if(enable && CW){
			int x = instrucao -1;
			int y = instrucao2 ;
			if(ope == 10){
				pule.write(x);
				//cout << x << " x <-JC-> " << ope <<pule <<endl;
			}else if(ope2 == 11 || ope2 == 12){
				pule.write(y);
				//cout << y << " y <-JC-> " << ope << " "<< pule <<endl;
			}else{
				pule.write(ende+1);
				//cout << pule << endl;
			}
		}
	}

	SC_CTOR(JumpControl){
		SC_METHOD(pula);
			sensitive << ope;
			sensitive << instrucao;
			sensitive << ende;
			sensitive << ope2;
			sensitive << instrucao2;
	}
};
/*
SC_MODULE(Controlador){
	sc_in<int> palavra[4];
	int ins1[4] = {0,0,0,0}; 
	int ins2[4] = {0,0,0,0};

	checkCon(){
		for(int i = 0; i < 4; i++){
			ins2[i] = ins1[i];
			ins1[i] = palavra[i];
		}
		if(ins2[3] == ins1[1] || ins2[3] == ins1[2]){

		}
	}
}
*/
SC_MODULE(bREG){
	sc_in<int> rr1;
	sc_in<int> rr2;
	sc_in<int> wbR;
	sc_in<int> wbV;
	sc_in<bool> enable;
	int banco[32] = {0,1,2,3,4,5,6,-7,
					 0,7,6,5,4,3,2,1,
					 1,7,8,6,3,2,5,4,
					 6,2,1,3,4,5,8,7};
	sc_out<int> rd1;
	sc_out<int> rd2;
	sc_in_clk clock;

	void salva(){
		if(enable == true){
			banco[wbR] = wbV;
			cout <<"Valor do registrador(WB) "<< wbR << ": "<< banco[wbR] << endl;
		}
	}

	void writeR(){
		if(enable ==true){
			rd1.write(banco[rr1]);
			//cout <<"Valor do registrador " << rr1 << ": " << banco[rr1] <<endl;
 			rd2.write(banco[rr2]);
 			//cout <<"Valor do registrador " << rr2 << ": " << banco[rr2] <<endl;
		}	
	}

	SC_CTOR(bREG){
		SC_METHOD(writeR);
			sensitive << clock.pos();
		SC_METHOD(salva);
			sensitive << wbR;
			sensitive << wbV;
	}
};

SC_MODULE(IDEX){
	sc_in<int> op1;
	sc_in<int> op2;
	sc_in<int> op;
	sc_in<int> memP;
	sc_in<bool> enable;
	sc_out<int> s1;
	sc_out<int> s2;
	sc_out<int> opS;
	sc_out<int> memS;
	int x[4];
	sc_in_clk clock;

	void prafila(){
		if(enable == true){
			x[0] = op1;
			x[1] = op2;
			x[2] = op;
			x[3] = memP;
		}
	}

	void writeIDEX(){
		if(enable == true){
			opS.write(x[2]);
			s1.write(x[0]);
			s2.write(x[1]);	
			memS.write(x[3]);
			//cout << opS << " " << s1 << " " << s2 << " " << memS << endl;
		}
	}

	SC_CTOR(IDEX){
		SC_METHOD(prafila);
			sensitive << op1;
			sensitive << op2;
			sensitive << op;
			//sensitive << memP;
		SC_METHOD(writeIDEX);
			sensitive << clock.neg();
	}
};

SC_MODULE(EXMEM){
	sc_in<int> resALU;
	sc_in<int> op;
	sc_in<int> memP;
	sc_in<int> vR;
	sc_in<bool> enable;
	sc_out<int> srALU;
	sc_out<int> sop;
	sc_out<int> smP;
	sc_out<int> svR;
	sc_out<int> stoJCOP;
	sc_out<int> stoJCINS;
	int x[4];
	sc_in_clk clock;

	void prafila(){
		if(enable == true){
			x[0] = resALU;
			x[1] = op;
			x[2] = memP;
			x[3] = vR;
		}
	}

	void writeEXMEM(){
		if(enable == true){
			if(x[1] == 11 || x[1] == 12){
				if(resALU == true){
					stoJCINS.write(x[2]);
					stoJCOP.write(x[1]);
				}
			}else{
				srALU.write(x[0]);
				sop.write(x[1]);
				smP.write(x[2]);
				svR.write(x[3]);
				//cout << "EXMEM: "<<srALU << " " << sop << " " << smP << " " << svR << endl;
			}
		}
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
	sc_in<bool> enable;
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
		if(enable == true){
			switch(op){
				case 8:
					lemem();
					break;
				case 9:
					armazena();
					break;
			}
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
	sc_in<bool> enable;
	sc_out<int> sV;
	sc_out<int> sR;
	int x[4];
	sc_in_clk clock;

	void writeMEMWB(){
		if(enable == true){
			x[0] = vmem;
			x[1] = resALU;
			x[2] = op;
			x[3] = pR;
		}
	}

	void dsaida(){
		if(enable == true){
			if(op == 8){
				sV.write(x[0]);
				sR.write(31);
			}else{
				sV.write(x[1]);
				sR.write(x[3]);
			}
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