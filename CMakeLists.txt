cmake_minimum_required(VERSION 3.29)
project(CandyCrushConsoleEdition)

set(CMAKE_CXX_STANDARD 23)

add_executable(CandyCrushConsoleEdition main.cpp
        CandyCrushGame.h
        CandyCrushGame.cpp
        CandyMatrix.h
        CandyBase.h
        Candy.h
        Bomb.h
)
