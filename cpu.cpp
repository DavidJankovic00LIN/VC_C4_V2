#include "cpu.hpp"

SC_HAS_PROCESS(Cpu);


int read_ddr_cnt=0;
int write_ddr_cnt=0;



Cpu::Cpu(sc_core::sc_module_name name): 
sc_module(name), 
PlayOut(0),
EVA(0),
provocation(false),
offset(sc_core::SC_ZERO_TIME),
ip_result(0)
{
	
	SC_THREAD(main_thread);
	SC_REPORT_INFO("CPU","Constructed.");


}
Cpu::~Cpu()
{
	SC_REPORT_INFO("Cpu", "Destroyed.");
}

/*
void Cpu::clean()
{
	provocation=false;
	for(int i=0; i<=80 ; i++)
		write_bram(i,' ');
}
*/
int Cpu::get_ip()
{
	//SC_REPORT_INFO("CPU", "Starting HARD processing");

	write_hard(ADDR_START,1); // Pokretanje IP-a
	bool done = false;
	while(!done)
	{
		int ready = read_hard(ADDR_READY); //proverava status ready-a i u sl redu ispisuje
		//SC_REPORT_INFO("CPU", ("Ready status: " + std::to_string(ready)).c_str());

		if(ready){
			write_hard(ADDR_START,0); //zaustavi IP(postavi start na 0)
			done=true;
		}else{
			wait(100000,SC_NS); // cekaj dok hardverska komponenta ne zavrsi
		}
	}	
	//SC_REPORT_INFO("CPU","Geting winner value");
	int ip_result=read_hard(ADDR_WIN_VAL); // procitaj rezultat iz hardvera
	//SC_REPORT_INFO("CPU","HARD processing done");
	return ip_result;
}


/*
U ovoj varijanti funkcije game_play korisnik ima prednost prvog poteza;
int Cpu::game_play()
{
	int tempAI;
	srand(time(0));
	clean();
	
	while(true){
		PlayPosition('X');// uzima se korisnikov potez
		Board();
		tempAI=AIManager();
		get_ip();
		int Win_Value=ip_result; //cita stanje pobednika sa registra

		if(Win_Value!=O)
		{
			if(Win_Value==1)
			{
				cout<<endl<<"Player 2 WON!";
				return 0;
			}
			else if (Win_Value==2)
			{
				cout<<endl<<"Player 1 WON!";
				return 0;
			}
			else if(Win_Value==3){
				cout<<"You Tie!";
				return 0;
			}
			clean();
		}else{
			write_bram(tempAI,'O');
		}

	}
	return 0;
}
*/


int Cpu::game_play()
{
    SC_REPORT_INFO("CPU", "game_play started");
	int tempAI;
	srand(time(0));
	//clean();
	
	while(true){
		tempAI=AIManager();
		write_bram(tempAI,'O');
		Board();
		get_ip();
		int Win_Value=ip_result; //cita stanje pobednika sa registra

		if(Win_Value!=0)
		{
			if(Win_Value==1)
			{
				//cout<<endl<<"Player 2 WON!";
				 SC_REPORT_INFO("CPU", "Player 2 WON");
				return 0;
			}
			else if (Win_Value==2)
			{
				//cout<<endl<<"Player 1 WON!";
				 SC_REPORT_INFO("CPU", "Player 1 WON");
				return 0;
			}
			else if(Win_Value==3){
				//cout<<"You Tie!";
				 SC_REPORT_INFO("CPU", "Tie");
				return 0;
			}
		//clean();
		}else{
			PlayPosition('X'); //Bira se korisnicki potez
		}

	}
	return 0;
}

void Cpu::main_thread()
{
SC_REPORT_INFO("CPU", "Main_Thread_Started");    
game_play();

}


int Cpu::GetValue(int column) //uzima kolonu(1-7),i ako je ta kolona u tom redu free,upisuje u nju.
{
    //SC_REPORT_INFO("CPU", "GetValue started");

	if(column>7)
		return 0;
	int n;
	for(int i=0; i<=6; i++)
	{	
		unsigned char temp_s;
		read_bram(column+7*i,&temp_s,1);
		if(temp_s== ' ')
		{
			n=column+7*i;
			break;
		}
	}
	if (n>42)
		return 0;
	return n;
}


void Cpu::Board() // funkcija za crtanje table

