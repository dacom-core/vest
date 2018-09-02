
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/contract.hpp>


namespace eosio {
    static const account_name _self = N(tt.tc);
	static const eosio::symbol_name _SYM = S(4, FLO);


    //@abi table vesting i64 
    struct vesting{
    	uint64_t id;
    	account_name owner;
    	eosio::time_point_sec startat;
    	uint64_t duration;
    	eosio::asset amount;
    	eosio::asset available;
    	eosio::asset withdrawed;

    	uint64_t primary_key() const {return id;}

    	EOSLIB_SERIALIZE(vesting, (id)(owner)(startat)(duration)(amount)(available)(withdrawed))
    };

    typedef eosio::multi_index<N(vesting), vesting> vesting_index;


    //@abi action startvest
    struct startvest {
    	account_name sender;
    	account_name owner;
    	eosio::asset amount;
    	uint64_t duration;

    	EOSLIB_SERIALIZE(startvest, (sender)(owner)(amount)(duration))

    };

    //@abi action refresh
    struct refresh {
    	account_name owner;
    	uint64_t id;

    	EOSLIB_SERIALIZE(refresh, (owner)(id))

    };

    //@abi action withdraw
    struct withdraw{
    	account_name owner;
    	uint64_t id;
     	EOSLIB_SERIALIZE(withdraw, (owner)(id))
    };



}