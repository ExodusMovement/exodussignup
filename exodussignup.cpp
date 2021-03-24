//
// Created by Hongbo Tang on 2018/7/5.
// Modified by Ferenc Kiraly on 2018/10/15
// Rewritten by Ferenc Kiraly on 2020/03/13
//


#include "exodussignup.hpp"

void exodussignup::transfer(name from, name to, asset quantity, string memo) {
    if (from == _self || to != _self) return;
    if (from == eosio::name("eosio.stake"_n.value)) return;

    symbol core_symbol = eosiosystem::system_contract::get_core_symbol();
    check(quantity.symbol == core_symbol, "Only CORE symbol accepted");
    check(quantity.is_valid(), "Invalid token transfer");
    check(quantity.amount > 0, "Quantity must be positive");

    memo.erase(memo.begin(), find_if(memo.begin(), memo.end(), [](int ch) {
        return !isspace(ch);
    }));
    memo.erase(find_if(memo.rbegin(), memo.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), memo.end());

    auto separator_pos = memo.find(' ');
    if (separator_pos == string::npos) {
        separator_pos = memo.find('-');
    }
    check(separator_pos != string::npos, "Account name and public key must be separated by space or dash");

    string account_name_str = memo.substr(0, separator_pos);
    check(account_name_str.length() == 12, "Length of account name should be 12");
    name new_account_name = name{account_name_str};

    // Verify that the account does not exists
    check(!is_account(new_account_name), "Account already exists");

    string public_key_str = memo.substr(separator_pos + 1);
    check(public_key_str.length() == 53, "Length of public key should be 53");

    string pubkey_prefix("EOS");
    auto result = mismatch(pubkey_prefix.begin(), pubkey_prefix.end(), public_key_str.begin());
    check(result.first == pubkey_prefix.end(), "Public key should be prefixed with EOS");
    auto base58substr = public_key_str.substr(pubkey_prefix.length());

    vector<unsigned char> vch;
    check(decode_base58(base58substr, vch), "Decode pubkey failed");
    check(vch.size() == 37, "Invalid public key");

    array<unsigned char,33> pubkey_data;
    copy_n(vch.begin(), 33, pubkey_data.begin());
    checksum160 check_pubkey = ripemd160(reinterpret_cast<const char *>(pubkey_data.data()), 33);
    check(memcmp(check_pubkey.extract_as_byte_array().data(), &vch.end()[-4], 4) == 0, "Invalid public key");

    const int64_t max_ram_cost = 10000; // Maximum RAM cost
    const int64_t net_stake = 0;        // Amount to stake for NET [1/10 mEOS]
    const int64_t cpu_stake = 0;        // Amount to stake for CPU [1/10 mEOS]
    const uint32_t bytes = 4096;        // Number of bytes of RAM to buy for the created account
    const uint32_t bytes_self = 240;    // Number of bytes of RAM to buy for exodussignup to reimburse for lost RAM and make exodussignup self-sufficient

    asset buy_ram_total = eosiosystem::ramcost(bytes + bytes_self, core_symbol);
    check(buy_ram_total.amount <= max_ram_cost, "RAM currently too expensive, try again later");
    asset buy_ram = buy_ram_total * bytes / (bytes + bytes_self);
    asset buy_ram_self = buy_ram_total - buy_ram;

    // print(" ####Exodussignup buy_ram_total: ", buy_ram_total.amount);
    // print(" ####Exodussignup buy_ram: ", buy_ram.amount);
    // print(" ####Exodussignup buy_ram_self: ", buy_ram_self.amount);

    asset stake_net(net_stake, core_symbol);
    asset stake_cpu(cpu_stake, core_symbol);
    asset liquid = quantity - stake_net - stake_cpu - buy_ram_total;
    check(liquid.amount > 0, "Not enough balance to buy ram");

    signup_public_key pubkey = {
         .type = 0,
         .data = pubkey_data,
    };
    key_weight pubkey_weight = {
        .key = pubkey,
        .weight = 1,
    };
    authority owner = authority{
        .threshold = 1,
        .keys = { pubkey_weight },
        .accounts = {},
        .waits = {}
    };
    authority active = authority{
        .threshold = 1,
        .keys = { pubkey_weight },
        .accounts = {},
        .waits = {}
    };

    action(
        permission_level{ _self, "active"_n },
        "eosio"_n,
        "newaccount"_n,
        std::make_tuple(_self, new_account_name, owner, active) 
    ).send();

    action(
        permission_level{ _self, "active"_n },
        "eosio"_n,
        "buyram"_n,
        std::make_tuple(_self, new_account_name, buy_ram) 
    ).send();

    action(
        permission_level{ _self, "active"_n },
        "eosio"_n,
        "buyram"_n,
        std::make_tuple(_self, _self, buy_ram_self) 
    ).send();
 
    if(stake_net.amount > 0 || stake_cpu.amount > 0) {
        action(
            permission_level{ _self, "active"_n },
            "eosio"_n,
            "delegatebw"_n,
            std::make_tuple(_self, new_account_name, stake_net, stake_cpu, true) 
       ).send();
    }

    action(
        permission_level{ _self, "active"_n },
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(_self, new_account_name, liquid, std::string("liquid balance"))
    ).send();
}
