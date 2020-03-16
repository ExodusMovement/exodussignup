#include <eosio.system/eosio.system.hpp>

namespace eosiosystem {

  using eosio::asset;

  int64_t get_bancor_input(
    int64_t out_reserve,
    int64_t inp_reserve,
    int64_t out )
  {
    const double ob = out_reserve;
    const double ib = inp_reserve;

    int64_t inp = (ib * out) / (ob - out);

    if ( inp < 0 ) inp = 0;

    return inp;
  }
  
  asset ramcost( uint32_t bytes, symbol core_symbol ) {
    name system_account = "eosio"_n;
    rammarket _rammarket(system_account, system_account.value);
    auto itr = _rammarket.find(system_contract::ramcore_symbol.raw());
    const int64_t ram_reserve   = itr->base.balance.amount;
    const int64_t eos_reserve   = itr->quote.balance.amount;
    const int64_t cost          = get_bancor_input( ram_reserve, eos_reserve, bytes );
    const int64_t cost_plus_fee = cost / double(0.995);
    return asset{ cost_plus_fee, core_symbol };
  }

}