# TODO: Check environment
echo "Setting up sputter aliases"

# Building
alias bsput="pushd $SPUTTER_ROOT > /dev/null; ./scripts/build_linux.sh; popd > /dev/null"

# Executing
alias egame="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT; popd > /dev/null"
alias egamecli="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddlearena/paddlearena $PADDLE_ARENA_ASSETS_ROOT --p2p-client $1 --log-verbosity $SPUTTER_VERBOSITY; popd > /dev/null"
alias egamesrv="pushd $SPUTTER_ROOT > /dev/null; ./build/src/exe/paddleserver/paddleserver; popd > /dev/null"
