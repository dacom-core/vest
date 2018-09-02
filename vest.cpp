#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/print.hpp>
#include "vest.hpp"
using namespace eosio;

struct impl {
	
	void make_vesting_action(const startvest &op){
    require_auth(op.sender);
		eosio_assert(op.amount.is_valid(), "Amount not valid");
    eosio_assert(op.amount.symbol == _SYM, "Not valid symbol for this vesting contract");
    eosio_assert(is_account(op.owner), "Owner account does not exist");
    eosio_assert(op.duration > 0 && op.duration < 315400000, "Duration must be more then 0 and less then 315400000");

    vesting_index vests (_self, op.owner);

    action(
            permission_level{ op.sender, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( op.sender, _self, op.amount, std::string("")) 
    ).send();

    vests.emplace(op.sender, [&](auto &v){
      v.id = vests.available_primary_key();
      v.owner = op.owner;
      v.amount = op.amount;
      v.startat = eosio::time_point_sec(now());
      v.duration = op.duration;
    });

	}

  void refresh_action(const refresh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    
    if (eosio::time_point_sec(now() ) > v->startat){
      
      auto elapsed_seconds = (eosio::time_point_sec(now()) - v->startat).to_seconds();
      eosio::asset available;
    
      if( elapsed_seconds < v->duration){
        available = v->amount * elapsed_seconds / v->duration;
      } else {
        print(v->amount);
        available = v->amount;
      
      }
      available = available - v->withdrawed;
      vests.modify(v, op.owner, [&](auto &m){
        m.available = available;
      });

    }
  }

  void withdraw_action(const withdraw &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    eosio_assert((v->available).amount > 0, "Only positive amount can be withdrawed");
    if (v->withdrawed + v->available == v->amount){
      vests.erase(v);

    } else {

      
    action(
            permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( _self, op.owner, v->available, std::string("Vesting Withdrawed")) 
    ).send();

    vests.modify(v, op.owner, [&](auto &m){
        m.withdrawed = v->withdrawed + v->available;
        m.available = eosio::asset(0, _SYM);
      });
    };
    
  };

	void apply( uint64_t /*receiver*/, uint64_t code, uint64_t action ) {
      if (code == _self) {

        if (action == N(refresh)){
          refresh_action(eosio::unpack_action_data<refresh>());
        }

        if (action == N(withdraw)){
          withdraw_action(eosio::unpack_action_data<withdraw>());
        }

        if (action == N(startvest))
     	  {
        	make_vesting_action(eosio::unpack_action_data<startvest>());
     	  } 
      }

  };
};

extern "C" {
   
   /// The apply method implements the dispatch of events to this contract
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
      impl().apply(receiver, code, action);
   };

}; 