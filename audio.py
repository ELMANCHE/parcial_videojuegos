# short2mp3.py
import yt_dlp
import sys

# Puedes cambiar esta URL o pasar una como argumento
URL = "https://youtube.com/shorts/Txi9T0wlQEA?si=C5zkWfD9-FoCYSp4"  # Short por defecto

def main():
    # Si se pasa una URL como argumento, usarla
    url = sys.argv[1] if len(sys.argv) > 1 else URL
    
    print(f"📥 Descargando audio de: {url}")
    
    ydl_opts = {
        # Mejor audio disponible
        "format": "bestaudio/best",
        # Salida en el directorio actual con el título como nombre
        "outtmpl": "%(title)s.%(ext)s",
        # Extraer y convertir a MP3 con FFmpeg
        "postprocessors": [
            {
                "key": "FFmpegExtractAudio",
                "preferredcodec": "mp3",
                # '0' = mejor calidad VBR; usar '320' para CBR 320 kbps si se prefiere
                "preferredquality": "0",
            },
            # Incrustar metadatos ID3
            {"key": "FFmpegMetadata", "add_metadata": True},
            # Incrustar miniatura (requiere writethumbnail=True)
            {"key": "EmbedThumbnail", "already_have_thumbnail": False},
        ],
        # Necesario para poder incrustar la miniatura
        "writethumbnail": True,
        # ID3v2.3 para mejor compatibilidad
        "postprocessor_args": ["-id3v2_version", "3"],
        # Reintentos básicos
        "retries": 3,
        "fragment_retries": 3,
        # Progreso en consola
        "noprogress": False,
    }

    try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            ydl.download([url])
        print("✅ Descarga completada!")
    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
