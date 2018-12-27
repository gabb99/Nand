//
//  cpu.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef cpu_hpp
#define cpu_hpp

#include "alu.hpp"
#include "control.hpp"
#include "ram.hpp"

template <unsigned N = 8>
class cpu_t
{
	alu_t<N> m_alu;
	control_t<N> m_control;
	ram_t<N> m_ram;

public:
	cpu_t() {}
};

#endif /* cpu_hpp */
