#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#define SC_INCLUE_FX
#include<tlm>
#include<sysc/datatypes/fx/sc_fixed.h>

typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm::tlm_base_protocol_types::tlm_phase_type ph_t;

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace sc_dt;

//registers in hard
#define ADDR_START 0x00
#define ADDR_READY 0x01
#define ADDR_WIN_VAL 0x02

//bram size 
#define BRAM_SIZE 0x3A980 //BRAM size 240KB

# define DELAY 10 //macro for offser (Delay =T=1/f=1/100Mhz=10ns)


//AXI&BRAM
#define VP_ADDR_BRAM_L 0x00000000 
#define VP_ADDR_BRAM_H 0x00000000 + BRAM_SIZE 

#define VP_ADDR_IP_HARD_L 0x40000000
#define VP_ADDR_IP_HARD_H 0x4000000F

#endif// TYPEDEFS_HPP