{
    //SC_REPORT_INFO("CPU", "Board started");
	cout<<endl<<"    1   "<<"    2   "<<"    3   "<<"    4   "<<"    5   "<<"    6   "<<"    7   "<<endl;

    int j = 42;
    for(int i = 0 ; i<= 23 ; i++)
    {
        if(i % 4 == 0)
            cout<<string(57,'-');
        else
        {
            if( (i - 2) % 4 == 0)
            {
                j=42-(0.25*i+0.5)*6-((0.25*i+0.5)-1) ;
                for(int i = 0 ; i<=6 ; i++)
                {	
                	unsigned char temp_b;
                	read_bram(j,&temp_b,1);
                    cout<<"|"<<"   "<<temp_b<<"   ";
                    j++;
                }
                cout<<"|";
            }
            else
            {
                for(int i = 0 ; i<=6 ; i++)
                    cout<<"|"<<string(7,' ');
                cout<<"|";
            }
        }
        cout<<endl;
    }
    cout<<string(57,'-');
    if(provocation == true)
        cout<<endl<<"Hehe I'm sure of my winning :D "<<endl;
}


void Cpu::PlayPosition(char XO)
{
    //SC_REPORT_INFO("CPU", "PlayPosition started");
	static std::vector<int>moves;
	static size_t currentMoveIndex=0;

	if (moves.empty()|| currentMoveIndex>=moves.size())
	{
		moves.clear();
		int move;
		ifstream inputFile("input.txt");
		if(!inputFile.is_open())
		{
			cerr<<"Can't open file"<<endl;
			exit(1);
		}

		while (inputFile>>move)
		{
			moves.push_back(move);
		}
		inputFile.close();
		currentMoveIndex=0;
	}

	if(currentMoveIndex<moves.size())
	{
		int sth=GetValue(moves[currentMoveIndex++]);
		unsigned char temp_xo;
		read_bram(sth,&temp_xo,1);
		
		while(sth==0 || temp_xo!=' ')
		{
			cout<<"WARNING: Invalid move or position already taken. Taking the next available position."<<endl;
			sth=GetValue(moves[currentMoveIndex]);
		}

		if (sth !=0)
		{
			write_bram(sth,XO);
		}
		else
		{
			cout<<"ERROR: invalid move or position already taken."<<endl;
		}

	}

}

int Cpu::AIManager()
{
    //SC_REPORT_INFO("CPU", "AIManager started");
	float chance[2]={9999999 , 0 };
	for(int column=1; column<=7; column ++)
	{
		PlayOut=0;
		EVA=0;
		int PlayNumber=GetValue(column);

		if(PlayNumber!=0)
		{
			write_bram(PlayNumber,'O');
			get_ip();
			if(ip_result==2)
			{
				write_bram(PlayNumber,' ');
				return PlayNumber;
			}

			float temp=-(100*NegaMax(1));
			if(PlayOut !=0)
				temp -=((100*EVA)/PlayOut);
			if(-temp>=100)
				provocation=true;
			if(chance[0]>temp)
			{
				chance[0]=temp;
				chance[1]=PlayNumber;
			}
			write_bram(PlayNumber,' ');
		}
	}

	return chance[1];


}

