echo "Setting up sputter environment"

# Environment prereqs
if [ -z "$SPUTTER_ROOT" ]; then
    echo -e "SPUTTER_ROOT not defined"
    exit -1
fi

# Sputter
export SPUTTER_BUILD_ROOT=$SPUTTER_ROOT/build
export SPUTTER_VERBOSITY=Info

# PaddleArena
export PADDLE_ARENA_ASSETS_ROOT=$HOME/gamedev/assets/paddlearena

