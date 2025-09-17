#!/bin/bash

echo "🔍 VERIFICADOR DE REQUISITOS - ANGRY BIRDS OPENCV"
echo "================================================="

# Función para verificar comando
check_command() {
    if command -v "$1" &> /dev/null; then
        echo "✅ $1: $(which $1)"
        if [ "$1" = "cmake" ]; then
            echo "   Versión: $(cmake --version | head -n1)"
        elif [ "$1" = "pkg-config" ]; then
            echo "   Versión: $(pkg-config --version)"
        elif [ "$1" = "make" ]; then
            echo "   Versión: $(make --version | head -n1)"
        fi
    else
        echo "❌ $1: NO ENCONTRADO"
        return 1
    fi
}

# Función para verificar OpenCV
check_opencv() {
    if pkg-config --exists opencv4; then
        echo "✅ OpenCV4: $(pkg-config --modversion opencv4)"
        echo "   Ruta: $(pkg-config --variable=prefix opencv4)"
        echo "   Libs: $(pkg-config --libs opencv4 | cut -c1-50)..."
    elif pkg-config --exists opencv; then
        echo "✅ OpenCV: $(pkg-config --modversion opencv)"
        echo "   Ruta: $(pkg-config --variable=prefix opencv)"
    else
        echo "❌ OpenCV: NO ENCONTRADO"
        return 1
    fi
}

# Verificar sistema operativo
echo "🖥️  Sistema Operativo:"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "   macOS $(sw_vers -productVersion)"
    PACKAGE_MANAGER="Homebrew"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if command -v lsb_release &> /dev/null; then
        echo "   $(lsb_release -d -s)"
    else
        echo "   Linux (distribución desconocida)"
    fi
    if command -v apt &> /dev/null; then
        PACKAGE_MANAGER="apt (Debian/Ubuntu)"
    elif command -v yum &> /dev/null; then
        PACKAGE_MANAGER="yum (RHEL/CentOS)"
    elif command -v dnf &> /dev/null; then
        PACKAGE_MANAGER="dnf (Fedora)"
    else
        PACKAGE_MANAGER="Desconocido"
    fi
else
    echo "   Sistema no identificado: $OSTYPE"
    PACKAGE_MANAGER="Manual"
fi
echo "   Gestor de paquetes: $PACKAGE_MANAGER"
echo ""

# Verificar compilador C++
echo "🔧 Compilador C++:"
if command -v g++ &> /dev/null; then
    echo "✅ g++: $(g++ --version | head -n1)"
elif command -v clang++ &> /dev/null; then
    echo "✅ clang++: $(clang++ --version | head -n1)"
elif command -v c++ &> /dev/null; then
    echo "✅ c++: $(c++ --version | head -n1)"
else
    echo "❌ Compilador C++: NO ENCONTRADO"
fi
echo ""

# Verificar herramientas de construcción
echo "🛠️  Herramientas de Construcción:"
check_command cmake
check_command make
check_command pkg-config
echo ""

# Verificar OpenCV
echo "📹 OpenCV:"
check_opencv
echo ""

# Verificar si el proyecto puede compilar
echo "🎮 Estado del Proyecto:"
if [ -f "CMakeLists.txt" ]; then
    echo "✅ CMakeLists.txt encontrado"
else
    echo "❌ CMakeLists.txt NO encontrado"
fi

if [ -d "src" ]; then
    echo "✅ Directorio src encontrado ($(ls src/*.cpp | wc -l | tr -d ' ') archivos .cpp)"
else
    echo "❌ Directorio src NO encontrado"
fi

if [ -d "include" ]; then
    echo "✅ Directorio include encontrado ($(ls include/*.h | wc -l | tr -d ' ') archivos .h)"
else
    echo "❌ Directorio include NO encontrado"
fi

if [ -f "build/AngryBirdsOpenCV" ]; then
    echo "✅ Ejecutable ya compilado"
else
    echo "⚠️  Ejecutable no encontrado (necesita compilación)"
fi
echo ""

# Sugerencias de instalación
echo "📋 INSTRUCCIONES DE INSTALACIÓN:"
echo "================================="

if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS:"
    echo "brew install cmake opencv pkg-config"
elif command -v apt &> /dev/null; then
    echo "Ubuntu/Debian:"
    echo "sudo apt update"
    echo "sudo apt install -y build-essential cmake pkg-config libopencv-dev"
elif command -v yum &> /dev/null; then
    echo "CentOS/RHEL:"
    echo "sudo yum install -y gcc-c++ cmake pkg-config opencv-devel"
elif command -v dnf &> /dev/null; then
    echo "Fedora:"
    echo "sudo dnf install -y gcc-c++ cmake pkg-config opencv-devel"
fi

echo ""
echo "📖 Para más información consulta el README.md"
echo "================================================="