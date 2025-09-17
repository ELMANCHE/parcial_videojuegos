#!/bin/bash

echo "🎮 EJECUTANDO ANGRY BIRDS OPENCV 🎮"
echo "=================================="

# Cambiar al directorio del proyecto
cd "$(dirname "$0")"

# Verificar si el ejecutable existe
if [ ! -f "build/AngryBirdsOpenCV" ]; then
    echo "❌ Ejecutable no encontrado. Ejecutando compilación..."
    ./build.sh
    if [ $? -ne 0 ]; then
        echo "❌ Error en la compilación"
        exit 1
    fi
fi

echo "🚀 Iniciando juego..."
echo ""
echo "MEJORAS EN ESTA VERSIÓN:"
echo "✅ Rango de tiro duplicado (200px vs 100px)"
echo "✅ Velocidad reducida a la mitad (200px/s vs 400px/s)"
echo "✅ Mejor previsualización de trayectoria"
echo "✅ Física más visible y controlable"
echo ""
echo "CONTROLES:"
echo "🖱️  Mouse: Arrastra para apuntar y disparar"
echo "⌃  SPACE: Reiniciar nivel"
echo "📝  N: Siguiente nivel"
echo "⚡  C: Habilidad especial"
echo "⎋  ESC: Salir"
echo "=================================="

cd build
exec ./AngryBirdsOpenCV