cmake_minimum_required(VERSION 3.28)
project(CandyCrushConsoleEdition)

set(CMAKE_CXX_STANDARD 20)

add_executable(CandyCrushConsoleEdition main.cpp
        CandyCrushGame.h
        CandyCrushGame.cpp
        CandyMatrix.h
        CandyMatrix.cpp
        items/Item.h
        items/Candy.h
        items/Candy.cpp
        items/Bomb.h
        items/Bomb.cpp
        utils/Random.h
        utils/Random.cpp
        utils/macros.h
        utils/print.h
        utils/structs.h
)
