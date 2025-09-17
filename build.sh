#!/bin/bash

echo "🎮 COMPILANDO ANGRY BIRDS OPENCV 🎮"
echo "=================================="
echo "Versión: 1.0 - Velocidad Optimizada"
echo "Fecha: $(date)"
echo ""

# Verificar requisitos básicos
echo "🔍 Verificando requisitos..."
if ! command -v cmake &> /dev/null; then
    echo "❌ ERROR: CMake no está instalado"
    echo "💡 Instalar con: brew install cmake (macOS) o sudo apt install cmake (Ubuntu)"
    exit 1
fi

if ! pkg-config --exists opencv4 && ! pkg-config --exists opencv; then
    echo "❌ ERROR: OpenCV no está instalado"
    echo "💡 Instalar con: brew install opencv (macOS) o sudo apt install libopencv-dev (Ubuntu)"
    exit 1
fi

echo "✅ Requisitos verificados"
echo ""

# Crear directorio build si no existe
if [ ! -d "build" ]; then
    echo "📁 Creando directorio build..."
    mkdir build
fi

cd build

# Ejecutar CMake
echo "⚙️  Configurando proyecto con CMake..."
cmake ..

if [ $? -eq 0 ]; then
    echo "✅ Configuración exitosa!"
    echo ""
    echo "🔨 Compilando proyecto..."
    echo "   Usando $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) núcleos de CPU"
    
    # Detectar sistema para usar el comando make apropiado
    if [[ "$OSTYPE" == "darwin"* ]] || [[ "$OSTYPE" == "linux-gnu"* ]]; then
        make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    else
        make
    fi
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "🎉 ¡COMPILACIÓN EXITOSA!"
        echo "=================================="
        echo "📊 Estadísticas:"
        echo "   Ejecutable: $(ls -lh AngryBirdsOpenCV 2>/dev/null | awk '{print $5}' || echo 'No encontrado')"
        echo "   Ubicación: $(pwd)/AngryBirdsOpenCV"
        echo ""
        echo "🚀 Para ejecutar el juego:"
        echo "   Opción 1: cd .. && ./play.sh"
        echo "   Opción 2: ./AngryBirdsOpenCV"
        echo ""
        echo "🎮 CARACTERÍSTICAS DEL JUEGO:"
        echo "   ✅ 3 niveles de dificultad progresiva"
        echo "   ✅ Física realista con gravedad y colisiones"
        echo "   ✅ Velocidad optimizada (120px/s - perfecta para seguimiento visual)"
        echo "   ✅ Rango de tiro extendido (200px)"
        echo "   ✅ Previsualización precisa de trayectoria"
        echo "=================================="
    else
        echo ""
        echo "❌ ERROR EN LA COMPILACIÓN"
        echo "🔍 Posibles soluciones:"
        echo "   1. Verificar que OpenCV está correctamente instalado"
        echo "   2. Ejecutar: ./check-requirements.sh"
        echo "   3. Revisar que tienes un compilador C++17"
        exit 1
    fi
else
    echo "❌ ERROR EN LA CONFIGURACIÓN"
    echo "🔍 Posibles soluciones:"
    echo "   1. Verificar instalación de CMake: cmake --version"
    echo "   2. Verificar instalación de OpenCV: pkg-config --modversion opencv4"
    echo "   3. Ejecutar: ./check-requirements.sh"
    exit 1
fi

echo "=================================="