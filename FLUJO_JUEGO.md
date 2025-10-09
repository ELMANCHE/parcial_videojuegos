# 🎮 Flujo del Juego - Squid Games

## Diagrama de Flujo

```
┌─────────────┐
│   MENU      │  ← Pantalla inicial
│ (menu.cpp)  │
└──────┬──────┘
       │ Click en "JUGAR"
       ↓
┌─────────────────────────────┐
│  ARENA ISOMÉTRICA           │  ← Nivel 1 (60 segundos)
│  (arena_isometrica.cpp)     │
│                             │
│  • Esquivar pelotas         │
│  • Timer: 60 segundos       │
│  • 2 jugadores              │
└──────┬──────────────┬───────┘
       │              │
       │              │ Ambos muertos antes de tiempo
       │              └──────────────┐
       │                             ↓
       │ Tiempo terminado         ┌─────────────┐
       │ + 1 o 2 vivos            │  GAME OVER  │
       ↓                          │  (end.cpp)  │
┌─────────────────────────────┐ └──────┬──────┘
│  GLASS BRIDGE               │        │
│  (arena2.cpp)               │        │ Click "JUGAR DE NUEVO"
│                             │        └───────────┐
│  • Grid 2×6 con trampas     │                    │
│  • Movimiento discreto      │                    ↓
│  • Vidrios grises           │              ┌─────────────┐
│  • Contorno negro           │              │   ARENA     │
└──────┬──────────────────────┘              │             │
       │                                     └─────────────┘
       │ Victoria o muerte
       ↓
┌─────────────┐
│  GAME OVER  │
│  (end.cpp)  │
└──────┬──────┘
       │ Click "JUGAR DE NUEVO"
       ↓
  Reinicia en ARENA
```

## Detalles de Cada Nivel

### 🎯 Menú Principal (menu.cpp)
- **Pantalla**: Fondo con imagen de Squid Game
- **Botones**: 
  - JUGAR → Inicia arena isométrica
  - SALIR → Cierra el juego
- **Controles**: Mouse (hover + click)

### ⏱️ Nivel 1: Arena Isométrica (arena_isometrica.cpp)
- **Duración**: 60 segundos exactos
- **Objetivo**: Sobrevivir esquivando pelotas
- **Jugadores**: 2 personajes estilo Roblox
- **Características**:
  - 30 pelotas cayendo del cielo
  - Sistema de vida (100 HP por jugador)
  - Timer visible en esquina superior derecha (rojo)
  - Vista isométrica 3D
  - Física realista (gravedad, rebotes)

**Controles Jugador 1:**
- W/A/S/D: Mover
- Q: Saltar

**Controles Jugador 2:**
- I/J/K/L: Mover
- U: Saltar

**Condiciones de finalización:**
1. **Tiempo terminado + sobrevivientes** → Pasa a Glass Bridge
2. **Ambos muertos antes del tiempo** → Game Over

### 🌉 Nivel 2: Glass Bridge (arena2.cpp)
- **Inspirado en**: Juego del Calamar - Desafío del Puente de Vidrio
- **Grid**: 2 columnas × 6 filas
- **Objetivo**: Llegar al final del puente sin pisar trampas

**Características del Grid:**
- **Vidrios no revelados**: Gris claro con brillo
- **Vidrios seguros**: Verde (después de pisarlos)
- **Vidrios rotos**: Rojo (trampa mortal)
- **Contorno**: Negro grueso en cada cuadrícula
- **Trampas**: 1 por fila, colocación aleatoria

**Reglas de Movimiento:**
- Solo avanzar hacia adelante (no retroceder)
- Opciones: Recto, diagonal izquierda, diagonal derecha
- Movimiento discreto (casilla por casilla)
- Animación suave de transición

**Controles Jugador 1:**
- W: Avanzar recto
- A: Diagonal izquierda-adelante
- D: Diagonal derecha-adelante

**Controles Jugador 2:**
- I: Avanzar recto
- J: Diagonal izquierda-adelante
- L: Diagonal derecha-adelante

**Condiciones de finalización:**
1. **Jugador llega a fila 6** → Victoria → Game Over
2. **Ambos muertos** → Game Over
3. **Pisar trampa** → Muerte instantánea

### 🔄 Game Over (end.cpp)
- **Pantalla**: Fondo oscurecido con mensaje
- **Texto**: "GAME OVER" en rojo
- **Subtítulo**: "Ambos jugadores han sido eliminados"
- **Botones**:
  - JUGAR DE NUEVO → Reinicia en Arena Isométrica
  - SALIR → Cierra el juego
- **Controles**: Mouse (hover + click)

## 🎮 Controles Generales

### En todos los niveles:
- **+/-**: Zoom in/out
- **Mouse arrastrar**: Mover cámara
- **ESC**: Salir del juego

## 🚀 Cómo Ejecutar

```bash
# Compilar todo
make arena arena2 menu end

# Iniciar juego completo desde menú
./menu

# O iniciar directamente en un nivel específico
./arena   # Arena Isométrica
./arena2  # Glass Bridge
./end     # Pantalla Game Over
```

## 📊 Progresión del Jugador

```
Inicio
  ↓
Sobrevivir 60s en Arena (Nivel 1)
  ↓
Cruzar el Puente de Vidrio (Nivel 2)
  ↓
Game Over → Reiniciar
```

## 🎨 Características Visuales

### Arena Isométrica:
- ✅ Vista isométrica 3D
- ✅ Texturas en paredes
- ✅ Piso texturizado
- ✅ Barras de vida
- ✅ Timer grande y visible
- ✅ 30 pelotas con colores variados
- ✅ Efectos de muerte

### Glass Bridge:
- ✅ Cuadrículas grises simulando vidrio
- ✅ Contorno negro grueso
- ✅ Efecto de brillo en vidrios no revelados
- ✅ Cambio de color al revelar (verde/rojo)
- ✅ Movimiento suave entre casillas
- ✅ Vista isométrica del puente

### Game Over:
- ✅ Fondo oscurecido
- ✅ Título grande en rojo
- ✅ Botones interactivos con hover
- ✅ Imagen de fondo de Squid Game

## 🔧 Tecnologías Utilizadas

- **Lenguaje**: C++17
- **Gráficos**: OpenCV 4.x
- **Proyección**: Isométrica custom
- **Física**: Sistema propio de gravedad y colisiones
- **Build**: Make

## 📝 Notas de Diseño

1. **Sistema de Timer**: Usa `<chrono>` para precisión exacta de 60 segundos
2. **Transiciones**: `system()` para cambiar entre ejecutables
3. **Grid Aleatorio**: Cada partida de Glass Bridge tiene trampas diferentes
4. **Muerte Instantánea**: Pisar trampa = game over inmediato
5. **Reinicio**: Siempre vuelve a Arena Isométrica, no al menú
