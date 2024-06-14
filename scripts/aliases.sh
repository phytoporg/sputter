# Building
alias bsput="pushd $SPUTTER_ROOT &> /dev/null; ./scripts/build_linux.sh; popd &> /dev/null"
alias bsputc="pushd $SPUTTER_ROOT &> /dev/null; ./scripts/build_linux.sh; popd &> /dev/null"
alias bsputd="pushd $SPUTTER_ROOT &> /dev/null; ./scripts/build_linux.sh --debug --clean; popd &> /dev/null"
alias bsputdc="pushd $SPUTTER_ROOT &> /dev/null; ./scripts/build_linux.sh --debug --clean; popd &> /dev/null"

# Game
alias egame="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT; popd > /dev/null"
alias egamecli="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT --p2p-client $1 --log-verbosity $SPUTTER_VERBOSITY; popd > /dev/null"
alias egamed="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT; popd > /dev/null"
alias egamedcli="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT --p2p-client $1 --log-verbosity $SPUTTER_VERBOSITY; popd > /dev/null"
# Server
alias egamesrv="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddleserver/paddleserver; popd > /dev/null"
alias egamedsrv="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddleserver/paddleserver; popd > /dev/null"

