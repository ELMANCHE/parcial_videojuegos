# Squid Games - Arena 3D

Juego de arena 3D isométrica con personajes estilo Roblox. Los jugadores deben esquivar pelotas que caen del cielo mientras mantienen su vida.

## Requisitos

- C++17 o superior
- OpenCV 4.x
- make
- pkg-config

## Instalación de Dependencias

### macOS
```bash
brew install opencv pkg-config
```

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential pkg-config libopencv-dev
```

## Compilación

```bash
make arena
```

## Ejecución

```bash
./arena
```

## Controles

### Jugador 1
- W/A/S/D: Mover
- Q: Saltar

### Jugador 2
- I/J/K/L: Mover
- U: Saltar

### General
- +/-: Zoom
- ESC: Salir

## Características

- Arena 3D isométrica con texturas
- 2 jugadores con personajes estilo Roblox
- 30 pelotas con física realista
- Sistema de vida (100 HP inicial, -10 HP por golpe)
- Efecto visual de muerte
- Texturas en paredes y piso

## Estructura del Proyecto

```
parcial_videojuegos/
├── arena_isometrica.cpp  # Código principal del juego
├── juego3d.cpp           # Código de referencia
├── juegoroblox.cpp       # Código de referencia
├── Makefile              # Archivo de compilación
└── sources/              # Texturas del juego
    ├── images-5.jpeg
    ├── images-6.jpeg
    └── piso.jpeg
```
