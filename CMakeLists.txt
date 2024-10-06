cmake_minimum_required(VERSION 3.28)
project(CandyCrushConsoleEdition)

set(CMAKE_CXX_STANDARD 20)

add_executable(CandyCrushConsoleEdition main.cpp
        CandyCrushGame.h
        CandyCrushGame.cpp
        CandyMatrix.h
        CandyBase.h
        Candy.h
        Bomb.h
)
