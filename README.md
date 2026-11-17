# 🎀 HairClip Digital — LilyGO T-Display S3

Firmware de horquilla / pasador de pelo digital (*HairClip*) portátil basado en la placa de desarrollo **LilyGO T-Display-S3** (ESP32-S3 con pantalla IPS color de 1.9" a 170×320 píxeles).

Permite mostrar texto en movimiento (marquee scroll), texto estático o imágenes personalizadas a pantalla completa, todo controlado de forma inalámbrica desde el iPhone mediante una moderna interfaz web con modo oscuro que levanta el propio dispositivo.

---

## ✨ Características

- 🖥️ **Pantalla IPS de alta velocidad**: Conexión por bus paralelo de 8 bits con la librería **LovyanGFX**, renderizado fluido sin parpadeos mediante sprites DMA.
- 📱 **Control total desde iPhone**:
  - Sin necesidad de instalar ninguna app.
  - El dispositivo crea su propia red Wi-Fi (`HairClip`).
  - Al conectarte, abre automáticamente el portal web (*Captive Portal* / `http://192.168.4.1`).
- 📝 **Editor de texto**:
  - Modo estático o texto en movimiento (*scroll marquee*).
  - Selector de color interactivo + presets rápidos.
  - Velocidad y tamaño de fuente ajustables.
  - Vista previa animada en tiempo real con la proporción exacta de la pantalla (320×170).
- 🖼️ **Subida de fotos e imágenes**:
  - El navegador (Safari del iPhone) procesa y redimensiona la imagen mediante HTML5 Canvas.
  - Convierte los píxeles a formato binario nativo **RGB565** antes de enviarlos, ahorrando CPU y batería al ESP32.
  - Opciones de recorte (*Cover*) o ajuste con bordes negros (*Contain*).
- 🔋 **Gestión inteligente de batería (Bajo consumo)**:
  - El Wi-Fi y el servidor web solo se activan cuando tú quieres configurar algo (ahorro máximo de energía mientras se usa).
  - Modo **Deep Sleep** (~150-200 µA): apaga pantalla, retroiluminación y riel de alimentación (`GPIO 15`).
- 💾 **Memoria persistente (LittleFS)**: Guarda el último texto, color, ajustes e imagen subida para que no se pierdan al apagar o reiniciar.

---

## 🔘 Control con Botones

| Botón | Pulsación | Acción |
|---|---|---|
| **BOOT (GPIO 0)** | Pulsación normal | **Modo Configuración**: Enciende el Wi-Fi AP (`HairClip`) y muestra la IP en pantalla. |
| **USER (GPIO 14)** | Mantener pulsado (> 1.2s) | **Modo Reposo (Deep Sleep)**: Apaga pantalla y periféricos para ahorrar batería. |
| **USER (GPIO 14)** | Pulsación normal (estando dormido) | **Despertar (Wakeup)**: Enciende la pantalla y reanuda el contenido guardado. |
| *Desde la web* | Botón "Guardar y Volver" | Apaga el Wi-Fi y vuelve inmediatamente al modo visualización. |

---

## 📌 Especificaciones de Hardware (LilyGO T-Display-S3)

| Componente | GPIO / Parámetro |
|---|---|
| **Microcontrolador** | ESP32-S3R8 Dual-Core LX7 @ 240MHz (16MB Flash, 8MB PSRAM OPI) |
| **Pantalla** | ST7789V 1.9" IPS TFT (170×320 px, orientación horizontal 320×170) |
| **Bus de Pantalla** | Paralelo 8-bit Intel 8080 (D0-D7: GPIO 39, 40, 41, 42, 45, 46, 47, 48) |
| **Control Pantalla** | WR: 8, RD: 9, RS/DC: 7, CS: 6, RST: 5 |
| **Power Enable (Pantalla)** | GPIO 15 (**HIGH** para encender) |
| **Backlight PWM** | GPIO 38 |
| **Botones** | GPIO 0 (BOOT), GPIO 14 (USER) |
| **ADC Batería** | GPIO 4 |

---

## 🚀 Cómo compilar y flashear el Firmware

### Requisitos
1. [Visual Studio Code](https://code.visualstudio.com/).
2. Extensión **PlatformIO IDE** (instálala desde la pestaña de extensiones en VS Code).
3. Cable USB-C de datos.

### Pasos
1. Abre esta carpeta en VS Code (`File > Open Folder...`).
2. Conecta el LilyGO T-Display S3 al puerto USB del ordenador.
3. En la barra inferior de VS Code:
   - Haz clic en **`✓` (Build)** para compilar el firmware.
   - Haz clic en **`→` (Upload)** para flashearlo a la placa.
   - Haz clic en **`🔌` (Serial Monitor)** para ver los mensajes de depuración a 115200 baudios.

### Si el flasheo falla (Modo Bootloader Manual)
Si PlatformIO se queda esperando (`Timed out waiting for packet header`):
1. Mantén presionado el botón **BOOT** (GPIO 0).
2. Pulsa y suelta el botón **RST** (Reset) sin soltar BOOT.
3. Suelta el botón **BOOT**.
4. Vuelve a pulsar **`→` (Upload)** en VS Code.
5. Al terminar, presiona una vez **RST** para arrancar.

---

## 📖 Instrucciones de Uso

1. Al encender, el HairClip mostrará el mensaje de bienvenida o el último diseño guardado.
2. Pulsa el botón **BOOT** (GPIO 0) para entrar en modo configuración. La pantalla mostrará:
   - SSID: `HairClip`
   - IP: `192.168.4.1`
3. En tu iPhone:
   - Ve a **Ajustes > Wi-Fi** y conéctate a la red **`HairClip`** (red abierta, sin contraseña).
   - Se abrirá automáticamente la ventana de inicio de sesión o puedes abrir **Safari** e ir a `http://192.168.4.1`.
4. Elige tu texto, colores, velocidad o sube una foto desde tu galería.
5. Pulsa **"Guardar y Volver a Mostrar"**. La red Wi-Fi se apagará para maximizar la duración de la batería y tu diseño brillará en el HairClip. 🎀
