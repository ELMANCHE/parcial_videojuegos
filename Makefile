# Ejecutable unificado menú + juego
squid: primeravista.cpp arena_isometrica.cpp
	$(CXX) $(CXXFLAGS) primeravista.cpp arena_isometrica.cpp -o squid $(OPENCV_FLAGS)
# Makefile para compilar juegos 3D con OpenCV

CXX = g++
CXXFLAGS = -std=c++17 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

# Targets

all: juegoroblox juego3d snake3d arena menu
menu: primeravista.cpp arena
	$(CXX) $(CXXFLAGS) primeravista.cpp -o menu $(OPENCV_FLAGS)

juegoroblox: juegoroblox.cpp
	$(CXX) $(CXXFLAGS) juegoroblox.cpp -o juegoroblox $(OPENCV_FLAGS)

juego3d: juego3d.cpp
	$(CXX) $(CXXFLAGS) juego3d.cpp -o juego3d $(OPENCV_FLAGS)

snake3d: snake3d_hypercube.cpp
	$(CXX) $(CXXFLAGS) snake3d_hypercube.cpp -o snake3d $(OPENCV_FLAGS)

arena: arena_isometrica.cpp
	$(CXX) $(CXXFLAGS) arena_isometrica.cpp -o arena $(OPENCV_FLAGS)

# Ejecutar juego de Roblox
run: juegoroblox
	./juegoroblox

# Ejecutar juego 3D original
run-juego: juego3d
	./juego3d

# Ejecutar Snake 3D
run-snake: snake3d
	./snake3d

# Ejecutar Arena Isométrica (nuevo)
run-arena: arena
	./arena

# Limpiar ejecutables
clean:
	rm -f juegoroblox juego3d snake3d arena

.PHONY: all run run-juego run-snake clean
