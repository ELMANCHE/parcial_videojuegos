#!/usr/bin/env python3
"""
Reproductor de música de fondo para el juego Squid Game
"""
import pygame
import sys
import os
import signal
import time

# Variable global para controlar el loop
running = True
CONTROL_FILE = "/tmp/squidgame_music_control.txt"

def signal_handler(sig, frame):
    """Manejador de señales para detener limpiamente"""
    global running
    running = False

def play_background_music(music_file):
    """Reproduce música en loop hasta que el proceso termine"""
    global running
    
    # Crear archivo de control
    with open(CONTROL_FILE, 'w') as f:
        f.write("playing")
    
    try:
        # Registrar manejador de señales
        signal.signal(signal.SIGINT, signal_handler)
        signal.signal(signal.SIGTERM, signal_handler)
        
        # Inicializar pygame mixer
        pygame.mixer.init()
        
        # Cargar y reproducir música
        pygame.mixer.music.load(music_file)
        pygame.mixer.music.set_volume(0.5)  # Volumen al 50%
        pygame.mixer.music.play(-1)  # -1 = loop infinito
        
        print(f"🎵 Reproduciendo: {os.path.basename(music_file)}")
        
        # Mantener el script corriendo
        while running:
            time.sleep(0.1)
            
            # Verificar si el archivo de control existe
            if not os.path.exists(CONTROL_FILE):
                running = False
                break
            
            # Leer el archivo de control
            try:
                with open(CONTROL_FILE, 'r') as f:
                    status = f.read().strip()
                    if status == "stop":
                        running = False
                        break
            except:
                pass
            
    except Exception as e:
        print(f" Error: {e}")
    finally:
        pygame.mixer.music.stop()
        pygame.mixer.quit()
        # Limpiar archivo de control
        if os.path.exists(CONTROL_FILE):
            os.remove(CONTROL_FILE)
        print(" Audio detenido")

if __name__ == "__main__":
    music_path = "sources/squidgame2.mp3"
    
    if not os.path.exists(music_path):
        print(f" Error: No se encontró el archivo {music_path}")
        sys.exit(1)
    
    play_background_music(music_path)
