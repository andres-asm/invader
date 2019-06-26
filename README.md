[![pipeline status](https://git.retromods.org/dev/invader/badges/master/pipeline.svg)](https://git.retromods.org/dev/invader/commits/master)

# invader

invader is an experimental implementation of the libretro API.

It's split in two parts:
- piccolo as the libretro player implementation
- kami as the GUI frontend, currently it uses nuklear, and has both SDL2 and GLFW3 implementations

As for the goals, nothing concrete so far, we'll see how it goes.

# Compilation

```
git clone git@git.retromods.org:dev/invader.git
cd invader
git submodule update --init
make -f Makefile.sdl2
```
# Current Progress
## Backend
- [X] core loading
- [X] core initialization
- [ ] content loading
- [X] video rendering
- [ ] audio rendering
- [ ] input processing

## Frontend
- [ ] everything

## Screenshots

### 0.1.0
![img](docs/images/0.1.0.png)

### 0.1.1
![img](docs/images/0.1.1.png)

### 0.1.2
![img](docs/images/0.1.2.png)
