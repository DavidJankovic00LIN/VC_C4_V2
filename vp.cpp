#include "vp.hpp"

Vp::Vp (sc_core::sc_module_name name):
sc_module(name),
cpu("Cpu"),
interconnect("Interconnect"),
hard("Hard"),
bram("Bram")

{
	cpu.interconnect_socket.bind(interconnect.cpu_socket);
	interconnect.bram_socket.bind(bram.bram_socket_1);
	interconnect.hard_socket.bind(hard.interconnect_socket);
	hard.bram_socket.bind(bram.bram_socket_2);

	SC_REPORT_INFO("VP","Constructed.");

}

Vp::~Vp()
{
	SC_REPORT_INFO("VP","Destroyed.");
}