//
// Created by Hongbo Tang on 2018/7/5.
// Modified by Ferenc Kiraly on 2018/10/15
// Rewritten by Ferenc Kiraly on 2020/03/13
//

#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>
#include <cstring>
#include <cmath>

#include "ramcost.hpp"
#include "base58.hpp"

using namespace eosio;
using namespace std;

using eosio::name;

class  [[eosio::contract]] exodussignup: public contract {
public:
    using contract::contract;

    [[eosio::action]] 
    void transfer(name from, name to, asset quantity, string memo);

private:
    struct signup_public_key {
        uint8_t        type;
        array<unsigned char,33> data;
    };
    struct permission_level_weight {
        permission_level permission;
        uint16_t weight;
    };
    struct key_weight {
        signup_public_key key;
        uint16_t weight;
    };
    struct wait_weight {
        uint32_t wait_sec;
        uint16_t weight;
    };
    struct authority {
        uint32_t threshold;
        vector<key_weight> keys;
        vector<permission_level_weight> accounts;
        vector<wait_weight> waits;
    };
};

extern "C" {
    void apply(uint64_t receiver, uint64_t code, uint64_t action) { // receiver is me (exodussignup)
        check(code != receiver, "Invalid receiver"); // must not be my code
        if (code == "eosio.token"_n.value && action == "transfer"_n.value) {
            eosio::execute_action( eosio::name(receiver), eosio::name(code), &exodussignup::transfer );
        }
        // If code == 'tethertether' then we allow the transfer to continue. Maybe we should fail?
    }
}



