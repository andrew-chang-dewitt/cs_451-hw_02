# libcompart + conway's game of life

this repo contains:

- `./hello_compartment/`: the provided example using `libcompart`
- `./libcompart/`: the provided libcompart source, edited to ensure memory is
  freed
- `./life/`: the provided source code for Conways game of life, w/ minor edits
  to silence compiler warnings
- `./part/`: the compartmentalized version of `life` using `libcompart`
- `./scripts/`: some test scripts & a script (`./scripts/dev`) for generating
  the requested files

to generate requested files yourself, clone this repo, then run `sudo
./scripts/dev`. sudo required to allow compartments to drop privileges as
needed.

all requested files are also already generated & provided in the [latest
release](https://github.com/iltech-cs451-sp26/assignment-2-andrew-chang-dewitt/releases).
