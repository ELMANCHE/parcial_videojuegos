#!/bin/bash

echo "🎮 COMPILANDO ANGRY BIRDS OPENCV 🎮"
echo "=================================="

# Crear directorio build si no existe
if [ ! -d "build" ]; then
    echo "Creando directorio build..."
    mkdir build
fi

cd build

# Ejecutar CMake
echo "Configurando proyecto con CMake..."
cmake ..

if [ $? -eq 0 ]; then
    echo "✅ Configuración exitosa!"
    echo ""
    echo "Compilando proyecto..."
    make -j4
    
    if [ $? -eq 0 ]; then
        echo "✅ ¡Compilación exitosa!"
        echo ""
        echo "🚀 Para ejecutar el juego:"
        echo "   ./AngryBirdsOpenCV"
        echo ""
        echo "💡 Asegúrate de tener OpenCV instalado en tu sistema"
        echo "   En macOS: brew install opencv"
        echo "   En Ubuntu: sudo apt-get install libopencv-dev"
    else
        echo "❌ Error en la compilación"
        echo "Revisa que OpenCV esté correctamente instalado"
    fi
else
    echo "❌ Error en la configuración"
    echo "Asegúrate de tener CMake y OpenCV instalados"
fi

echo "=================================="