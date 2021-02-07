# exodussignup

Smart contract for creating EOS accounts for users of Exodus. EOS can be sent to this contract with a memo formatted as ```<account_name>-<publick_key>```, and the account `account_name` will be created and permissions for controlling the account will be assgined to `public_key`. For the account approximately 4k of RAM will be purchaed and some amount of EOS may be staked. The remaining EOS will be sent to the created account for immediate use.

## Set Up Environment

1. Install a fresh Ubuntu 18.04 LTS

2. Install `eosio v2.0.7` from https://github.com/EOSIO/eos/releases

3. Install `eosio.cdt v1.7.0` from https://github.com/EOSIO/eosio.cdt/releases

4. Create `contracts` directory somewherei and `cd contracts`

5. Download `eosio.contracts v1.9.0` from  https://github.com/EOSIO/eosio.contracts/releases

6. Unpack and prepare `eosio.contracts`:    
   ```
   tar xzf v1.9.0.tar.gz
   ln -s eosio.contracts-1.9.0 eosio.contracts
   ```

7. Clone the `exodussignup` repo: ```git clone git@github.com:ExodusMovement/exodussignup.git```

8. Build the contract:
   ```
   cd exodussingup
   make

   ```

9. Deploy the contract, assuming you have the `exodussignup` account set up:
   ```
   make deploy
   ```


## Prepare EOS wallet
```
# start the wallet daemon
keosd --wallet-dir $WALLET_DIR &

# create the default wallet
wallet  wallet create --to-console
Creating wallet: default
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
"PW5..."

# import private key for the active permission of the 'exodussignup' account
cleos wallet import --private-key "..."

# check public key in wallet
cleos wallet list keys
Wallets:
[
  "default *"
]
[
  "EOS6AtonxqWfiQwoBEUsdGf3BSoobTeeQGgvDQnL4WWfpvkWqBiX9",
]

# wallet will self-lock after a short period of inactivity, unlock with:
cleos wallet unlock
```

## Deploy and test contract

This assumes the account `exodussignup` is already created and has enough RAM for the contract and enough staked for uploading the contract. Staked EOS is not required for contract operation, but you need enough EOS staked for deploying the contract. Choose a node that works for you and set the NODE variable accordingly.
```
NODE=https://eu.eosdac.io
cleos -u $NODE set contract exodussignup exodussignup
cleos -u $NODE set account permission exodussignup active '{"threshold": 1,"keys": [{"key": "EOS6AtonxqWfiQwoBEUsdGf3BSoobTeeQGgvDQnL4WWfpvkWqBiX9","weight": 1}],"accounts": [{"permission":{"actor":"exodussignup","permission":"eosio.code"},"weight":1}]}' owner -p exodussignup
```

Test contract from your wallet. Choose an account name, that does not exists and verify at a block browser. Prepare a public/private key pair, so you have control of the resulting account. 
```
cleos wallet create_key
Created new private key with a public key of: "EOS7...WpM"
```

Form your wallet send enough EOS to 'exodussignup', with a memo like `<account_name>-<public-key>`. Or from an account with a key in your local wallet:
```
cleos -u $NODE transfer <my_account> exodussignup "3.0000 EOS" "account_name-public_key"
```


