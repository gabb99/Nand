//
//  ram.hpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef ram_hpp
#define ram_hpp

#include "decoder.hpp"
#include "register.hpp"
#include "decoder.hpp"

template <unsigned N = 2>
class ram_t
{
	registr_t<N> m_memory_adress_register;

public:
	ram_t() {}
};

#endif /* ram_hpp */
