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
  aniworld-cli "One Punch Man"


Choose specific staffel & episode
  aniworld-cli "Watamote" 1 5


Watch the movie
  aniworld-cli "Watamote" 0

aniworld-cli watch      show [staffel] [episode]
aniworld-cli watch-url  show [staffel] [episode]
aniworld-cli search     show
```
