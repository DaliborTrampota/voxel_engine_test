# Repozitář pro testování voxelového enginu na diplomovou práci
- Autor: Bc. Dalibor Trampota
- Škola: ČVUT FIT
- Předpokládaný rok dokončení studia: 2025/26


## Prerekvizity

- CMake v3.10+
- Podporované/testované kompilátory: MSVC, Clang, GCC
- Podporované/testované platformy: Windows, Linux (Wayland/Ubuntu)
- Doporučené IDE pro nejjednodušší start: Visual Studio 2022


## Getting started

```
git submodule init
git submodule update --init --recursive
```

Tyto kroky lze nahradit spuštěním skriptu `start.bat -conf`
```
mkdir build
cmake --preset vs2022 # nebo ninja
```

- Voxelový engine je ve složce `voxel_engine` spolu s 3P knihovnami
- Testovací aplikace musí být naprogramovaná v adresáři `src`
- Doxygen dokumentace je dostupná na adrese https://dalibortrampota.github.io/voxel_engine/
- Zdrojové soubory enginu se includují následovně
 - `#include <render/Engine.h>`
 - `#include <level/Chunk.h>`
- Všechny třídy voxelového enginu jsou v namespace `engine`
- Všechny třídy z OpenGL abstrakční knihovny LWGL jsou v namespace `gl`


