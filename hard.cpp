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
				winning(offset,last_move);
				ready=1;	
				}
				break;
			case ADDR_LAST_MOVE:
            	last_move = toInt(buf);  // Čuva last_move
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
					win_value=winning(offset, last_move);
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



uint8_t Hard::winning(sc_core::sc_time &system_offset, int last_move) {
    unsigned char symbol = read_bram(last_move);
    if (symbol == ' ') return 0;

    int row = last_move / 7;
    int col = last_move % 7;

    // Provera horizontale
    if (col <= 3 && 
        symbol == read_bram(row * 7 + col + 1) && 
        symbol == read_bram(row * 7 + col + 2) && 
        symbol == read_bram(row * 7 + col + 3)) {
        return (symbol == 'X') ? 1 : 2;
    }

    // Provera vertikale
    if (row <= 2 && 
        symbol == read_bram((row + 1) * 7 + col) && 
        symbol == read_bram((row + 2) * 7 + col) && 
        symbol == read_bram((row + 3) * 7 + col)) {
        return (symbol == 'X') ? 1 : 2;
    }

    // Provera dijagonale (/)
    if (row <= 2 && col <= 3 && 
        symbol == read_bram((row + 1) * 7 + col + 1) && 
        symbol == read_bram((row + 2) * 7 + col + 2) && 
        symbol == read_bram((row + 3) * 7 + col + 3)) {
        return (symbol == 'X') ? 1 : 2;
    }

    // Provera dijagonale (\)
    if (row <= 2 && col >= 3 && 
        symbol == read_bram((row + 1) * 7 + col - 1) && 
        symbol == read_bram((row + 2) * 7 + col - 2) && 
        symbol == read_bram((row + 3) * 7 + col - 3)) {
        return (symbol == 'X') ? 1 : 2;
    }

    // Provera punog table (samo za donji red)
    if (row == 5) {
        for (int c = 0; c < 7; c++) {
            if (read_bram(5 * 7 + c) == ' ') return 0;
        }
        return 3;
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