/*int Cpu::NegaMax(int Depth)
{
	char XO;
	int PlayNumber[8]= {0,0,0,0,0,0,0,0}; 
    int chance=0;
    if(Depth % 2 != 0)
        XO='X';
    else
        XO='O';
    for(int column = 1 ; column <= 7 ; column ++)
    	 PlayNumber[column]=GetValue(column);
    for(int column = 1 ; column <= 7 ; column++)
    {
        if(PlayNumber[column] != 0)
        {
        	write_bram(PlayNumber[column],XO);
        	
        	get_ip();

        	if(ip_result!=0)
        	{
        		PlayOut ++;
        		if(XO=='O')
        			EVA ++;
        		else
        			EVA--;
        		write_bram(PlayNumber[column],' ');
        		return -1;
        	}
        	write_bram(PlayNumber[column],' ');

        }
    if(Depth<=6)
    {
    	for(int column=1; column<=7; column++)
    	{
    		int temp=0;
    		if(PlayNumber[column]!=0)
    		{
    			write_bram(PlayNumber[column],XO);

    			get_ip();
    			if (ip_result !=0)
    			{
    				PlayOut++;
    				if(XO=='O')
    					EVA++;
    				else
    					EVA--;
    				write_bram(PlayNumber[column], ' ');
    				return -1;
    			}
    			temp=NegaMax(Depth+1);
    			if(column==1)
    				chance=temp;
    			if(chance<temp)
    				chance=temp;
    			write_bram(PlayNumber[column], ' ');
    		}
    	}

    }    
    return -chance;
	}
}
*/
int Cpu::NegaMax(int Depth) {
    //SC_REPORT_INFO("CPU", "NEGAMAX started");
    char XO;
    int PlayNumber[8] = {0,0,0,0,0,0,0,0}; 
    int chance = 0;
    
    if (Depth % 2 != 0)
        XO = 'X';
    else
        XO = 'O';
        
    for (int column = 1; column <= 7; column++)
        PlayNumber[column] = GetValue(column);
        
    for (int column = 1; column <= 7; column++) {
        if (PlayNumber[column] != 0) {
            write_bram(PlayNumber[column], XO);
            get_ip();

            if (ip_result != 0) {
                PlayOut++;
                if (XO == 'O') EVA++;
                else EVA--;
                write_bram(PlayNumber[column], ' ');
                return -1;
            }
            write_bram(PlayNumber[column], ' ');
        }
    }

    if (Depth <= 6) {
        for (int column = 1; column <= 7; column++) {
            int temp = 0;
            if (PlayNumber[column] != 0) {
                write_bram(PlayNumber[column], XO);
                get_ip();
                if (ip_result != 0) {
                    PlayOut++;
                    if (XO == 'O') EVA++;
                    else EVA--;
                    write_bram(PlayNumber[column], ' ');
                    return -1;
                }
                temp = NegaMax(Depth + 1);
                if (column == 1)
                    chance = temp;
                if (chance < temp)
                    chance = temp;
                write_bram(PlayNumber[column], ' ');
            }
        }
    }
    
    // Dodajemo eksplicitni return na kraju funkcije
    return -chance;
}

// funkcije za komunikaciju za bram i hardom

void Cpu::write_bram(sc_uint<64> addr, unsigned char val)
{
	pl_t pl;	
	offset += sc_core::sc_time(DELAY , sc_core::SC_NS);	
	unsigned char buf;
	read_ddr_cnt++;
	buf = val;
	pl.set_address(VP_ADDR_BRAM_L + addr);
	pl.set_data_length(1); 
	pl.set_data_ptr(&buf);
	pl.set_command( tlm::TLM_WRITE_COMMAND );
	pl.set_response_status ( tlm::TLM_INCOMPLETE_RESPONSE );
	interconnect_socket->b_transport(pl, offset);
}


void Cpu::read_bram(sc_uint<64> addr,unsigned char *all_data, int length)
{
    //SC_REPORT_INFO("CPU", ("BRAM READ START - Addr: " + std::to_string(addr) +", Length: " + std::to_string(length)).c_str());
	
	offset += sc_core::sc_time((9+1) * DELAY,sc_core::SC_NS); 
	pl_t pl;
	unsigned char buf;
	int n=0;

	for(int i =0; i<length; i++)
	{
		write_ddr_cnt +=4;
		pl.set_address(VP_ADDR_BRAM_L +addr+i);
		pl.set_data_length(1);
		pl.set_data_ptr(&buf);
		pl.set_command(tlm::TLM_READ_COMMAND);
		pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
		interconnect_socket->b_transport(pl,offset);

		all_data[n]=buf;
      //  SC_REPORT_INFO("CPU", ("BRAM READ - Addr: " + std::to_string(addr + i) + ", Data: '" + std::string(1, buf) + "' (ASCII " + std::to_string((int)buf) + ")").c_str());

		n++;
	}

}
 
// proveri da li treba uint8_t mesto int kao tip funkcije, kao i neohodnu sisrinu buffera?
int Cpu::read_hard(sc_uint<64> addr)
{
    //SC_REPORT_INFO("CPU", ("HARD READ START - Addr: " + std::to_string(addr)).c_str());
	
	pl_t pl;
	unsigned char buf[8];
	pl.set_address(VP_ADDR_IP_HARD_L + addr);
	pl.set_data_length(1);
	pl.set_data_ptr(buf);
	pl.set_command(tlm::TLM_READ_COMMAND);
	pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	sc_core::sc_time offset=sc_core::SC_ZERO_TIME;
	interconnect_socket->b_transport(pl,offset);
	return toInt(buf);
}


void Cpu::write_hard(sc_uint<64> addr,int val)
{
   // SC_REPORT_INFO("CPU", ("HARD WRITE - Addr: " + std::to_string(addr) + ", Value: " + std::to_string(val)).c_str());
	
	pl_t pl;
	unsigned char buf[4];
	toUchar(buf,val); 	
    pl.set_address(VP_ADDR_IP_HARD_L + addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_WRITE_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    interconnect_socket->b_transport(pl,offset);


}