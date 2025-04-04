#include "hard.hpp"

SC_HAS_PROCESS(Hard);

Hard::Hard(sc_module_name name):
	sc_module(name),
	ready(1)
	{
		interconnect_socket.register_b_transport(this,&Hard::b_transport);
		SC_REPORT_INFO("Hard", "Constructed.");
	}

Hard::~Hard()
{
	SC_REPORT_INFO("Hard","Destroyed");
}

void Hard::b_transport(pl_t &pl,sc_time &offset)
{
	tlm_command cmd=pl.get_command();
	sc_dt::uint64 addr=pl.get_address();
	unsigned int len= pl.get_data_length();
	unsigned char *buf= pl.get_data_ptr();
	pl.set_response_status(TLM_OK_RESPONSE);

	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			switch(addr)
			{
			case ADDR_START:
				start=toInt(buf);
				//cout<<"start= "<<start<<endl;
				if( start==1 && ready==1){
				ready=0;
				winning(offset);
				ready=1;	
				}
				break;
			default:
				pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);

			}
			break;
		
		case tlm::TLM_READ_COMMAND:
			switch(addr)
			{
				// preuzimanje vrednosti sa winninga
				case ADDR_WIN_VAL:
					win_value=winning(offset);
					toUchar(buf,win_value);
					break;


				case ADDR_READY:
					toUchar(buf,ready);
					break;
				default:
					pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
			}
			break;

		default:
			pl.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
			cout<<"Wrong command"<<endl;
	}
}



uint8_t Hard::winning(sc_core::sc_time &system_offset)
{
	{
	

		//Provera horizontalnih linija
			
			for(int row = 0; row < 6; row++){ // 6 redova
		        for (int col = 0; col <= 3; col++) { // Maksimalno 4 startne tačke po redu
		            unsigned char symbol = read_bram(row * 7 + col);
		            if (symbol != ' ' &&
		                symbol == read_bram(row * 7 + col + 1) &&
		                symbol == read_bram(row * 7 + col + 2) &&
		                symbol == read_bram(row * 7 + col + 3))

		                								 {
		                return (symbol == 'X') ? 1 : 2;
		            }
		        }
		    }

		    // Provera vertikalnih linija
		    for (int col = 0; col < 7; col++) { // 7 kolona
		        for (int row = 0; row <= 2; row++) { // Maksimalno 3 startne tačke po koloni
		           unsigned char symbol = read_bram(row * 7 + col);
		            if (symbol != ' ' &&
		                symbol == read_bram((row + 1) * 7 + col) &&
		                symbol == read_bram((row + 2) * 7 + col) &&
		                symbol == read_bram((row + 3) * 7 + col)) {
		                return (symbol == 'X') ? 1 : 2;
		            }
		        }
		    }

		    // Provera dijagonala (desno-nadole)
		    for (int row = 0; row <= 2; row++) { // 3 startne tačke po redu
		        for (int col = 0; col <= 3; col++) { // 4 startne tačke po koloni
		            unsigned char symbol = read_bram(row * 7 + col);
		            if (symbol != ' ' &&
		                symbol == read_bram((row + 1) * 7 + col + 1) &&
		                symbol == read_bram((row + 2) * 7 + col + 2) &&
		                symbol == read_bram((row + 3) * 7 + col + 3)) {
		                return (symbol == 'X') ? 1 : 2;
		            }
		        }
		    }

		    // Provera dijagonala (levo-nadole)
		    for (int row = 0; row <= 2; row++) { // 3 startne tačke po redu
		        for (int col = 3; col < 7; col++) { // 4 startne tačke po koloni
		            unsigned char symbol = read_bram(row * 7 + col);
		            if (symbol != ' ' &&
		                symbol == read_bram((row + 1) * 7 + col - 1) &&
		                symbol == read_bram((row + 2) * 7 + col - 2) &&
		                symbol == read_bram((row + 3) * 7 + col - 3)) {
		                return (symbol == 'X') ? 1 : 2;
		            }
		        }
		    }

		    // Provera da li je tabla puna
		    for (int i = 0; i < 42; i++) { // 42 pozicije
		    	unsigned char symbol=read_bram(i);
		        if (symbol == ' ') {
		            return 0; // Igra se nastavlja
		        }
		    }

		    return 3; // Nerešeno
		   	    
	}
	return 0;
}






void Hard::write_bram(sc_uint<64> addr, unsigned char val)
{
    // upitno da li je neohodna??
	/*pl_t pl;
	unsigned char buf;
	buf = val;
	pl.set_address(addr);
	pl.set_data_length(1); 
	pl.set_data_ptr(&buf);
	pl.set_command( tlm::TLM_WRITE_COMMAND );
	pl.set_response_status ( tlm::TLM_INCOMPLETE_RESPONSE );
	bram_socket->b_transport(pl, offset);*/


}

unsigned char Hard::read_bram(sc_uint <64> addr)
{
    pl_t pl;
    unsigned char buf;
    pl.set_address(VP_ADDR_BRAM_L+addr);
    pl.set_data_length(1);
    pl.set_data_ptr(&buf);
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    
    // DEBUG: Before reading
   // SC_REPORT_INFO("HARD", ("Attempting to read from BRAM addr: " + std::to_string(addr)).c_str());
    
    bram_socket->b_transport(pl,offset);
    
    // DEBUG: After reading
   /* SC_REPORT_INFO("HARD", ("Read from BRAM addr " + std::to_string(addr) + ": '" + std::string(1, buf) + "' (0x" + to_hex(buf) + ")").c_str());*/
    
    return buf;
}
