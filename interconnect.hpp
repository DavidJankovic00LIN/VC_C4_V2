#ifndef INTERCONNECT_HPP_
#define INTERCONNECT_HPP_
#define SC_INCLUDE_FX

#include <systemc>
#include <iostream>
#include <string>
#include "defines.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
using namespace sc_core;

class Interconnect : public sc_core::sc_module
{
	public:
		Interconnect(sc_core::sc_module_name name);
		~Interconnect();
		tlm_utils::simple_initiator_socket<Interconnect> bram_socket;
		tlm_utils::simple_initiator_socket<Interconnect> hard_socket;
		tlm_utils::simple_target_socket<Interconnect> cpu_socket;
	 

	protected:
		pl_t pl;
		sc_core::sc_time offset;
		void b_transport(pl_t &pl, sc_core::sc_time &offset);
};

#endif // INTERCONNECT_HPP_