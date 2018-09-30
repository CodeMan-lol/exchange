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

#define SN(X) (string_to_symbol(0, #X) >> 8)

class exchange : public contract {
  public:
    using contract::contract;

    //@abi action
    void bid(account_name maker, asset quantity, uint64_t price, asset bid_currency, account_name bid_contract, account_name source);

    //@abi action
    void ask(account_name maker, asset quantity, uint64_t price, account_name ask_contract, account_name source);

    //@abi action
    void cancelorder(account_name scope, uint64_t order_id);

    account_name settlement_token_contract = N(eosio.token);
    symbol_type settlement_token_symbol = S(4, EOS);

  private:
    //@abi table
    struct orders {
      uint64_t id;
      uint64_t price;
      asset quantity;
      account_name maker;
      account_name contract;
      account_name source;
      uint64_t primary_key() const { return id; }
      uint64_t get_price() const { return price; }
      EOSLIB_SERIALIZE(orders, (id)(price)(quantity)(maker)(contract)(source))
    };

    typedef multi_index<N(orders), orders,
            indexed_by<N(byprice), const_mem_fun<orders, uint64_t, &orders::get_price> >
              > order_index;

    void add_order(uint64_t scope, account_name maker, asset quantity, uint64_t price, account_name contract,account_name source);

    struct account {
        asset    balance;
        uint64_t primary_key()const { return balance.symbol.name(); }
    };
    typedef multi_index<N(accounts), account> accounts;

    asset get_balance(account_name owner, account_name contract = N(eosio.token), symbol_name sym = SN(EOS)) const
    {
        accounts accountstable(contract, owner);
        auto ac = accountstable.find(sym);
        if (ac == accountstable.end()) {
            return asset();
        }
        return ac->balance;
    }


    void deposit(account_name contract, account_name user, asset quantity);
    void withdraw(account_name contract, account_name user, asset quantity);
    void transfer(account_name contract, account_name from, account_name to, asset quantity);
    void validate_bid_balance(account_name maker, asset quantity, uint64_t price);
    void validate_ask_balance(account_name maker, asset quantity, account_name contract);

    asset to_settlement_token(asset quantity, uint64_t price, bool floor);
    asset asset_min(asset a, asset b);
};

