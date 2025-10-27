# Ejecutable unificado menú + juego
squid: primeravista.cpp arena_isometrica.cpp
	$(CXX) $(CXXFLAGS) primeravista.cpp arena_isometrica.cpp -o squid $(OPENCV_FLAGS)
# Makefile para compilar juegos 3D con OpenCV

CXX = g++
CXXFLAGS = -std=c++17 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`
SFML_FLAGS = `pkg-config --cflags --libs sfml-audio`

# Targets

all: juegoroblox juego3d snake3d arena arena2 menu end
menu: primeravista.cpp arena arena2 end
	$(CXX) $(CXXFLAGS) primeravista.cpp -o menu $(OPENCV_FLAGS) $(SFML_FLAGS)

end: end.cpp
	$(CXX) $(CXXFLAGS) end.cpp -o end $(OPENCV_FLAGS) $(SFML_FLAGS)

juegoroblox: juegoroblox.cpp
	$(CXX) $(CXXFLAGS) juegoroblox.cpp -o juegoroblox $(OPENCV_FLAGS) $(SFML_FLAGS)

juego3d: juego3d.cpp
	$(CXX) $(CXXFLAGS) juego3d.cpp -o juego3d $(OPENCV_FLAGS) $(SFML_FLAGS)

snake3d: snake3d_hypercube.cpp
	$(CXX) $(CXXFLAGS) snake3d_hypercube.cpp -o snake3d $(OPENCV_FLAGS) $(SFML_FLAGS)

arena: arena_isometrica.cpp
	$(CXX) $(CXXFLAGS) arena_isometrica.cpp -o arena $(OPENCV_FLAGS) $(SFML_FLAGS)

arena2: arena2.cpp
	$(CXX) $(CXXFLAGS) arena2.cpp -o arena2 $(OPENCV_FLAGS) $(SFML_FLAGS)

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

# Ejecutar Glass Bridge (arena2)
run-arena2: arena2
	./arena2

# Ejecutar juego completo desde el menú
play: menu
	./menu

# Limpiar ejecutables
clean:
	rm -f juegoroblox juego3d snake3d arena arena2 menu end

.PHONY: all run run-juego run-snake run-arena run-arena2 play clean
