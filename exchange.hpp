/**
 * simple decentralized exchange
 * smart contract on eos
 *
 */

#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>

using namespace eosio;

class exchange : public contract {
  public:
    using contract::contract;

    //@abi action
    void bid(account_name maker, asset quantity, uint64_t price);

    //@abi action
    void ask(account_name maker, asset quantity, uint64_t price);

    //token contract is not eosio.token it is the op
//    account_name settlement_token_contract = N(eosio.token);
    account_name settlement_token_contract = N(onedexchange);
    symbol_type settlement_token_symbol = S(4, EOS);

    account_name exchange_token_contract = N(onedechange);
//    account_name exchange_token_contract = N(eosio.token);
    symbol_type exchange_token_symbol = S(3, SYS);

  private:
    //@abi table
    struct orders {
      uint64_t id;
      uint64_t price;
      asset quantity;
      account_name maker;
      uint64_t primary_key() const { return id; }
      uint64_t get_price() const { return price; }
      EOSLIB_SERIALIZE(orders, (id)(price)(quantity)(maker))
    };

    typedef multi_index<N(orders), orders,
            indexed_by<N(byprice), const_mem_fun<orders, uint64_t, &orders::get_price> >
              > order_index;

    void add_order(uint64_t scope, account_name maker, asset quantity, uint64_t price);

    void deposit(account_name contract, account_name user, asset quantity);
    void withdraw(account_name contract, account_name user, asset quantity);
    void transfer(account_name contract, account_name from, account_name to, asset quantity);

    asset to_settlement_token(asset quantity, uint64_t price, bool floor);
    asset asset_min(asset a, asset b);
};

