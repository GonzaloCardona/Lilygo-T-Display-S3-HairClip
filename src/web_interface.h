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
      --bg: #0d0f17;
      --card: #161924;
      --accent: #ff4d8d;
      --accent-glow: rgba(255, 77, 141, 0.4);
      --text: #f0f3fa;
      --text-muted: #8c93aa;
      --border: #262a3d;
      --success: #00e676;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Apple Color Emoji", sans-serif; }
    body { background: var(--bg); color: var(--text); padding: 12px; display: flex; flex-direction: column; align-items: center; min-height: 100vh; }
    .container { width: 100%; max-width: 430px; }
    header { text-align: center; margin-bottom: 14px; }
    header h1 { font-size: 1.45rem; color: var(--accent); font-weight: 800; letter-spacing: -0.5px; text-shadow: 0 0 16px var(--accent-glow); }
    header p { font-size: 0.8rem; color: var(--text-muted); margin-top: 2px; }

    /* Tabs */
    .nav-tabs { display: flex; background: var(--card); border-radius: 12px; padding: 4px; margin-bottom: 14px; border: 1px solid var(--border); }
    .tab-btn { flex: 1; padding: 9px 0; background: none; border: none; color: var(--text-muted); font-size: 0.88rem; font-weight: 600; cursor: pointer; border-radius: 8px; transition: all 0.2s; }
    .tab-btn.active { background: var(--accent); color: white; box-shadow: 0 2px 8px var(--accent-glow); }

    /* Card Panels */
    .tab-content { display: none; background: var(--card); border-radius: 16px; padding: 16px; border: 1px solid var(--border); box-shadow: 0 4px 20px rgba(0,0,0,0.4); margin-bottom: 14px; }
    .tab-content.active { display: block; animation: fadeIn 0.2s ease; }
    @keyframes fadeIn { from { opacity: 0; transform: translateY(4px); } to { opacity: 1; transform: translateY(0); } }

    .form-group { margin-bottom: 14px; }
    label { display: flex; justify-content: space-between; align-items: center; font-size: 0.78rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.5px; color: var(--text-muted); margin-bottom: 6px; }
    .label-badge { color: var(--accent); font-size: 0.82rem; font-weight: 700; text-transform: none; }
    input[type="text"] { width: 100%; padding: 11px 13px; background: #10121d; border: 1px solid var(--border); border-radius: 10px; color: var(--text); font-size: 1rem; outline: none; }
    input[type="text"]:focus { border-color: var(--accent); box-shadow: 0 0 8px var(--accent-glow); }

    /* Button Toggles */
    .btn-group { display: flex; gap: 8px; }
    .btn-opt { flex: 1; padding: 9px; background: #10121d; border: 1px solid var(--border); border-radius: 10px; color: var(--text); font-size: 0.85rem; font-weight: 600; cursor: pointer; transition: 0.15s; }
    .btn-opt.selected { background: rgba(255, 77, 141, 0.18); border-color: var(--accent); color: var(--accent); font-weight: 700; }

    /* Color Pickers */
    .color-row { display: flex; align-items: center; gap: 8px; }
    input[type="color"] { -webkit-appearance: none; border: none; width: 40px; height: 40px; border-radius: 10px; cursor: pointer; background: transparent; }
    input[type="color"]::-webkit-color-swatch-wrapper { padding: 0; }
    input[type="color"]::-webkit-color-swatch { border: 2px solid var(--border); border-radius: 10px; }
    .presets { display: flex; gap: 6px; flex-wrap: wrap; }
    .chip { width: 28px; height: 28px; border-radius: 7px; border: 1.5px solid var(--border); cursor: pointer; transition: transform 0.1s; }
    .chip:active { transform: scale(0.9); }

    /* Sliders */
    .slider-container { display: flex; align-items: center; gap: 10px; }
    input[type="range"] { flex: 1; accent-color: var(--accent); height: 6px; border-radius: 3px; background: #222638; outline: none; }
    .slider-val { min-width: 38px; text-align: right; font-size: 0.85rem; font-weight: 700; color: var(--accent); }

    /* Screen Canvas Previews (Aspect 320x170) */
    .screen-preview-wrap { position: relative; width: 100%; aspect-ratio: 320 / 170; background: #000; border-radius: 12px; border: 2px solid var(--border); overflow: hidden; box-shadow: inset 0 0 15px rgba(0,0,0,0.9); margin-top: 4px; }
    .screen-canvas { width: 100%; height: 100%; display: block; image-rendering: pixelated; }
    .canvas-overlay-hint { position: absolute; bottom: 6px; left: 50%; transform: translateX(-50%); background: rgba(0,0,0,0.65); padding: 3px 8px; border-radius: 6px; font-size: 0.68rem; color: #cbd5e1; pointer-events: none; }

    /* Action Buttons */
    .btn-primary { width: 100%; padding: 13px; background: var(--accent); border: none; border-radius: 12px; color: white; font-size: 0.98rem; font-weight: 700; cursor: pointer; box-shadow: 0 4px 14px var(--accent-glow); transition: 0.15s; }
    .btn-primary:active { transform: scale(0.98); }
    .btn-primary:disabled { opacity: 0.45; cursor: not-allowed; }
    .btn-save { width: 100%; padding: 13px; background: #10121d; border: 1px solid var(--success); border-radius: 12px; color: var(--success); font-size: 0.98rem; font-weight: 700; cursor: pointer; margin-top: 6px; }

    .file-btn { display: block; text-align: center; width: 100%; padding: 11px; background: #1c2030; border: 1px dashed var(--accent); border-radius: 10px; color: var(--text); font-weight: 600; cursor: pointer; font-size: 0.9rem; }
    input[type="file"] { display: none; }

    /* Toast Notification */
    #toast { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%); background: #1f2333; color: white; padding: 9px 18px; border-radius: 20px; font-size: 0.82rem; font-weight: 600; border: 1px solid var(--border); display: none; z-index: 100; box-shadow: 0 4px 15px rgba(0,0,0,0.6); }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>🎀 HairClip Digital</h1>
      <p>LilyGO T-Display S3 • Control Inalámbrico</p>
    </header>

    <div class="nav-tabs">
      <button class="tab-btn active" onclick="switchTab('text')">Texto & Emojis</button>
      <button class="tab-btn" onclick="switchTab('image')">Foto / GIF</button>
      <button class="tab-btn" onclick="switchTab('settings')">Ajustes</button>
    </div>

    <!-- TAB 1: TEXTO -->
    <div id="tab-text" class="tab-content active">
      <div class="form-group">
        <label>Mensaje (con tildes, ñ y emojis)</label>
        <input type="text" id="clipText" value="🎀 ¡Hola España! ✨" oninput="onTextInput()" placeholder="Escribe tu texto o emojis...">
      </div>

      <div class="form-group">
        <label>Modo de Pantalla</label>
        <div class="btn-group">
          <button type="button" class="btn-opt" id="modeStatic" onclick="setTextScrollMode(false)">Estático (Centrado)</button>
          <button type="button" class="btn-opt selected" id="modeScroll" onclick="setTextScrollMode(true)">En Movimiento (Scroll)</button>
        </div>
      </div>

      <div class="form-group">
        <label>Tamaño de Letra <span class="label-badge" id="fontSizeBadge">44 px</span></label>
        <div class="slider-container">
          <input type="range" id="clipFontSize" min="20" max="90" value="44" oninput="onFontSizeInput(this.value)">
        </div>
      </div>

      <div class="form-group">
        <label>Color del Texto</label>
        <div class="color-row">
          <input type="color" id="clipTextColor" value="#ff69b4" onchange="onTextColorChange(this.value)">
          <div class="presets">
            <div class="chip" style="background:#ff69b4" onclick="setTextColor('#ff69b4')"></div>
            <div class="chip" style="background:#00f5d4" onclick="setTextColor('#00f5d4')"></div>
            <div class="chip" style="background:#fee440" onclick="setTextColor('#fee440')"></div>
            <div class="chip" style="background:#9b5de5" onclick="setTextColor('#9b5de5')"></div>
            <div class="chip" style="background:#ff595e" onclick="setTextColor('#ff595e')"></div>
            <div class="chip" style="background:#ffffff" onclick="setTextColor('#ffffff')"></div>
          </div>
        </div>
      </div>

      <div class="form-group">
        <label>Color de Fondo</label>
        <div class="color-row">
          <input type="color" id="clipBgColor" value="#000000" onchange="onBgColorChange(this.value)">
          <div class="presets">
            <div class="chip" style="background:#000000" onclick="setTextBgColor('#000000')"></div>
            <div class="chip" style="background:#1a102f" onclick="setTextBgColor('#1a102f')"></div>
            <div class="chip" style="background:#2b0938" onclick="setTextBgColor('#2b0938')"></div>
            <div class="chip" style="background:#ff69b4" onclick="setTextBgColor('#ff69b4')"></div>
            <div class="chip" style="background:#003366" onclick="setTextBgColor('#003366')"></div>
            <div class="chip" style="background:#ffffff" onclick="setTextBgColor('#ffffff')"></div>
          </div>
        </div>
      </div>

      <div class="form-group" id="speedGroup">
        <label>Velocidad de Movimiento <span class="label-badge" id="speedVal">3</span></label>
        <div class="slider-container">
          <input type="range" id="clipSpeed" min="1" max="10" value="3" oninput="updateSpeedVal(this.value)">
        </div>
      </div>

      <div class="form-group">
        <label>Vista Previa Exacta (320 × 170 px)</label>
        <div class="screen-preview-wrap">
          <canvas id="textPreviewCanvas" class="screen-canvas" width="320" height="170"></canvas>
        </div>
      </div>

      <button class="btn-primary" id="btnSendText" onclick="uploadTextBanner()">🚀 Enviar a Pantalla</button>
    </div>

    <!-- TAB 2: FOTO O GIF -->
    <div id="tab-image" class="tab-content">
      <div class="form-group">
        <label>Elegir Foto o GIF Animado</label>
        <label class="file-btn" for="fileInput">📁 Seleccionar Foto o GIF desde iPhone</label>
        <input type="file" id="fileInput" accept="image/*" onchange="handleFileSelect(event)">
        <div id="gifBadge" style="display:none; color:var(--accent); font-weight:700; font-size:0.83rem; margin-top:6px; text-align:center;">🎬 GIF Animado detectado</div>
        <div id="freeSpaceInfo" style="font-size:0.78rem; color:var(--text-muted); text-align:center; margin-top:6px;"></div>
      </div>

      <div class="form-group" id="fitModeGroup">
        <label>Modo de Ajuste</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="fitCover" onclick="setImageFit('cover')">Recortar (Llenar)</button>
          <button type="button" class="btn-opt" id="fitContain" onclick="setImageFit('contain')">Ajustar (Bordes)</button>
        </div>
      </div>

      <!-- Controles de Desplazamiento y Zoom (Modo Recortar/Cover) -->
      <div id="panControlsGroup">
        <div class="form-group">
          <label>Desplazamiento Horizontal <span class="label-badge" id="panXBadge">0%</span></label>
          <div class="slider-container">
            <input type="range" id="panXSlider" min="-100" max="100" value="0" oninput="onPanSliderChange()">
          </div>
        </div>

        <div class="form-group">
          <label>Desplazamiento Vertical <span class="label-badge" id="panYBadge">0%</span></label>
          <div class="slider-container">
            <input type="range" id="panYSlider" min="-100" max="100" value="0" oninput="onPanSliderChange()">
          </div>
        </div>

        <div class="form-group">
          <label>Zoom / Escala <span class="label-badge" id="zoomBadge">100%</span></label>
          <div class="slider-container">
            <input type="range" id="zoomSlider" min="100" max="300" value="100" oninput="onZoomSliderChange()">
          </div>
        </div>

        <div style="margin-bottom: 12px; text-align: right;">
          <button type="button" class="btn-opt" style="padding: 6px 14px; font-size: 0.8rem;" onclick="resetPan()">🔄 Centrar Encuadre</button>
        </div>
      </div>

      <!-- Color de fondo para bordes (Modo Ajustar/Contain o GIF) -->
      <div class="form-group" id="imgBgGroup" style="display:none;">
        <label>Color de Fondo / Bordes</label>
        <div class="color-row">
          <input type="color" id="imgBgColor" value="#000000" onchange="onImgBgColorChange(this.value)">
          <div class="presets">
            <div class="chip" style="background:#000000" onclick="setImgBgColor('#000000')"></div>
            <div class="chip" style="background:#ffffff" onclick="setImgBgColor('#ffffff')"></div>
            <div class="chip" style="background:#ff69b4" onclick="setImgBgColor('#ff69b4')"></div>
            <div class="chip" style="background:#00f5d4" onclick="setImgBgColor('#00f5d4')"></div>
            <div class="chip" style="background:#9b5de5" onclick="setImgBgColor('#9b5de5')"></div>
          </div>
        </div>
      </div>

      <div class="form-group">
        <label>Vista Previa (320 × 170 px)</label>
        <div class="screen-preview-wrap">
          <canvas id="imageCanvas" class="screen-canvas" width="320" height="170"></canvas>
          <img id="gifPreview" style="display:none; width:100%; height:100%; object-fit:contain; border-radius:10px;">
          <div class="canvas-overlay-hint" id="dragHint">👆 Arrastra con el dedo para encuadrar</div>
        </div>
      </div>

      <button class="btn-primary" id="btnUploadImage" onclick="onUploadMediaClick()" disabled>📤 Enviar a Pantalla</button>
    </div>

    <!-- TAB 3: AJUSTES -->
    <div id="tab-settings" class="tab-content">
      <div class="form-group">
        <label>Brillo de Pantalla <span class="label-badge" id="brightVal">160</span></label>
        <div class="slider-container">
          <input type="range" id="clipBrightness" min="20" max="255" value="160" oninput="updateBrightness(this.value)">
        </div>
      </div>

      <div class="form-group">
        <label>Orientación de Pantalla</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="rotNormal" onclick="setRotation(1)">Normal (USB derecha)</button>
          <button type="button" class="btn-opt" id="rotInverted" onclick="setRotation(3)">Invertida (USB izquierda)</button>
        </div>
      </div>

      <div class="form-group">
        <label>Orden de Color RGB565 (Swap Bytes)</label>
        <div class="btn-group">
          <button type="button" class="btn-opt selected" id="swapTrue" onclick="setSwapBytes(true)">Corregido (Normal)</button>
          <button type="button" class="btn-opt" id="swapFalse" onclick="setSwapBytes(false)">Invertir</button>
        </div>
      </div>

      <div class="form-group" style="background:#10121d; padding:12px; border-radius:10px; font-size:0.83rem; color:var(--text-muted); line-height:1.5;">
        <strong style="color:var(--text);">💡 Botones físicos:</strong><br>
        • <strong>Botón User (GPIO 14)</strong>: Mantén pulsado >1.2s para suspender en reposo profundo (pantalla apagada).<br>
        • <strong>Despertar</strong>: Vuelve a presionar el botón User.<br>
        • <strong>Botón BOOT (GPIO 0)</strong>: Pulsa para activar el WiFi y entrar a esta web.
      </div>
    </div>

    <!-- BOTÓN FINAL -->
    <button class="btn-save" onclick="applyAndExit()">💾 Guardar y Volver a Mostrar</button>
  </div>

  <div id="toast"></div>

  <script>
    // ==========================================================
    // 🎀 ESTADO GLOBAL
    // ==========================================================
    let isScroll = true;
    let currentFontSize = 44;
    let textColor = '#ff69b4';
    let textBgColor = '#000000';
    let scrollSpeed = 3;

    // Animación de preview de texto
    let textPreviewAnimId = null;
    let previewScrollX = 0;
    let cachedBannerWidth = 320;

    // Manejo de imagen y GIF
    let loadedImage = null;
    let selectedGifFile = null;
    let imageFitMode = 'cover';
    let deviceFreeBytes = 0; // Espacio libre en LittleFS (actualizado al cargar)
    let imgBgColor = '#000000';
    let panX = 0; // -100 a +100
    let panY = 0; // -100 a +100
    let zoomLevel = 1.0; // 1.0 a 3.0

    // Touch / Drag en Canvas de imagen
    let isDragging = false;
    let dragStartX = 0, dragStartY = 0;
    let panStartX = 0, panStartY = 0;

    // ==========================================================
    // 🗂️ GESTIÓN DE TABS
    // ==========================================================
    function switchTab(tabId) {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
      event.target.classList.add('active');
      document.getElementById('tab-' + tabId).classList.add('active');

      if (tabId === 'text') {
        startTextPreviewLoop();
      } else {
        stopTextPreviewLoop();
        if (tabId === 'image') renderImageCanvas();
      }
    }

    // ==========================================================
    // 📝 TAB 1: TEXTO, EMOJIS & PREVIEW EXACTA
    // ==========================================================
    function onTextInput() {
      previewScrollX = 0;
      updateTextBannerMetrics();
    }

    function setTextScrollMode(scroll) {
      isScroll = scroll;
      document.getElementById('modeScroll').classList.toggle('selected', scroll);
      document.getElementById('modeStatic').classList.toggle('selected', !scroll);
      document.getElementById('speedGroup').style.display = scroll ? 'block' : 'none';
      previewScrollX = 0;
      updateTextBannerMetrics();
    }

    function onFontSizeInput(val) {
      currentFontSize = parseInt(val);
      document.getElementById('fontSizeBadge').innerText = val + ' px';
      previewScrollX = 0;
      updateTextBannerMetrics();
    }

    function onTextColorChange(val) {
      textColor = val;
      updateTextBannerMetrics();
    }

    function setTextColor(val) {
      textColor = val;
      document.getElementById('clipTextColor').value = val;
      updateTextBannerMetrics();
    }

    function onBgColorChange(val) {
      textBgColor = val;
      updateTextBannerMetrics();
    }

    function setTextBgColor(val) {
      textBgColor = val;
      document.getElementById('clipBgColor').value = val;
      updateTextBannerMetrics();
    }

    function updateSpeedVal(val) {
      scrollSpeed = parseInt(val);
      document.getElementById('speedVal').innerText = val;
    }

    function updateTextBannerMetrics() {
      const text = document.getElementById('clipText').value || ' ';
      const canvas = document.getElementById('textPreviewCanvas');
      const ctx = canvas.getContext('2d');
      ctx.font = `bold ${currentFontSize}px -apple-system, BlinkMacSystemFont, "Apple Color Emoji", "Segoe UI Emoji", Roboto, sans-serif`;
      const textMetrics = ctx.measureText(text);
      const textW = Math.ceil(textMetrics.width);

      if (isScroll) {
        cachedBannerWidth = Math.max(320, textW + 80);
      } else {
        cachedBannerWidth = 320;
      }
    }

    function startTextPreviewLoop() {
      if (textPreviewAnimId) cancelAnimationFrame(textPreviewAnimId);
      updateTextBannerMetrics();

      function step() {
        renderTextPreviewFrame();
        textPreviewAnimId = requestAnimationFrame(step);
      }
      textPreviewAnimId = requestAnimationFrame(step);
    }

    function stopTextPreviewLoop() {
      if (textPreviewAnimId) {
        cancelAnimationFrame(textPreviewAnimId);
        textPreviewAnimId = null;
      }
    }

    function renderTextPreviewFrame() {
      const canvas = document.getElementById('textPreviewCanvas');
      const ctx = canvas.getContext('2d');
      const cw = 320, ch = 170;

      // 1. Limpiar fondo con color exacto
      ctx.fillStyle = textBgColor;
      ctx.fillRect(0, 0, cw, ch);

      const text = document.getElementById('clipText').value || ' ';
      ctx.font = `bold ${currentFontSize}px -apple-system, BlinkMacSystemFont, "Apple Color Emoji", "Segoe UI Emoji", Roboto, sans-serif`;
      ctx.fillStyle = textColor;
      ctx.textBaseline = 'middle';

      if (!isScroll) {
        // Estático: Centrado
        ctx.textAlign = 'center';
        ctx.fillText(text, cw / 2, ch / 2);
      } else {
        // En movimiento: Marquee continuo exacto como en el ESP32
        ctx.textAlign = 'left';
        const gap = 60;
        
        ctx.fillText(text, previewScrollX + 20, ch / 2);

        const nextX = previewScrollX + cachedBannerWidth + gap;
        if (nextX < cw) {
          ctx.fillText(text, nextX + 20, ch / 2);
        }

        // Avance según velocidad
        const stepPx = (scrollSpeed * 0.7);
        previewScrollX -= stepPx;
        if (previewScrollX <= -(cachedBannerWidth + gap)) {
          previewScrollX += (cachedBannerWidth + gap);
        }
      }
    }

    // Convertir canvas a RGB565 Little-Endian binario
    function canvasToRGB565(canvas) {
      const ctx = canvas.getContext('2d');
      const imgData = ctx.getImageData(0, 0, canvas.width, canvas.height).data;
      const buffer = new Uint8Array(canvas.width * canvas.height * 2);
      let p = 0;
      for (let i = 0; i < imgData.length; i += 4) {
        const r = imgData[i];
        const g = imgData[i + 1];
        const b = imgData[i + 2];
        const rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        buffer[p++] = rgb565 & 0xFF;
        buffer[p++] = (rgb565 >> 8) & 0xFF;
      }
      return buffer;
    }

    // Enviar cartel de texto/emojis a la pantalla
    async function uploadTextBanner() {
      const btn = document.getElementById('btnSendText');
      btn.disabled = true;
      showToast('⏳ Generando cartel con emojis y tildes...');

      const text = document.getElementById('clipText').value || ' ';
      const offCanvas = document.createElement('canvas');
      offCanvas.height = 170;

      const offCtx = offCanvas.getContext('2d');
      offCtx.font = `bold ${currentFontSize}px -apple-system, BlinkMacSystemFont, "Apple Color Emoji", "Segoe UI Emoji", Roboto, sans-serif`;
      const textW = Math.ceil(offCtx.measureText(text).width);

      if (isScroll) {
        offCanvas.width = Math.max(320, textW + 80);
      } else {
        offCanvas.width = 320;
      }

      // Dibujar fondo
      offCtx.fillStyle = textBgColor;
      offCtx.fillRect(0, 0, offCanvas.width, 170);

      // Dibujar texto/emojis
      offCtx.font = `bold ${currentFontSize}px -apple-system, BlinkMacSystemFont, "Apple Color Emoji", "Segoe UI Emoji", Roboto, sans-serif`;
      offCtx.fillStyle = textColor;
      offCtx.textBaseline = 'middle';

      if (isScroll) {
        offCtx.textAlign = 'left';
        offCtx.fillText(text, 20, 85);
      } else {
        offCtx.textAlign = 'center';
        offCtx.fillText(text, offCanvas.width / 2, 85);
      }

      // Generar RGB565
      const rgbBuffer = canvasToRGB565(offCanvas);
      const bgHex = textBgColor.replace('#', '');
      const colorHex = textColor.replace('#', '');
      const url = `/api/image?w=${offCanvas.width}&h=170&scroll=${isScroll ? 1 : 0}&speed=${scrollSpeed}&bg=${bgHex}&fontSize=${currentFontSize}&color=${colorHex}&text=${encodeURIComponent(text)}`;

      try {
        const blob = new Blob([rgbBuffer], { type: 'application/octet-stream' });
        const formData = new FormData();
        formData.append('file', blob, 'banner.raw');

        const res = await fetch(url, { method: 'POST', body: formData });
        if (res.ok) {
          showToast('✅ ¡Cartel enviado a pantalla con éxito!');
        } else {
          showToast('❌ Error en el envío');
        }
      } catch (e) {
        showToast('❌ Error de conexión con el ESP32');
      } finally {
        btn.disabled = false;
      }
    }

    // ==========================================================
    // 🖼️ TAB 2: IMAGEN Y GIF CON PREVIEW & CONTROLES
    // ==========================================================
    function handleFileSelect(e) {
      const file = e.target.files[0];
      if (!file) return;

      // Advertir si el archivo (solo para GIF, la foto se recorta a 108 KB fijo) supera el espacio libre
      const isGif = file.type === 'image/gif' || file.name.toLowerCase().endsWith('.gif');
      if (isGif && deviceFreeBytes > 0 && file.size > deviceFreeBytes) {
        const freeKB = (deviceFreeBytes / 1024).toFixed(0);
        const fileKB = (file.size / 1024).toFixed(0);
        showToast('⚠️ El GIF pesa ' + fileKB + ' KB pero solo hay ' + freeKB + ' KB libres en el dispositivo');
      }
      selectedGifFile = isGif ? file : null;

      const reader = new FileReader();
      reader.onload = function(evt) {
        if (isGif) {
          loadedImage = null;
          document.getElementById('gifBadge').style.display = 'block';
          const gifEl = document.getElementById('gifPreview');
          gifEl.style.display = 'block';
          gifEl.src = evt.target.result;
          gifEl.style.backgroundColor = imgBgColor;
          document.getElementById('imageCanvas').style.display = 'none';
          document.getElementById('dragHint').style.display = 'none';
          document.getElementById('panControlsGroup').style.display = 'none';
          document.getElementById('fitModeGroup').style.display = 'none';
          document.getElementById('imgBgGroup').style.display = 'block';
          document.getElementById('btnUploadImage').innerText = '🎬 Enviar GIF a Pantalla';
          document.getElementById('btnUploadImage').disabled = false;
        } else {
          selectedGifFile = null;
          const img = new Image();
          img.onload = function() {
            loadedImage = img;
            document.getElementById('gifBadge').style.display = 'none';
            document.getElementById('gifPreview').style.display = 'none';
            document.getElementById('imageCanvas').style.display = 'block';
            document.getElementById('fitModeGroup').style.display = 'block';
            document.getElementById('btnUploadImage').innerText = '📤 Enviar Foto a Pantalla';
            resetPan();
            setImageFit(imageFitMode);
            document.getElementById('btnUploadImage').disabled = false;
          };
          img.src = evt.target.result;
        }
      };
      reader.readAsDataURL(file);
    }

    function setImageFit(mode) {
      imageFitMode = mode;
      document.getElementById('fitCover').classList.toggle('selected', mode === 'cover');
      document.getElementById('fitContain').classList.toggle('selected', mode === 'contain');
      document.getElementById('panControlsGroup').style.display = (mode === 'cover') ? 'block' : 'none';
      document.getElementById('imgBgGroup').style.display = (mode === 'contain') ? 'block' : 'none';
      document.getElementById('dragHint').style.display = (mode === 'cover') ? 'block' : 'none';
      renderImageCanvas();
    }

    function onImgBgColorChange(val) {
      imgBgColor = val;
      if (selectedGifFile) {
        document.getElementById('gifPreview').style.backgroundColor = imgBgColor;
      } else {
        renderImageCanvas();
      }
    }

    function setImgBgColor(val) {
      imgBgColor = val;
      document.getElementById('imgBgColor').value = val;
      onImgBgColorChange(val);
    }

    function onPanSliderChange() {
      panX = parseInt(document.getElementById('panXSlider').value);
      panY = parseInt(document.getElementById('panYSlider').value);
      document.getElementById('panXBadge').innerText = panX + '%';
      document.getElementById('panYBadge').innerText = panY + '%';
      renderImageCanvas();
    }

    function onZoomSliderChange() {
      const zVal = parseInt(document.getElementById('zoomSlider').value);
      zoomLevel = zVal / 100.0;
      document.getElementById('zoomBadge').innerText = zVal + '%';
      renderImageCanvas();
    }

    function resetPan() {
      panX = 0;
      panY = 0;
      zoomLevel = 1.0;
      document.getElementById('panXSlider').value = 0;
      document.getElementById('panYSlider').value = 0;
      document.getElementById('zoomSlider').value = 100;
      document.getElementById('panXBadge').innerText = '0%';
      document.getElementById('panYBadge').innerText = '0%';
      document.getElementById('zoomBadge').innerText = '100%';
      renderImageCanvas();
    }

    // Renderizar imagen en canvas con pan, zoom y color de bordes
    function renderImageCanvas() {
      const canvas = document.getElementById('imageCanvas');
      const ctx = canvas.getContext('2d');
      const cw = 320, ch = 170;

      // Fondo
      const bg = (imageFitMode === 'contain') ? document.getElementById('imgBgColor').value : '#000000';
      ctx.fillStyle = bg;
      ctx.fillRect(0, 0, cw, ch);

      if (!loadedImage) return;

      const iw = loadedImage.width;
      const ih = loadedImage.height;

      if (imageFitMode === 'cover') {
        const baseScale = Math.max(cw / iw, ch / ih);
        const finalScale = baseScale * zoomLevel;
        const dw = iw * finalScale;
        const dh = ih * finalScale;

        // Rango de desplazamiento
        const maxOffsetX = Math.max(0, (dw - cw) / 2);
        const maxOffsetY = Math.max(0, (dh - ch) / 2);

        const offsetX = (panX / 100.0) * maxOffsetX;
        const offsetY = (panY / 100.0) * maxOffsetY;

        const dx = (cw - dw) / 2 + offsetX;
        const dy = (ch - dh) / 2 + offsetY;

        ctx.drawImage(loadedImage, dx, dy, dw, dh);
      } else {
        // Modo Contain: encaja dentro de 320x170, bordes rellenos de imgBgColor
        const scale = Math.min(cw / iw, ch / ih);
        const dw = iw * scale;
        const dh = ih * scale;
        const dx = (cw - dw) / 2;
        const dy = (ch - dh) / 2;
        ctx.drawImage(loadedImage, dx, dy, dw, dh);
      }
    }

    // Touch & Mouse Drag para encuadrar la foto interactivamente
    const imgCanvas = document.getElementById('imageCanvas');
    function startDrag(clientX, clientY) {
      if (!loadedImage || imageFitMode !== 'cover') return;
      isDragging = true;
      dragStartX = clientX;
      dragStartY = clientY;
      panStartX = panX;
      panStartY = panY;
    }
    function moveDrag(clientX, clientY) {
      if (!isDragging) return;
      const deltaX = clientX - dragStartX;
      const deltaY = clientY - dragStartY;

      // Sensibilidad del arrastre
      let newPanX = panStartX + (deltaX / 1.2);
      let newPanY = panStartY + (deltaY / 1.2);

      newPanX = Math.max(-100, Math.min(100, newPanX));
      newPanY = Math.max(-100, Math.min(100, newPanY));

      panX = Math.round(newPanX);
      panY = Math.round(newPanY);

      document.getElementById('panXSlider').value = panX;
      document.getElementById('panYSlider').value = panY;
      document.getElementById('panXBadge').innerText = panX + '%';
      document.getElementById('panYBadge').innerText = panY + '%';
      renderImageCanvas();
    }
    function endDrag() { isDragging = false; }

    imgCanvas.addEventListener('mousedown', e => startDrag(e.clientX, e.clientY));
    window.addEventListener('mousemove', e => moveDrag(e.clientX, e.clientY));
    window.addEventListener('mouseup', endDrag);

    imgCanvas.addEventListener('touchstart', e => {
      if (e.touches.length === 1) {
        startDrag(e.touches[0].clientX, e.touches[0].clientY);
      }
    }, { passive: true });
    window.addEventListener('touchmove', e => {
      if (isDragging && e.touches.length === 1) {
        moveDrag(e.touches[0].clientX, e.touches[0].clientY);
      }
    }, { passive: true });
    window.addEventListener('touchend', endDrag);

    // Subir imagen encuadrada a pantalla
    async function uploadImageCanvas() {
      const btn = document.getElementById('btnUploadImage');
      btn.disabled = true;
      showToast('📤 Subiendo imagen encuadrada...');

      const canvas = document.getElementById('imageCanvas');
      const rgbBuffer = canvasToRGB565(canvas);
      const bgHex = ((imageFitMode === 'contain') ? document.getElementById('imgBgColor').value : '#000000').replace('#', '');
      const url = `/api/image?w=320&h=170&scroll=0&bg=${bgHex}`;

      try {
        const blob = new Blob([rgbBuffer], { type: 'application/octet-stream' });
        const formData = new FormData();
        formData.append('file', blob, 'image.raw');

        const res = await fetch(url, { method: 'POST', body: formData });
        if (res.ok) {
          showToast('✅ ¡Imagen cargada en pantalla!');
        } else {
          showToast('❌ Error al subir imagen');
        }
      } catch (e) {
        showToast('❌ Error de conexión al subir imagen');
      } finally {
        btn.disabled = false;
      }
    }

    // Subir archivo GIF animado directamente sin procesar
    async function uploadGifFile() {
      if (!selectedGifFile) return;

      // Validar espacio antes de intentar subir
      if (deviceFreeBytes > 0 && selectedGifFile.size > deviceFreeBytes) {
        const freeKB = (deviceFreeBytes / 1024).toFixed(0);
        const fileKB = (selectedGifFile.size / 1024).toFixed(0);
        showToast('❌ No hay espacio suficiente: el GIF pesa ' + fileKB + ' KB y solo hay ' + freeKB + ' KB libres');
        return;
      }

      const btn = document.getElementById('btnUploadImage');
      btn.disabled = true;
      showToast('🎬 Subiendo GIF animado al dispositivo...');

      const bgHex = document.getElementById('imgBgColor').value.replace('#', '');
      const url = `/api/gif?bg=${bgHex}`;

      try {
        const formData = new FormData();
        formData.append('file', selectedGifFile, 'image.gif');

        const res = await fetch(url, { method: 'POST', body: formData });
        if (res.ok) {
          showToast('✅ ¡GIF animado cargado con éxito!');
        } else {
          showToast('❌ Error al subir el GIF');
        }
      } catch (e) {
        showToast('❌ Error de conexión al subir GIF');
      } finally {
        btn.disabled = false;
      }
    }

    // Botón único para enviar foto o GIF
    function onUploadMediaClick() {
      if (selectedGifFile) {
        uploadGifFile();
      } else {
        uploadImageCanvas();
      }
    }

    // ==========================================================
    // ⚙️ TAB 3: AJUSTES (ROTACIÓN, SWAP BYTES, BRILLO)
    // ==========================================================
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

    // ==========================================================
    // 💾 GUARDAR Y SALIR
    // ==========================================================
    async function applyAndExit() {
      try {
        await fetch('/api/apply', { method: 'POST' });
        showToast('💾 Guardado. Apagando WiFi y volviendo a mostrar...');
      } catch (e) {
        showToast('💾 Guardado y listo.');
      }
    }

    function showToast(msg) {
      const t = document.getElementById('toast');
      t.innerText = msg;
      t.style.display = 'block';
      setTimeout(() => t.style.display = 'none', 3000);
    }

    // ==========================================================
    // 🚀 INICIALIZACIÓN
    // ==========================================================
    window.onload = async function() {
      startTextPreviewLoop();
      try {
        const res = await fetch('/api/status');
        if (res.ok) {
          const data = await res.json();
          if (data.text) document.getElementById('clipText').value = data.text;
          if (data.color) {
            textColor = '#' + data.color.toString(16).padStart(6, '0');
            document.getElementById('clipTextColor').value = textColor;
          }
          if (data.bgColor !== undefined) {
            textBgColor = '#' + data.bgColor.toString(16).padStart(6, '0');
            document.getElementById('clipBgColor').value = textBgColor;
            imgBgColor = textBgColor;
            document.getElementById('imgBgColor').value = imgBgColor;
          }
          if (data.fontSize) {
            currentFontSize = data.fontSize;
            document.getElementById('clipFontSize').value = currentFontSize;
            document.getElementById('fontSizeBadge').innerText = currentFontSize + ' px';
          }
          if (data.scrollSpeed) {
            scrollSpeed = data.scrollSpeed;
            document.getElementById('clipSpeed').value = scrollSpeed;
            document.getElementById('speedVal').innerText = scrollSpeed;
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
          setTextScrollMode(data.scrollMode !== false);
          // Espacio libre en el dispositivo
          if (data.freeSpace !== undefined) {
            deviceFreeBytes = data.freeSpace;
            const freeKB = (data.freeSpace / 1024).toFixed(0);
            const el = document.getElementById('freeSpaceInfo');
            if (el) el.innerText = 'Espacio libre en dispositivo: ' + freeKB + ' KB';
          }
        }
      } catch(e) {}
    };
  </script>
</body>
</html>
)rawliteral";
