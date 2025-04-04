#ifndef HARD_HPP_
#define HARD_HPP_

#include<systemc>
#include<vector>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "defines.hpp"
#include "utils.hpp"


class Hard: public sc_module
{
public:
	Hard(sc_module_name);
	~Hard();

	tlm_utils::simple_target_socket<Hard> interconnect_socket;
	tlm_utils::simple_initiator_socket<Hard> bram_socket;

protected:
	pl_t pl;
	sc_time offset;

	//input parameters
	//char input[43];
	sc_uint<1> start;

	//output parameters
	sc_uint<1> ready;

	sc_uint<3> win_value;

	uint8_t winning(sc_time&);

	void b_transport(pl_t&, sc_time&);
	void write_bram(sc_uint<64> addr, unsigned char val);
	unsigned char read_bram(sc_uint<64> addr);

};



#endif //HARD_HPP_