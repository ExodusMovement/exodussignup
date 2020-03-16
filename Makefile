CONTRACTS_PREFIX = ../eosio.contracts-1.9.0/contracts
CLEOS = $(HOME)/bin/cleos-jungle.sh
CONTRACT = exodussignup
DEPS = $(CONTRACT).hpp ramcost.hpp
SOURCES = $(CONTRACT).cpp ramcost.cpp
WASM = $(CONTRACT).wasm
INCLUDES = -I$(CONTRACTS_PREFIX)/eosio.system/include -I$(CONTRACTS_PREFIX)/eosio.token/include

build: $(CONTRACT).wasm

clean:
	rm -f $(CONTRACT).wasm $(CONTRACT).abi

$(WASM): $(SOURCES) $(DEPS)
	eosio-cpp $(SOURCES) -o $@ --abigen $(INCLUDES)

deploy: $(WASM)
	$(CLEOS) set contract $(CONTRACT) ../$(CONTRACT)

test:
	$(CLEOS) transfer ferijungle42 $(CONTRACT) "1.0000 EOS" "barasherfood-EOS5MqQPx2BzerwwKkdXkihzTZREcteMxzgo5EBQGA5RzdBZjq4NQ"

unlock:
	$(CLEOS) wallet unlock

.PHONY: deploy clean
