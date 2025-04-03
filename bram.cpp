 #include "bram.hpp"

Bram::Bram(sc_core::sc_module_name name):sc_module(name)
{
	
	bram_socket_1.register_b_transport(this,&Bram::b_transport);
	bram_socket_2.register_b_transport(this,&Bram::b_transport);
	mem.reserve(BRAM_SIZE);
    for (int i = 0; i < BRAM_SIZE; ++i)
    {
    mem.push_back(' ');  // Dodaje elemente jedan po jedan
    }   

	SC_REPORT_INFO("BRAM","Constructed.");
}

Bram::~Bram()
{
	SC_REPORT_INFO("BRAM","Destroyed.");
}

void Bram::b_transport(pl_t &pl, sc_core::sc_time &offset)
{
	tlm::tlm_command cmd = pl.get_command(); //u cmd upisujemo komadnu sa payload-a
	sc_dt::uint64 addr = pl.get_address(); // u addr se upisuje adresa koja stize iz payload-a
	// $$$ proveriti kolika ce nam trebati duzina za adresu i eventualno je smanjti
	unsigned int len = pl.get_data_length(); // u len upisujemo duzinu podatka
	unsigned char *buf = pl.get_data_ptr(); // ako je write buf pokazuje na podatke koji treba upisati u bram,ako je read,buf pokazuje  na prosor gde ce se pisati procitani podaci


 //u zavisnosti od cmd komande- read ili write 

	switch(cmd)
	{
	case tlm::TLM_WRITE_COMMAND:
		for(unsigned int i=0; i<len; ++i)
		{
			mem[addr + i] = buf[i]; // podatak iz bafera se upisuje u memoriju i povecava se adreas
		}
		pl.set_response_status(tlm::TLM_OK_RESPONSE); // dobija status ok nakon zavrsetka upisa

		offset += sc_core::sc_time(DELAY,sc_core::SC_NS); //simulirano je kasnjenje  da bi se emuliralo realno vreme pristupa memoriji
		break;

	case tlm::TLM_READ_COMMAND:
		for (unsigned int i =0; i<len; ++i)
		{
			buf[i]=mem[addr + i]; // iz memorije sa adrese se premesta podatak u bafer, pomera se mesto u memoriji
		}
		pl.set_response_status(tlm::TLM_OK_RESPONSE);

		offset += sc_core::sc_time(DELAY,sc_core::SC_NS);
		break;

	default:
		pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);		
		offset += sc_core::sc_time(DELAY,sc_core::SC_NS);
	}

}