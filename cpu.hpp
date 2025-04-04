#ifndef CPU_H
#define CPU_H
#define SC_INCLUDE_FX

#include <iostream>
#include <fstream>
#include <systemc>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <vector>
#include <sstream>

#include <cstdlib>
//#include<conio.h>
#include <ctime>




#include "hard.hpp"

using namespace std;

class Cpu:public sc_core::sc_module
{
public:
	SC_HAS_PROCESS(Cpu);
	Cpu(sc_core::sc_module_name name);
	~Cpu();
	tlm_utils::simple_initiator_socket<Cpu> interconnect_socket;


    int PlayOut;
    int EVA;
    bool provocation;
    int get_ip();

    //void clean();
    void Board();
    void PlayPosition(char XO);
    void main_thread();
   

protected:

	sc_core:: sc_time offset;

	
	int GetValue(int t);
	int AIManager();
	int NegaMax(int Depth);
	int game_play();

	
	



	void read_bram(sc_uint<64> addr, unsigned char *all_data,int length);
	void write_bram(sc_uint<64> addr,unsigned char val);
	int read_hard(sc_uint<64> addr);
	void write_hard(sc_uint<64> addr,int val);

private:
	int ip_result;
};

#endif