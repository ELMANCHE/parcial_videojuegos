# Squid Games - Arena 3D

Juego de arena 3D isométrica con personajes estilo Roblox inspirado en el Juego del Calamar.

## 🎮 Flujo del Juego

```
MENÚ → ARENA (60s) → GLASS BRIDGE → GAME OVER → Reinicio
```

### Flujo Completo:
1. **Menú Principal**: Click en "JUGAR" para comenzar
2. **Arena Isométrica** (60 segundos): Esquiva pelotas, sobrevive
   - Si ambos mueren → Game Over
   - Si 1 o 2 sobreviven → Glass Bridge
3. **Glass Bridge**: Cruza el puente de vidrio evitando trampas
   - Victoria o muerte → Game Over
4. **Game Over**: Click "JUGAR DE NUEVO" → Vuelve a Arena

## Niveles Disponibles

### 🎯 Menú Principal (`menu`)
Pantalla inicial con fondo de Squid Game
- **JUGAR**: Inicia el juego en Arena Isométrica
- **SALIR**: Cierra el juego

### ⏱️ Arena Isométrica (`arena`) - Nivel 1
Los jugadores deben esquivar 30 pelotas que caen del cielo durante **60 segundos**.

**Características:**
- Timer visible de 60 segundos (esquina superior derecha en rojo)
- Sistema de vida (100 HP por jugador)
- 30 pelotas con física realista
- Vista isométrica 3D con texturas

**Objetivo:** Sobrevivir 60 segundos para pasar al siguiente nivel

### 🌉 Glass Bridge Challenge (`arena2`) - Nivel 2 🆕
Nivel inspirado en el desafío del puente de vidrio del Juego del Calamar. Los jugadores deben avanzar por un puente de 2 columnas x 6 filas, donde una casilla por fila es una trampa mortal.

**Características visuales:**
- Cuadrículas grises simulando vidrio
- Contorno negro grueso en cada tile
- Efecto de brillo en vidrios no revelados
- Cambio de color al revelar: verde (segura) o rojo (trampa)

**Reglas:**
- Solo puedes avanzar hacia adelante (no retroceder)
- Puedes moverte recto o en diagonal
- Si pisas una trampa, mueres instantáneamente
- El primer jugador en llegar al final gana
- Victoria o muerte → Pantalla Game Over

### 🔄 Game Over (`end`)
Pantalla de finalización con opciones:
- **JUGAR DE NUEVO**: Reinicia en Arena Isométrica
- **SALIR**: Cierra el juego

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
# Compilar todos los juegos
make all

# O compilar individualmente
make arena    # Arena original
make arena2   # Glass Bridge
make menu     # Menú principal
make end      # Pantalla de game over
```

## Ejecución

```bash
# Iniciar el juego completo desde el menú (RECOMENDADO)
./menu

# O ejecutar niveles individuales para pruebas
./arena    # Arena Isométrica (60s)
./arena2   # Glass Bridge Challenge
./end      # Pantalla Game Over
```

## Controles

### Menú y Game Over
- **Mouse**: Hover sobre botones y click para seleccionar

### Arena Isométrica (arena) - Nivel 1

#### Jugador 1
- **W**: Adelante
- **S**: Atrás  
- **A**: Izquierda
- **D**: Derecha
- **Q**: Saltar

#### Jugador 2
- **I**: Adelante
- **K**: Atrás
- **J**: Izquierda
- **L**: Derecha
- **U**: Saltar

### Glass Bridge (arena2) - Nivel 2

#### Jugador 1
- **W**: Avanzar recto
- **A**: Diagonal izquierda-adelante
- **D**: Diagonal derecha-adelante

#### Jugador 2
- **I**: Avanzar recto
- **J**: Diagonal izquierda-adelante
- **L**: Diagonal derecha-adelante

### Controles Generales (todos los niveles)
- **+/-**: Zoom in/out
- **Mouse (arrastrar)**: Mover cámara
- **ESC**: Salir del juego actual

## 🎯 Características Destacadas

### Timer de 60 Segundos
- Contador visible en rojo en la esquina superior derecha
- Al terminar el tiempo:
  - **Con sobrevivientes** → Pasa a Glass Bridge
  - **Sin sobrevivientes** → Game Over

### Grid Mejorado en Glass Bridge
- **Cuadrículas grises** simulando vidrio templado
- **Contorno negro** grueso en cada tile
- **Efecto de brillo** en vidrios no revelados
- **Colores de estado**:
  - Gris: No revelado
  - Verde: Seguro (después de pisar)
  - Rojo: Trampa rota (muerte)

### Sistema de Transiciones
- Flujo automático entre niveles
- Transiciones suaves con mensajes
- Reinicio inteligente al Game Over

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
