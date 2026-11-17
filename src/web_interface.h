#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>🎀 HairClip Control</title>
  <style>
    :root {
      --bg: #0f111a;
      --card: #181b26;
      --accent: #ff4d8d;
      --accent-glow: rgba(255, 77, 141, 0.35);
      --text: #f0f3fa;
      --text-muted: #8c93aa;
      --border: #282c3c;
      --success: #00e676;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body { background: var(--bg); color: var(--text); padding: 14px; display: flex; flex-direction: column; align-items: center; min-height: 100vh; }
    .container { width: 100%; max-width: 420px; }
    header { text-align: center; margin-bottom: 16px; }
    header h1 { font-size: 1.5rem; color: var(--accent); font-weight: 800; letter-spacing: -0.5px; text-shadow: 0 0 16px var(--accent-glow); }
    header p { font-size: 0.85rem; color: var(--text-muted); margin-top: 2px; }

    /* Tabs */
    .nav-tabs { display: flex; background: var(--card); border-radius: 12px; padding: 4px; margin-bottom: 16px; border: 1px solid var(--border); }
    .tab-btn { flex: 1; padding: 10px 0; background: none; border: none; color: var(--text-muted); font-size: 0.9rem; font-weight: 600; cursor: pointer; border-radius: 8px; transition: all 0.2s; }
    .tab-btn.active { background: var(--accent); color: white; box-shadow: 0 2px 8px var(--accent-glow); }

    /* Card Panels */
    .tab-content { display: none; background: var(--card); border-radius: 16px; padding: 18px; border: 1px solid var(--border); box-shadow: 0 4px 20px rgba(0,0,0,0.4); margin-bottom: 16px; }
    .tab-content.active { display: block; animation: fadeIn 0.25s ease; }
    @keyframes fadeIn { from { opacity: 0; transform: translateY(4px); } to { opacity: 1; transform: translateY(0); } }

    .form-group { margin-bottom: 16px; }
    label { display: block; font-size: 0.82rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.6px; color: var(--text-muted); margin-bottom: 8px; }
    input[type="text"] { width: 100%; padding: 12px 14px; background: #12141f; border: 1px solid var(--border); border-radius: 10px; color: var(--text); font-size: 1rem; outline: none; }
    input[type="text"]:focus { border-color: var(--accent); box-shadow: 0 0 8px var(--accent-glow); }

    /* Mode toggles */
    .btn-group { display: flex; gap: 8px; }
    .btn-opt { flex: 1; padding: 10px; background: #12141f; border: 1px solid var(--border); border-radius: 10px; color: var(--text); font-size: 0.85rem; font-weight: 600; cursor: pointer; }
    .btn-opt.selected { background: rgba(255, 77, 141, 0.15); border-color: var(--accent); color: var(--accent); }

    /* Color picker & presets */
    .color-row { display: flex; align-items: center; gap: 10px; }
    input[type="color"] { -webkit-appearance: none; border: none; width: 44px; height: 44px; border-radius: 10px; cursor: pointer; background: transparent; }
    input[type="color"]::-webkit-color-swatch-wrapper { padding: 0; }
    input[type="color"]::-webkit-color-swatch { border: 2px solid var(--border); border-radius: 10px; }
    .presets { display: flex; gap: 6px; flex-wrap: wrap; }
    .chip { width: 32px; height: 32px; border-radius: 8px; border: 2px solid var(--border); cursor: pointer; }

    /* Sliders */
    .slider-container { display: flex; align-items: center; gap: 12px; }
    input[type="range"] { flex: 1; accent-color: var(--accent); }
    .slider-val { width: 32px; text-align: right; font-size: 0.9rem; font-weight: 700; color: var(--accent); }

    /* Live Preview Screen (320:170 ratio) */
    .preview-box { width: 100%; aspect-ratio: 320 / 170; background: black; border-radius: 12px; border: 2px solid var(--border); overflow: hidden; position: relative; display: flex; align-items: center; justify-content: center; margin-bottom: 16px; box-shadow: inset 0 0 20px rgba(0,0,0,0.8); }
    .preview-text { white-space: nowrap; font-weight: 700; position: absolute; font-size: 20px; }
    .marquee { animation: marquee linear infinite; }
    @keyframes marquee { 0% { transform: translateX(100%); } 100% { transform: translateX(-100%); } }

    /* Image upload canvas */
    #imageCanvas { max-width: 100%; border-radius: 8px; border: 1px solid var(--border); background: black; }
    .file-btn { display: block; text-align: center; width: 100%; padding: 12px; background: #212536; border: 1px dashed var(--accent); border-radius: 10px; color: var(--text); font-weight: 600; cursor: pointer; }
    input[type="file"] { display: none; }

    /* Action Buttons */
    .btn-primary { width: 100%; padding: 14px; background: var(--accent); border: none; border-radius: 12px; color: white; font-size: 1rem; font-weight: 700; cursor: pointer; box-shadow: 0 4px 14px var(--accent-glow); transition: 0.2s; }
    .btn-primary:active { transform: scale(0.98); }
    .btn-save { width: 100%; padding: 14px; background: #12141f; border: 1px solid var(--success); border-radius: 12px; color: var(--success); font-size: 1rem; font-weight: 700; cursor: pointer; margin-top: 8px; }

    /* Toast */
    #toast { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%); background: #242938; color: white; padding: 10px 20px; border-radius: 20px; font-size: 0.85rem; font-weight: 600; border: 1px solid var(--border); display: none; z-index: 100; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>🎀 HairClip Digital</h1>
      <p>Control portátil para LilyGO T-Display S3</p>
    </header>

    <div class="nav-tabs">
      <button class="tab-btn active" onclick="switchTab('text')">Texto</button>
      <button class="tab-btn" onclick="switchTab('image')">Imagen</button>
      <button class="tab-btn" onclick="switchTab('settings')">Ajustes</button>
    </div>

    <!-- TAB 1: TEXTO -->
    <div id="tab-text" class="tab-content active">
      <div class="form-group">
        <label>Mensaje a mostrar</label>
        <input type="text" id="clipText" value="HairClip Digital" oninput="updatePreview()" placeholder="Escribe tu texto...">
      </div>

      <div class="form-group">
        <label>Modo de animación</label>
        <div class="btn-group">
          <button type="button" class="btn-opt" id="modeStatic" onclick="setMode(false)">Estático</button>
          <button type="button" class="btn-opt selected" id="modeScroll" onclick="setMode(true)">En Movimiento</button>
        </div>
      </div>

      <div class="form-group">
        <label>Color del texto</label>
        <div class="color-row">
          <input type="color" id="clipColor" value="#ff69b4" onchange="updatePreview()">
          <div class="presets">
            <div class="chip" style="background:#ff69b4" onclick="setColor('#ff69b4')"></div>
            <div class="chip" style="background:#00f5d4" onclick="setColor('#00f5d4')"></div>
            <div class="chip" style="background:#fee440" onclick="setColor('#fee440')"></div>
            <div class="chip" style="background:#9b5de5" onclick="setColor('#9b5de5')"></div>
            <div class="chip" style="background:#ff595e" onclick="setColor('#ff595e')"></div>
            <div class="chip" style="background:#ffffff" onclick="setColor('#ffffff')"></div>
          </div>
        </div>
      </div>

      <div class="form-group">
        <label>Tamaño de letra</label>
        <div class="btn-group">
          <button type="button" class="btn-opt" onclick="setSize(2, this)">P</button>
          <button type="button" class="btn-opt selected" onclick="setSize(3, this)">M</button>
          <button type="button" class="btn-opt" onclick="setSize(4, this)">G</button>
          <button type="button" class="btn-opt" onclick="setSize(5, this)">XL</button>
        </div>
      </div>

      <div class="form-group" id="speedGroup">
        <label>Velocidad de movimiento</label>
        <div class="slider-container">
          <input type="range" id="clipSpeed" min="1" max="10" value="3" oninput="updateSpeedVal(this.value)">
          <span class="slider-val" id="speedVal">3</span>
        </div>
      </div>

      <div class="form-group">
        <label>Vista previa simulada (320 × 170)</label>
        <div class="preview-box">
          <div id="previewText" class="preview-text marquee">HairClip Digital</div>
        </div>
      </div>

      <div style="display:flex; flex-direction:column; gap:8px;">
        <button class="btn-primary" onclick="sendText()">🚀 Aplicar Texto</button>
        <button class="btn-primary" style="background: linear-gradient(135deg, #ff4d8d, #8a2be2); box-shadow: 0 4px 14px rgba(138,43,226,0.35);" onclick="sendAsEmojiBanner()">🎨 Enviar como Cartel con Emojis Reales</button>
      </div>
      <p style="font-size:0.75rem; color:var(--text-muted); text-align:center; margin-top:8px; line-height:1.4;">
        💡 <strong>Nota sobre Emojis:</strong> El chip del ESP32 procesa texto estándar. Para ver emojis en color real (🎀, ✨, ❤️, etc.), pulsa <em>"Cartel con Emojis Reales"</em> para que tu iPhone los dibuje en alta definición.
      </p>
    </div>

    <!-- TAB 2: IMAGEN -->
    <div id="tab-image" class="tab-content">
      <div class="form-group">
        <label>Seleccionar imagen</label>
        <label class="file-btn" for="fileInput">📁 Elegir Foto desde iPhone</label>
        <input type="file" id="fileInput" accept="image/*" onchange="handleFileSelect(event)">
      </div>

      <div class="form-group">
        <label>Ajuste de imagen</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="fitCover" onclick="setImageFit('cover')">Recortar (Llenar)</button>
          <button type="button" class="btn-opt" id="fitContain" onclick="setImageFit('contain')">Ajustar (Bordes)</button>
        </div>
      </div>

      <div class="form-group">
        <label>Vista previa redimensionada (320 × 170)</label>
        <canvas id="imageCanvas" width="320" height="170"></canvas>
      </div>

      <button class="btn-primary" id="btnUploadImage" onclick="uploadImage()" disabled>📤 Enviar Imagen a Pantalla</button>
    </div>

    <!-- TAB 3: AJUSTES -->
    <div id="tab-settings" class="tab-content">
      <div class="form-group">
        <label>Brillo de Pantalla</label>
        <div class="slider-container">
          <input type="range" id="clipBrightness" min="20" max="255" value="160" oninput="updateBrightness(this.value)">
          <span class="slider-val" id="brightVal">160</span>
        </div>
      </div>

      <div class="form-group">
        <label>Orientación de Pantalla (Girar 180°)</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="rotNormal" onclick="setRotation(1)">Normal (USB derecha)</button>
          <button type="button" class="btn-opt" id="rotInverted" onclick="setRotation(3)">Invertida (USB izquierda)</button>
        </div>
      </div>

      <div class="form-group">
        <label>Modo de Color para Fotos/Emojis (Swap Bytes)</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="swapTrue" onclick="setSwapBytes(true)">Corregido (Normal)</button>
          <button type="button" class="btn-opt" id="swapFalse" onclick="setSwapBytes(false)">Invertir</button>
        </div>
      </div>

      <div class="form-group" style="background:#12141f; padding:12px; border-radius:10px; font-size:0.85rem; color:var(--text-muted); line-height:1.5;">
        <strong style="color:var(--text);">💡 Consejos de uso:</strong><br>
        • <strong>Botón User (GPIO 14)</strong>: Mantén pulsado >1s para apagar la pantalla y entrar en modo reposo de bajo consumo.<br>
        • <strong>Para despertar</strong>: Vuelve a presionar el botón User.<br>
        • <strong>Botón BOOT (GPIO 0)</strong>: Pulsa para volver a levantar esta red WiFi.
      </div>
    </div>

    <!-- BOTÓN FINAL -->
    <button class="btn-save" onclick="applyAndExit()">💾 Guardar y Volver a Mostrar</button>
  </div>

  <div id="toast"></div>

  <script>
    let isScroll = true;
    let currentFontSize = 3;
    let selectedImage = null;
    let imageFitMode = 'cover';

    function switchTab(tabId) {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
      event.target.classList.add('active');
      document.getElementById('tab-' + tabId).classList.add('active');
    }

    function setMode(scroll) {
      isScroll = scroll;
      document.getElementById('modeScroll').classList.toggle('selected', scroll);
      document.getElementById('modeStatic').classList.toggle('selected', !scroll);
      document.getElementById('speedGroup').style.display = scroll ? 'block' : 'none';
      updatePreview();
    }

    function setColor(color) {
      document.getElementById('clipColor').value = color;
      updatePreview();
    }

    function setSize(size, btn) {
      currentFontSize = size;
      btn.parentElement.querySelectorAll('.btn-opt').forEach(b => b.classList.remove('selected'));
      btn.classList.add('selected');
      updatePreview();
    }

    function updateSpeedVal(val) {
      document.getElementById('speedVal').innerText = val;
      const duration = (11 - val) * 0.8;
      document.getElementById('previewText').style.animationDuration = duration + 's';
    }

    function updatePreview() {
      const text = document.getElementById('clipText').value || ' ';
      const color = document.getElementById('clipColor').value;
      const preview = document.getElementById('previewText');
      preview.innerText = text;
      preview.style.color = color;
      preview.style.fontSize = (currentFontSize * 7 + 4) + 'px';

      if (isScroll) {
        preview.classList.add('marquee');
        updateSpeedVal(document.getElementById('clipSpeed').value);
      } else {
        preview.classList.remove('marquee');
      }
    }

    function showToast(msg) {
      const t = document.getElementById('toast');
      t.innerText = msg;
      t.style.display = 'block';
      setTimeout(() => t.style.display = 'none', 3000);
    }

    // Enviar configuración de texto
    async function sendText() {
      const text = document.getElementById('clipText').value;
      const colorHex = document.getElementById('clipColor').value.replace('#', '');
      const color = parseInt(colorHex, 16);
      const speed = parseInt(document.getElementById('clipSpeed').value);

      const payload = {
        text: text,
        color: color,
        fontSize: currentFontSize,
        scrollSpeed: speed,
        scrollMode: isScroll
      };

      try {
        const res = await fetch('/api/text', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(payload)
        });
        if (res.ok) showToast('✅ Texto aplicado en pantalla');
        else showToast('❌ Error al enviar');
      } catch (e) {
        showToast('❌ Error de conexión');
      }
    }

    // Renderizar texto con emojis nativos en Canvas y enviar
    function sendAsEmojiBanner() {
      const text = document.getElementById('clipText').value || ' ';
      const color = document.getElementById('clipColor').value;
      const canvas = document.getElementById('imageCanvas');
      const ctx = canvas.getContext('2d');

      ctx.fillStyle = '#000000';
      ctx.fillRect(0, 0, 320, 170);

      // Calcular tamaño de fuente según currentFontSize (P: 26px, M: 36px, G: 48px, XL: 60px)
      const fontSize = currentFontSize * 11 + 4;
      ctx.font = `bold ${fontSize}px -apple-system, BlinkMacSystemFont, "Apple Color Emoji", "Segoe UI Emoji", sans-serif`;
      ctx.fillStyle = color;
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';

      ctx.fillText(text, 160, 85);

      selectedImage = true;
      document.getElementById('btnUploadImage').disabled = false;
      uploadImage();
    }

    // Manejo y subida de imágenes con redimensión Canvas a 320x170 y conversión RGB565 en el iPhone
    function handleFileSelect(e) {
      const file = e.target.files[0];
      if (!file) return;
      const reader = new FileReader();
      reader.onload = function(evt) {
        const img = new Image();
        img.onload = function() {
          selectedImage = img;
          renderCanvas();
          document.getElementById('btnUploadImage').disabled = false;
        };
        img.src = evt.target.result;
      };
      reader.readAsDataURL(file);
    }

    function setImageFit(mode) {
      imageFitMode = mode;
      document.getElementById('fitCover').classList.toggle('selected', mode === 'cover');
      document.getElementById('fitContain').classList.toggle('selected', mode === 'contain');
      if (selectedImage) renderCanvas();
    }

    function renderCanvas() {
      if (!selectedImage) return;
      const canvas = document.getElementById('imageCanvas');
      const ctx = canvas.getContext('2d');
      ctx.fillStyle = '#000000';
      ctx.fillRect(0, 0, 320, 170);

      const iw = selectedImage.width;
      const ih = selectedImage.height;
      const cw = 320;
      const ch = 170;

      let dx = 0, dy = 0, dw = cw, dh = ch;

      if (imageFitMode === 'cover') {
        const scale = Math.max(cw / iw, ch / ih);
        dw = iw * scale;
        dh = ih * scale;
        dx = (cw - dw) / 2;
        dy = (ch - dh) / 2;
      } else {
        const scale = Math.min(cw / iw, ch / ih);
        dw = iw * scale;
        dh = ih * scale;
        dx = (cw - dw) / 2;
        dy = (ch - dh) / 2;
      }

      ctx.drawImage(selectedImage, dx, dy, dw, dh);
    }

    async function uploadImage() {
      const canvas = document.getElementById('imageCanvas');
      const ctx = canvas.getContext('2d');
      const imgData = ctx.getImageData(0, 0, 320, 170).data;

      // Convertir RGBA (4 bytes) a RGB565 (2 bytes little-endian)
      const buffer = new Uint8Array(320 * 170 * 2);
      let p = 0;
      for (let i = 0; i < imgData.length; i += 4) {
        const r = imgData[i];
        const g = imgData[i + 1];
        const b = imgData[i + 2];
        const rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        // Little endian
        buffer[p++] = rgb565 & 0xFF;
        buffer[p++] = (rgb565 >> 8) & 0xFF;
      }

      showToast('📤 Subiendo imagen...');
      try {
        const blob = new Blob([buffer], { type: 'application/octet-stream' });
        const formData = new FormData();
        formData.append('file', blob, 'image.raw');

        const res = await fetch('/api/image', {
          method: 'POST',
          body: formData
        });
        if (res.ok) showToast('✅ Imagen cargada en pantalla');
        else showToast('❌ Error en subida de imagen');
      } catch (e) {
        showToast('❌ Error de conexión al subir imagen');
      }
    }

    // Actualizar rotación
    async function setRotation(r) {
      document.getElementById('rotNormal').classList.toggle('selected', r === 1);
      document.getElementById('rotInverted').classList.toggle('selected', r === 3);
      try {
        await fetch('/api/settings', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ rotation: r })
        });
        showToast('🔄 Pantalla girada');
      } catch(e) {
        showToast('❌ Error al girar pantalla');
      }
    }

    // Actualizar swap bytes de imagen
    async function setSwapBytes(s) {
      document.getElementById('swapTrue').classList.toggle('selected', s === true);
      document.getElementById('swapFalse').classList.toggle('selected', s === false);
      try {
        await fetch('/api/settings', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ swapBytes: s })
        });
        showToast('🎨 Modo de color actualizado');
      } catch(e) {
        showToast('❌ Error al cambiar color');
      }
    }

    // Actualizar brillo
    let brightTimeout = null;
    function updateBrightness(val) {
      document.getElementById('brightVal').innerText = val;
      clearTimeout(brightTimeout);
      brightTimeout = setTimeout(async () => {
        try {
          await fetch('/api/settings', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ brightness: parseInt(val) })
          });
        } catch(e) {}
      }, 100);
    }

    // Guardar y salir
    async function applyAndExit() {
      try {
        await fetch('/api/apply', { method: 'POST' });
        showToast('💾 Guardado. WiFi apagándose...');
      } catch (e) {
        showToast('💾 Guardado y listo.');
      }
    }

    // Inicializar valores al cargar
    window.onload = async function() {
      updatePreview();
      try {
        const res = await fetch('/api/status');
        if (res.ok) {
          const data = await res.json();
          if (data.text) document.getElementById('clipText').value = data.text;
          if (data.color) {
            const hex = '#' + data.color.toString(16).padStart(6, '0');
            document.getElementById('clipColor').value = hex;
          }
          if (data.fontSize) currentFontSize = data.fontSize;
          if (data.scrollSpeed) {
            document.getElementById('clipSpeed').value = data.scrollSpeed;
            document.getElementById('speedVal').innerText = data.scrollSpeed;
          }
          if (data.brightness) {
            document.getElementById('clipBrightness').value = data.brightness;
            document.getElementById('brightVal').innerText = data.brightness;
          }
          if (data.rotation) {
            document.getElementById('rotNormal').classList.toggle('selected', data.rotation === 1);
            document.getElementById('rotInverted').classList.toggle('selected', data.rotation === 3);
          }
          if (data.swapBytes !== undefined) {
            document.getElementById('swapTrue').classList.toggle('selected', data.swapBytes === true);
            document.getElementById('swapFalse').classList.toggle('selected', data.swapBytes === false);
          }
          setMode(data.scrollMode !== false);
        }
      } catch(e) {}
    };
  </script>
</body>
</html>
)rawliteral";
