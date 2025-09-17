# 🐦 Angry Birds OpenCV - Parcial Videojuegos

Un juego completo tipo Angry Birds implementado en **C++** usando la librería **OpenCV** para gráficos y física.

## 📋 Tabla de Contenidos
- [🚀 Inicio Rápido](#inicio-rápido)
- [Características](#características)
- [Requisitos del Sistema](#requisitos-del-sistema)
- [Instalación de Dependencias](#instalación-de-dependencias)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Controles del Juego](#controles-del-juego)
- [Niveles](#niveles)
- [Estructura del Proyecto](#estructura-del-proyecto)

## 🚀 Inicio Rápido

**¿Tienes prisa? ¡Empieza aquí!** 

📖 Ver [INICIO-RAPIDO.md](INICIO-RAPIDO.md) para instrucciones ultra-rápidas.

```bash
# Verificar requisitos
./check-requirements.sh

# Compilar
./build.sh

# Jugar
./play.sh
```

## ✨ Características

- 🎯 **3 niveles** con dificultades progresivas (Fácil, Intermedio, Difícil)
- ⚡ **Sistema de física realista** con gravedad, colisiones y trayectorias
- 🖱️ **Controles intuitivos** con mouse para apuntar y disparar
- 🎮 **3 tipos de pájaros** con habilidades especiales únicas
- 🏗️ **3 materiales de bloques** (Madera, Piedra, Hielo) con diferentes resistencias
- 🐷 **Cerdos de diferentes tamaños** (Pequeño, Mediano, Grande)
- 📊 **Sistema de puntuación** con bonificaciones por pájaros restantes
- 🎨 **Gráficos 2D** completamente dibujados con OpenCV
- 🎯 **Previsualización de trayectoria** en tiempo real
- 🎵 **Estados de juego** (Menú, Jugando, Completado, Game Over)

### 🆕 Mejoras Recientes
- ✅ **Rango de tiro extendido** (200px vs 100px original)
- ✅ **Velocidad optimizada** (120px/s para vuelo claramente visible)
- ✅ **Física mejorada** (gravedad reducida para mejor jugabilidad)
- ✅ **Trayectoria precisa** (previsualización que coincide exactamente con el vuelo)

## 💻 Requisitos del Sistema

### Sistemas Operativos Soportados
- ✅ **macOS** (10.14+)
- ✅ **Linux** (Ubuntu 18.04+, Debian 10+, CentOS 7+)
- ⚠️ **Windows** (con MinGW o Visual Studio)

### Dependencias Requeridas
- **C++ Compiler** con soporte C++17
- **CMake** 3.10 o superior
- **OpenCV** 4.0 o superior
- **pkg-config** (para detección automática de OpenCV)

## 📦 Instalación de Dependencias

### 🍎 macOS (Homebrew)
```bash
# Instalar Homebrew si no lo tienes
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar dependencias
brew install cmake opencv pkg-config

# Verificar instalación
cmake --version
pkg-config --modversion opencv4
```

### 🐧 Ubuntu/Debian
```bash
# Actualizar repositorios
sudo apt update

# Instalar dependencias
sudo apt install -y build-essential cmake pkg-config
sudo apt install -y libopencv-dev libopencv-contrib-dev

# Verificar instalación
cmake --version
pkg-config --modversion opencv4
```

### 🎩 CentOS/RHEL/Fedora
```bash
# CentOS/RHEL
sudo yum install -y gcc-c++ cmake pkg-config opencv-devel

# Fedora
sudo dnf install -y gcc-c++ cmake pkg-config opencv-devel

# Verificar instalación
cmake --version
pkg-config --modversion opencv4
```

### 🪟 Windows
```bash
# Opción 1: vcpkg (Recomendado)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install opencv4[contrib]:x64-windows

# Opción 2: Descargar binarios precompilados de OpenCV desde opencv.org
```

## 🔨 Compilación

### Método 1: Script Automático (Recomendado)
```bash
# Clonar el repositorio
git clone https://github.com/ELMANCHE/parcial_videojuegos.git
cd parcial_videojuegos

# Dar permisos de ejecución y compilar
chmod +x build.sh
./build.sh
```

### Método 2: Compilación Manual
```bash
# Clonar el repositorio
git clone https://github.com/ELMANCHE/parcial_videojuegos.git
cd parcial_videojuegos

# Crear directorio de compilación
mkdir build
cd build

# Configurar con CMake
cmake ..

# Compilar
make -j$(nproc)  # Linux/macOS
# nmake         # Windows con MSVC
```

### Verificación de Compilación
```bash
# El ejecutable debe aparecer en build/
ls -la build/AngryBirdsOpenCV

# Debe mostrar algo como:
# -rwxr-xr-x  1 user  staff  152704 AngryBirdsOpenCV
```

## 🚀 Ejecución

### Método 1: Script de Juego (Recomendado)
```bash
# Dar permisos y ejecutar
chmod +x play.sh
./play.sh
```

### Método 2: Ejecución Directa
```bash
# Desde el directorio del proyecto
cd build
./AngryBirdsOpenCV

# O desde cualquier lugar
/ruta/al/proyecto/build/AngryBirdsOpenCV
```

## 🎮 Controles del Juego

| Control | Acción |
|---------|--------|
| 🖱️ **Mouse** | Arrastra desde la catapulta para apuntar y disparar |
| ⌃ **SPACE** | Reiniciar nivel actual |
| 📝 **N** | Avanzar al siguiente nivel (cuando esté disponible) |
| ⚡ **C** | Activar habilidad especial del pájaro (durante el vuelo) |
| ⎋ **ESC** | Salir del juego |

### 🎯 Cómo Jugar
1. **Apuntar**: Arrastra el mouse desde la catapulta hacia atrás
2. **Trayectoria**: Observa los puntos amarillos que muestran la trayectoria
3. **Disparar**: Suelta el botón del mouse para lanzar
4. **Habilidad**: Presiona 'C' mientras el pájaro vuela para activar su poder especial
5. **Objetivo**: Destruye todos los cerdos para completar el nivel

## 🏆 Niveles

### 🟢 Nivel 1: Fácil
- **Objetivo**: 2 cerdos pequeños
- **Estructura**: Casa simple de madera
- **Pájaros**: 3 pájaros rojos
- **Puntuación objetivo**: 15,000 puntos

### 🟡 Nivel 2: Intermedio  
- **Objetivo**: 4 cerdos (pequeños y medianos)
- **Estructura**: Torres múltiples con materiales mixtos
- **Pájaros**: 4 pájaros (rojo, azul, amarillo, rojo)
- **Puntuación objetivo**: 35,000 puntos

### 🔴 Nivel 3: Difícil
- **Objetivo**: 6 cerdos (incluyendo grandes)
- **Estructura**: Fortaleza compleja de piedra
- **Pájaros**: 5 pájaros variados
- **Puntuación objetivo**: 60,000 puntos

### 🐦 Tipos de Pájaros
| Pájaro | Color | Habilidad Especial | Daño |
|--------|-------|-------------------|------|
| **Rojo** | 🔴 | Daño extra en impacto | 50 |
| **Azul** | 🔵 | Impulso adicional | 30 |
| **Amarillo** | 🟡 | Aceleración súbita | 40 |

## 📁 Estructura del Proyecto

```
parcial_videojuegos/
├── 📁 src/                    # Código fuente
│   ├── main.cpp              # Punto de entrada
│   ├── GameEngine.cpp        # Motor principal del juego
│   ├── Physics.cpp           # Sistema de física
│   ├── GameObjects.cpp       # Objetos del juego (Pájaros, Cerdos, Bloques)
│   └── LevelManager.cpp      # Gestor de niveles
├── 📁 include/               # Archivos de cabecera
│   ├── GameEngine.h
│   ├── Physics.h
│   ├── GameObjects.h
│   └── LevelManager.h
├── 📁 build/                 # Archivos de compilación
│   └── AngryBirdsOpenCV      # Ejecutable
├── 📄 CMakeLists.txt         # Configuración de CMake
├── 🚀 build.sh              # Script de compilación
├── 🎮 play.sh               # Script de ejecución
├── 📖 README.md             # Este archivo
└── 📝 MEJORAS_VELOCIDAD.md  # Documentación de mejoras
```

## 🔧 Solución de Problemas

### Error: "opencv not found"
```bash
# macOS
brew reinstall opencv
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"

# Ubuntu
sudo apt install -y pkg-config libopencv-dev

# Verificar
pkg-config --libs --cflags opencv4
```

### Error: "cmake: command not found"
```bash
# macOS
brew install cmake

# Ubuntu
sudo apt install cmake

# Verificar
cmake --version
```

### Error: "No such file or directory: AngryBirdsOpenCV"
```bash
# Recompilar
cd build && make

# O usar el script
./build.sh
```

### Problemas de Rendimiento
- Cierra otras aplicaciones que usen la GPU
- Verifica que tienes OpenCV con aceleración por hardware
- Reduce la resolución del juego editando `GameEngine.cpp`

## 👨‍💻 Desarrollo

### Compilar en Modo Debug
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Agregar Nuevos Niveles
1. Edita `src/LevelManager.cpp`
2. Crea una nueva función `createLevelX()`
3. Añade la llamada en `initializeLevels()`

### Personalizar Física
- Edita constantes en `include/Physics.h`
- Ajusta `GRAVITY`, `AIR_RESISTANCE`, etc.

## 📄 Licencia

Este proyecto fue desarrollado como parte del parcial de Videojuegos.

## 🙏 Agradecimientos

- **OpenCV** por la excelente librería de visión por computador
- **CMake** por el sistema de construcción multiplataforma
- Inspirado en el juego original **Angry Birds** de Rovio Entertainment

---

### 📊 Estadísticas del Proyecto
- **Líneas de código**: ~1,500
- **Archivos**: 12
- **Clases**: 7 principales
- **Tiempo de desarrollo**: Parcial académico
- **Lenguaje**: C++17
- **Librería gráfica**: OpenCV 4.x

## Controles
- **Mouse**: Apuntar y disparar el pájaro desde la catapulta
- **Spacebar**: Reiniciar nivel
- **N**: Siguiente nivel
- **ESC**: Salir del juego

## Niveles
1. **Fácil**: Pocos obstáculos y cerdos
2. **Intermedio**: Estructuras más complejas
3. **Difícil**: Múltiples cerdos y obstáculos desafiantes
