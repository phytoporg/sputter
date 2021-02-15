# sputter
Small game engine

## Building

### Dependencies
- `GLFW 3.*`
- `Google Log 0.4.*`

```
cmake -S .
make
```

### Running test program

```
$ ./src/exe/test_rendering/test_rendering
Usage: ./src/exe/test_rendering/test_rendering <asset_path> <test image asset>
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
