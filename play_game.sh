#!/bin/bash
# Script para iniciar el juego con música de fondo

# Colores para output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

CONTROL_FILE="/tmp/squidgame_music_control.txt"

echo -e "${BLUE}🎮 Iniciando Squid Game...${NC}"

# Limpiar archivo de control previo si existe
rm -f "$CONTROL_FILE"

# Iniciar música en segundo plano
echo -e "${GREEN}🎵 Iniciando música de fondo...${NC}"
.venv/bin/python play_music.py &
MUSIC_PID=$!

# Función para limpiar procesos al salir
cleanup() {
    echo -e "\n${GREEN}🔇 Deteniendo música...${NC}"
    
    # Señalar al proceso de música que debe detenerse
    echo "stop" > "$CONTROL_FILE" 2>/dev/null
    
    # Esperar un momento
    sleep 0.5
    
    # Si el proceso sigue vivo, forzar terminación
    if kill -0 $MUSIC_PID 2>/dev/null; then
        kill -TERM $MUSIC_PID 2>/dev/null
        sleep 0.3
    fi
    
    # Si aún sigue vivo, forzar kill
    if kill -0 $MUSIC_PID 2>/dev/null; then
        kill -9 $MUSIC_PID 2>/dev/null
    fi
    
    # Limpiar archivo de control
    rm -f "$CONTROL_FILE"
    
    # Eliminar cualquier proceso de play_music.py que siga corriendo
    pkill -f "play_music.py" 2>/dev/null
    
    echo -e "${BLUE}👋 ¡Hasta luego!${NC}"
}

# Capturar señales para limpiar al salir
trap cleanup EXIT INT TERM

# Esperar un momento para que inicie el audio
sleep 1

# Verificar que el proceso de música está corriendo
if ! kill -0 $MUSIC_PID 2>/dev/null; then
    echo -e "${RED} Error: No se pudo iniciar la música${NC}"
fi

# Ejecutar el menú del juego
echo -e "${BLUE}🎮 Iniciando juego...${NC}"
./menu

# La función cleanup se ejecutará automáticamente al salir
