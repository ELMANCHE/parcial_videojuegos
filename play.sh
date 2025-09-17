#!/bin/bash

echo "🎮 ANGRY BIRDS OPENCV - LANZADOR 🎮"
echo "===================================="
echo "Versión: 1.0 - Velocidad Ultra-Optimizada"
echo "Desarrollado en C++ con OpenCV"
echo ""

# Cambiar al directorio del proyecto
cd "$(dirname "$0")"

# Verificar si el ejecutable existe
if [ ! -f "build/AngryBirdsOpenCV" ]; then
    echo "⚠️  Ejecutable no encontrado. Iniciando compilación automática..."
    echo ""
    ./build.sh
    if [ $? -ne 0 ]; then
        echo ""
        echo "❌ ERROR: Falló la compilación"
        echo "🔍 Ejecuta ./check-requirements.sh para diagnosticar problemas"
        exit 1
    fi
fi

# Verificar permisos de ejecución
if [ ! -x "build/AngryBirdsOpenCV" ]; then
    echo "� Otorgando permisos de ejecución..."
    chmod +x build/AngryBirdsOpenCV
fi

echo "🎯 INFORMACIÓN DEL JUEGO:"
echo "========================="
echo "📊 Estadísticas del ejecutable:"
echo "   Tamaño: $(ls -lh build/AngryBirdsOpenCV | awk '{print $5}')"
echo "   Fecha: $(ls -l build/AngryBirdsOpenCV | awk '{print $6, $7, $8}')"
echo ""

echo "🆕 MEJORAS EN ESTA VERSIÓN:"
echo "✅ Rango de tiro DUPLICADO (200px vs 100px)"
echo "✅ Velocidad ULTRA-OPTIMIZADA (120px/s vs 400px/s original)"
echo "✅ Gravedad REDUCIDA (200px/s² vs 400px/s²)"
echo "✅ Resistencia del aire MEJORADA (0.99 vs 0.98)"
echo "✅ Tiempo de vuelo EXTENDIDO (más tiempo para ver la física)"
echo "✅ Previsualización PERFECTA (coincide exactamente con el vuelo)"
echo ""

echo "🎮 CONTROLES:"
echo "============="
echo "🖱️  Mouse    → Arrastra para apuntar y disparar"
echo "⌃  SPACE    → Reiniciar nivel actual"  
echo "📝  N        → Siguiente nivel"
echo "⚡  C        → Habilidad especial del pájaro"
echo "⎋  ESC      → Salir del juego"
echo ""

echo "🏆 NIVELES DISPONIBLES:"
echo "======================"
echo "🟢 Nivel 1 - FÁCIL:"
echo "   • 2 cerdos pequeños"
echo "   • Estructura simple de madera"
echo "   • 3 pájaros rojos"
echo "   • Meta: 15,000 puntos"
echo ""
echo "🟡 Nivel 2 - INTERMEDIO:"
echo "   • 4 cerdos mixtos"
echo "   • Torres múltiples con materiales variados"
echo "   • 4 pájaros variados"
echo "   • Meta: 35,000 puntos"
echo ""
echo "🔴 Nivel 3 - DIFÍCIL:"
echo "   • 6 cerdos incluyendo grandes"
echo "   • Fortaleza compleja de piedra"
echo "   • 5 pájaros estratégicos"
echo "   • Meta: 60,000 puntos"
echo ""

echo "🎯 ESTRATEGIAS:"
echo "==============="
echo "• Observa la línea amarilla de trayectoria antes de disparar"
echo "• Los materiales tienen diferentes resistencias: Hielo < Madera < Piedra"
echo "• Usa la habilidad especial (tecla C) en el momento preciso"
echo "• Apunta a los puntos débiles de las estructuras"
echo "• Los pájaros amarillos son excelentes para acelerar en vuelo"
echo ""

echo "🚀 ¡INICIANDO EL JUEGO!"
echo "======================"
echo "Presiona ESC para salir en cualquier momento"
echo "¡Que tengas una excelente experiencia de juego!"
echo ""

# Dar una pausa para que se pueda leer la información
sleep 2

# Cambiar al directorio build y ejecutar
cd build
exec ./AngryBirdsOpenCV