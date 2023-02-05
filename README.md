# sputter
A game engine with a focus on small footprint and deterministic simulation.

The project didn't quite start with a "small footprint" goal in mind so there's going to be some work to do there once there's an actual, shippable game in place. In the meantime, I'm doing my best not to introduce additional third-party dependencies.

"Deterministic simulation" means to allow for identical memory representations for game state across all supported platforms, and identical outcomes for each time step given a set of player inputs. I'm looking to use sputter to create multiplayer-first games with latency-hiding rollback netcode at its core.

# PaddleArena
PaddleArena is the in-progress game currently driving sputter's development. For all intents and purposes, sputter is just an attempt to separate reusable pieces from the core game logic in PaddleArena. For now, anyway.

Currently, PaddleArena is just a Pong clone, but the intent is to explore fighting game elements in a Pong-like format which allow for meaningful interactions between two human opponents.

## Building

### Dependencies
At some point I intend to whittle away at this list, but for the moment, here it is:

- `GLEW`
- `GLFW 3.*`
- `Google Log 0.4.*`
- `GTest`
- `libpng`

In Debian:
```
libglew-dev libglfw3-dev libgoogle-glog-dev googletest libgtest-dev libglm-dev
```

To build, execute from the source tree root:
```
mkdir build
cmake -S . -Bbuild
cmake --build build -- -j8
```

There is a script for building in Windows under ./scripts, which mostly stems from an awkward (in Windows) list of dependencies. I attempt to maintain it but the vast majority of my time is spent working in Linux so it'll be broken... often. 

This'll hopefully get better when I get around to pruning external dependencies.

### Running PaddleArena

```
$ ./build/src/exe/paddlearena/paddlearena
Usage: ./build/src/exe/paddlearena/paddlearena <asset_path>
```
Point to an `asset` directory. Assets aren't yet checked in, but SoOn !!

## Structure

- `src/lib/sputter` - main lib
  - `assets` - asset management; loading assets
  - `containers` - fixed-memory container implementations
  - `game` - ECS goo, game object and scene management, etc.
  - `input` - device discovery, input mappings and such
  - `math` - vectors and matrices of various types, useful constants, helper functions
  - `net` - p2p network communication
  - `memory` - custom allocators
  - `physics` - fixed-timestep simulation using fixed-point math
  - `core` - commonly-used classes and functionality across sputter
  - `render` - all of the rendering things
  - `system` - platform-specific abstractions + logging
  - `ui` - a very rudimentary ui
- src/exe/...
  -  where the games and experiments live
## Style

- Member variables use `m_` prefix.
- Default values for classes go in the header.
- Logging should be done through `glog` (include `#include <sputter/system/system.h` and look for `LOG` for examples)
- Invalid input should be checked with `RELEASE_CHECK(bool, error message)`
