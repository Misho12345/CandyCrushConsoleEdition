# Cross-platform Candy Crush clone in the terminal

> **Note:** at least C++20 is required

> **Note:** On Windows it's significantly slower and shape symbols can't be used, so it's recommended to play on Linux.

To build the project, run the following commands:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## How to play
- Find the exetutable in the `build` directory and run it.
- Put the terminal in full screen and if needed increase the font size.
- Move with `←`/`→`/`↑`/`↓` or with `W`/`A`/`S`/`D` keys.
- To swap two candies, press `Space` to enter `swap-mode`. Then wherever you move, the candy will be swapped with the candy at that position.
- To exit, go to the row below the last row that has `Quit` written on it and press `Enter`.