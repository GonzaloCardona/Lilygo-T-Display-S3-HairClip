# 🎀 HairClip Digital — LilyGO T-Display S3

[![PlatformIO Registry](https://img.shields.io/badge/PlatformIO-ESP32--S3-orange.svg)](https://platformio.org/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)
[![Display Driver](https://img.shields.io/badge/Driver-LovyanGFX-green.svg)](https://github.com/lovyan03/LovyanGFX)
[![License](https://img.shields.io/badge/License-MIT-purple.svg)](LICENSE)

Firmware de horquilla / pasador de pelo digital (**HairClip**) portátil basado en la placa de desarrollo **LilyGO T-Display-S3** (ESP32-S3 con pantalla IPS color de 1.9" a 170×320 píxeles y 8 MB de memoria PSRAM).

Permite mostrar texto con emojis nativos a todo color y tildes en movimiento (*marquee scroll* infinito), texto estático centrado o fotografías encuadradas a pantalla completa. Todo se controla de forma inalámbrica desde el iPhone mediante una moderna interfaz web con modo oscuro que levanta el propio dispositivo en su punto de acceso Wi-Fi autónomo.

---

## ✨ Características Principales

- 🖥️ **Pantalla IPS a 20 MHz (Bus Paralelo 8-bit Intel 8080)**: Conexión ultrarrápida mediante **LovyanGFX**, renderizado suave a 50 FPS sin ningún parpadeo gracias al uso de sprites en PSRAM.
- 📱 **Control inalámbrico directo desde iPhone**:
  - Sin instalar ninguna aplicación externa.
  - El dispositivo crea su propia red Wi-Fi autónoma (`HairClip`).
  - Al conectarte, abre automáticamente el portal web (*Captive Portal* / `http://192.168.4.1`).
- 🔤 **Texto, Tildes, 'Ñ' y Emojis en Color**:
  - Renderizado nativo en el cliente con fuentes vectoriales y Apple Color Emoji de alta definición.
  - **Bucle de scroll continuo sin cortes bruscos**: el ESP32 carga el cartel en su memoria PSRAM (8 MB) y lo desplaza en un bucle infinito suave con repetición fluida (`gap = 60 px`).
  - Modo estático centrado o texto en movimiento (*scroll marquee*).
- 🎚️ **Selector continuo de tamaño de letra**:
  - Slider continuo de 20 px a 90 px con previsualización en vivo.
- 🎯 **Vista previa 100% idéntica**:
  - Vista previa en un Canvas interactivo con la relación de aspecto real (320×170).
  - Utiliza exactamente la misma tipografía del sistema, proporciones de píxeles, colores y velocidad de animación que la pantalla física.
- 🎨 **Colores de fondo personalizables**:
  - Selector de color de texto y color de fondo con paletas HEX y botones de colores rápidos.
  - Selector de color de fondo para bordes en imágenes en modo ajuste (*contain*).
- 🖼️ **Subida, encuadre y desplazamiento de fotografías**:
  - **Arrastre táctil directo**: Toca con el dedo la foto en la vista previa del iPhone y arrástrala para ajustar el encuadre sobre la marcha.
  - **Controles de precisión**: Sliders de desplazamiento horizontal (-100% a +100%), vertical (-100% a +100%) y zoom (100% a 300%).
  - Modos de visualización: **Recortar (Llenar pantalla)** o **Ajustar (Bordes rellenos)**.
  - Conversión automática en el navegador a formato binario nativo **RGB565 Little-Endian**, liberando de cómputo a la CPU del ESP32.
- 🔋 **Ahorro extremo de batería (Bajo consumo)**:
  - El Wi-Fi y el servidor web solo están encendidos durante la configuración. En modo normal se apagan por completo.
  - **Modo Deep Sleep (~150-200 µA)**: apaga la pantalla, la retroiluminación y corta el riel de alimentación (`GPIO 15`) reteniendo el estado con `rtc_gpio_hold_en`.
- 💾 **Memoria persistente (LittleFS)**:
  - Guarda automáticamente el último texto, colores, velocidad, orientación, modo e imagen en memoria flash flash no volátil.

---

## 🔘 Control con Botones Físicos

| Botón | Pulsación | Acción |
|---|---|---|
| **BOOT (GPIO 0)** | Pulsación normal | **Modo Configuración**: Enciende el Wi-Fi AP (`HairClip`) y muestra la IP y datos de conexión en pantalla. |
| **USER (GPIO 14)** | Mantener pulsado (> 1.2s) | **Modo Reposo (Deep Sleep)**: Apaga pantalla y periféricos para no gastar batería. |
| **USER (GPIO 14)** | Pulsación normal (estando dormido) | **Despertar (Wakeup)**: Enciende la pantalla y reanuda el contenido guardado al instante. |
| *Desde la web* | Botón "Guardar y Volver a Mostrar" | Guarda la configuración en LittleFS, apaga el Wi-Fi y regresa al modo visualización. |

---

## 📌 Especificaciones Hardware (LilyGO T-Display-S3)

| Componente | GPIO / Parámetro |
|---|---|
| **Microcontrolador** | ESP32-S3R8 Dual-Core LX7 @ 240MHz (16MB Flash, 8MB PSRAM OPI) |
| **Pantalla** | ST7789V 1.9" IPS TFT (170×320 px, orientación horizontal 320×170) |
| **Bus de Pantalla** | Paralelo 8-bit Intel 8080 (D0-D7: GPIO 39, 40, 41, 42, 45, 46, 47, 48) |
| **Líneas de Control** | WR: 8, RD: 9, RS/DC: 7, CS: 6, RST: 5 |
| **Power Enable (Pantalla)** | GPIO 15 (**HIGH** para encender riel de alimentación) |
| **Backlight PWM** | GPIO 38 |
| **Botones** | GPIO 0 (BOOT), GPIO 14 (USER) |
| **ADC Batería** | GPIO 4 (Divisor resistivo 2:1) |

---

## 🚀 Cómo compilar y flashear el Firmware

### Requisitos
1. [Visual Studio Code](https://code.visualstudio.com/).
2. Extensión **PlatformIO IDE** (búscala en la pestaña de extensiones de VS Code).
3. Cable USB-C con soporte para transferencia de datos.

### Pasos
1. Clona o descarga este repositorio:
   ```bash
   git clone https://github.com/GonzaloCardona/Lilygo-T-Display-S3-HairClip.git
   cd Lilygo-T-Display-S3-HairClip
   ```
2. Abre la carpeta en VS Code (`File > Open Folder...`).
3. Conecta el LilyGO T-Display S3 al puerto USB del ordenador.
4. En la barra inferior de VS Code (o mediante terminal):
   - **Compilar**: `python -m platformio run`
   - **Subir a la placa**: `python -m platformio run -t upload`
   - **Monitor Serie (115200 baudios)**: `python -m platformio device monitor -b 115200`

### Si el flasheo falla (Modo Bootloader Manual)
Si PlatformIO se queda esperando (`Timed out waiting for packet header`):
1. Mantén presionado el botón **BOOT** (GPIO 0).
2. Pulsa y suelta el botón **RST** (Reset) sin soltar BOOT.
3. Suelta el botón **BOOT**.
4. Vuelve a ejecutar la subida (`upload`).
5. Al terminar, presiona una vez **RST** para arrancar el firmware normalmente.

---

## 📖 Instrucciones de Uso

1. Al encender el dispositivo por primera vez, verás el mensaje de bienvenida.
2. Pulsa el botón **BOOT** (GPIO 0) para entrar en el modo de configuración. La pantalla mostrará una tarjeta con los datos de conexión:
   - **Wi-Fi:** `HairClip`
   - **IP:** `http://192.168.4.1`
3. En tu iPhone:
   - Abre **Ajustes > Wi-Fi** y conéctate a la red **`HairClip`** (red abierta, no requiere contraseña).
   - Se abrirá automáticamente la ventana de inicio de sesión o puedes entrar en **Safari** e ir a `http://192.168.4.1`.
4. Personaliza tu HairClip:
   - **Pestaña Texto**: Escribe tu mensaje, añade emojis, ajusta el slider de tamaño, elige colores de texto y fondo, y selecciona la velocidad de desplazamiento.
   - **Pestaña Imagen**: Sube una foto de tu carrete, arrastra con el dedo sobre la pantalla para encuadrarla o usa los sliders de zoom y posición.
   - **Pestaña Ajustes**: Ajusta el brillo o gira la orientación de la pantalla 180° si lo llevas en el otro lado del pelo.
5. Pulsa **"Guardar y Volver a Mostrar"**. La red Wi-Fi se apagará para maximizar la autonomía y tu diseño se mostrará de forma continua.

---

## 📄 Licencia

Este proyecto es de código abierto bajo la licencia [MIT](LICENSE).
