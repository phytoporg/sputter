# sputter
Small game engine with a focus on small footprint and deterministic simulation.

## Building

### Dependencies
- `GLEW`
- `GLFW 3.*`
- `Google Log 0.4.*`
- `GTest`
- `zlib`
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

### Running test program

```
$ ./src/exe/sandbox/sandbox
Usage: ./src/exe/sandbox/sandbox <asset_path> <test image asset>
```
Point to an `asset` directory. 
Sputter will recursively load all textures in that directory.
Textures must be `.png` files.
Test image asset should be name of file without extension.


## Structure

- `src/lib/sputter` - main lib
  - `assets` - asset management; loading assets
  - `core` - basic core components
    - Subsystem - a system that manages a particular type of component
      - e.g. rigid body subsystem for kinematics
      - Subsystems are responsible for component lifecycle
        - create, update (tick), destroy
  - `render` - sprite stuff!
    - Sprite - renders a texture at a screenspace coordinate.
    - SpriteBatch - any sprites that render the same texture should use this
    - Texture - tells OpenGL about texture
    - TextureStorage - stores created textures (should create too, eventually)
    - Window - oh yeah, standard window stuff. Handles inputs

  - `system` - OS specific stuffs + logging
