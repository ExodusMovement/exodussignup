//
// Written by Ferenc Kiraly on 2020/03/13
//

#include <eosio.system/eosio.system.hpp>

namespace eosiosystem {

  int64_t get_bancor_input( int64_t out_reserve, int64_t inp_reserve, int64_t out );
  
  asset ramcost( uint32_t bytes, symbol core_symbol );

}
