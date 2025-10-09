#!/bin/bash
# Script para detener la música si sigue sonando

echo "🔇 Deteniendo toda la música..."

# Señalar al proceso que debe detenerse
echo "stop" > /tmp/squidgame_music_control.txt 2>/dev/null

# Eliminar todos los procesos de play_music.py
pkill -9 -f "play_music.py" 2>/dev/null

# Limpiar archivo de control
rm -f /tmp/squidgame_music_control.txt

echo "Música detenida"
