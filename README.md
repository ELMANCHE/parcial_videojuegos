# Angry Birds OpenCV - Parcial Videojuegos

Un juego tipo Angry Birds implementado en C++ usando la librería OpenCV.

## Características
- 3 niveles con diferentes dificultades (Fácil, Intermedio, Difícil)
- Sistema de física realista con gravedad y colisiones
- Controles intuitivos con mouse
- Sistema de puntuación
- Gráficos 2D usando OpenCV
- **NUEVO**: Rango de tiro mejorado (200px)
- **NUEVO**: Velocidad de lanzamiento optimizada (200px/s)
- **NUEVO**: Previsualización de trayectoria más precisa

## Compilación
```bash
# Opción 1: Script automático
./build.sh

# Opción 2: Manual
mkdir build
cd build
cmake ..
make
```

## Ejecutar el Juego
```bash
# Opción 1: Script de juego (recomendado)
./play.sh

# Opción 2: Ejecutar directamente
cd build
./AngryBirdsOpenCV
```

## Controles
- **Mouse**: Apuntar y disparar el pájaro desde la catapulta
- **Spacebar**: Reiniciar nivel
- **N**: Siguiente nivel
- **ESC**: Salir del juego

## Niveles
1. **Fácil**: Pocos obstáculos y cerdos
2. **Intermedio**: Estructuras más complejas
3. **Difícil**: Múltiples cerdos y obstáculos desafiantes
