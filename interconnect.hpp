#ifndef INTERCONNECT_HPP_
#define INTERCONNECT_HPP_
#define SC_INCLUDE_FX

#include <systemc>
#include <iostream>
#include <string>
#include "defines.hpp"
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/tlm_quantumkeeper.h"



class Interconnect : public sc_core::sc_module
{
	public:
		Interconnect(sc_core::sc_module_name name);
		~Interconnect();

		tlm_utils::simple_initiator_socket<Interconnect> bram_socket; //soket koji povezuje inter sa bramom
		tlm_utils::simple_initiator_socket<Interconnect> hard_socket; //soket koji povezuje inter sa hardom
		tlm_utils::simple_target_socket<Interconnect> cpu_socket; //cilji soket ->salju se READ WRITE komande sa cpu
	
	protected:
		pl_t pl;
		sc_core::sc_time offset;
		void b_transport(pl_t &pl, sc_core::sc_time &offset); //funkcija za obradu zahteva i preusmeravanju

};

#endif //INTERCONNECT_HPP_