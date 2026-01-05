# aniworld-cli

> [!IMPORTANT]
> Very early project, full of bugs.

Watch aniworld from CLI.

## Build:
```
gcc aniworld-cli.c -o aniworld-cli -lssl -lcrypto
```
or
```
gcc nob.c -o nob
./nob
```

## Usage:
```
Start new watch:
  aniworld-cli "Serial Experiments Lain"


Choose specific staffel and episode
  aniworld-cli "One Punch Man" 1 5


Watch the movie
  aniworld-cli "Watamote" 0

aniworld-cli watch      show [staffel] [episode]
aniworld-cli watch-url  show [staffel] [episode]
aniworld-cli search     show
```
