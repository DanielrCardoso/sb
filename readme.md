## Compilando
### Opção 1: Usando CMake
```
sudo apt install cmake
```

```
cmake .
```

```
make 
```

```
./montador 
```

### Opção 2: Compilação Convencional
```
g++ -o montador montador.cpp src/InstructionsUtils.cpp src/StringFileUtils.cpp
```
```
./montador 
```