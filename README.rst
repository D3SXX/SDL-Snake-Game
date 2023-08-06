==============
SDL 2 C++ Snake Game
==============

Introduction
------------

A simple snake game with a menu and scoreboard written in C++ using SDL2

Usage
-----

1. Visit the `releases page <https://github.com/D3SXX/SDL-Snake-Game/releases>`_ to find the latest version.
2. Download the preferred one and run it.

Building
--------

Dependencies
------------

C++ compiler

Snake game depends on the following libraries:

-- `Simple DirectMedia Layer (SDL) 2 <https://github.com/libsdl-org/SDL>`_

Follow the instructions below to build:

1. Clone/Download the repository.

.. code-block:: shell

    git clone https://github.com/D3SXX/SDL-Snake-Game.git

2. Go to the directory of desired version
3. Build the program:

- For Windows use 

.. code-block:: shell

  g++ -o snake.exe main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -static-libstdc++  

6. Launch the executable
