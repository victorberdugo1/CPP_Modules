// ============================================================================
// VideoExportJS - Grabación de canvas a MP4 usando MediaRecorder + FFmpeg.wasm
// Usa el core directamente (ffmpeg-core.js) sin la capa FFmpeg
// ============================================================================

(function() {
    'use strict';

    let g_stream = null;
    let g_mediaRecorder = null;
    let g_recordedChunks = [];
    let g_isRecording = false;
    let g_ffmpegReady = false;
    let g_ffmpegModule = null;

    // ========================================================================
    // CARGA DEL CORE DE FFMPEG
    // ========================================================================

    function loadCoreScript() {
        return new Promise((resolve, reject) => {
            if (typeof createFFmpegCore !== 'undefined') {
                resolve();
                return;
            }

            const script = document.createElement('script');
            script.src = '/ffmpeg/ffmpeg-core.js';
            script.onload = () => {
                console.log('[VideoExport] ffmpeg-core.js cargado');
                let attempts = 0;
                const interval = setInterval(() => {
                    if (typeof createFFmpegCore !== 'undefined') {
                        clearInterval(interval);
                        console.log('[VideoExport] createFFmpegCore detectado');
                        resolve();
                    } else if (++attempts > 300) {
                        clearInterval(interval);
                        reject(new Error('Timeout esperando createFFmpegCore'));
                    }
                }, 100);
            };
            script.onerror = () => {
                reject(new Error('Error al cargar ffmpeg-core.js'));
            };
            document.head.appendChild(script);
        });
    }

    async function initFFmpeg() {
        try {
            await loadCoreScript();

            if (typeof createFFmpegCore !== 'function') {
                throw new Error('createFFmpegCore no es una función');
            }

            console.log('[VideoExport] Inicializando FFmpeg core...');
            const module = await createFFmpegCore({
                locateFile: (path) => {
                    if (path.endsWith('.wasm')) {
                        return '/ffmpeg/ffmpeg-core.wasm';
                    }
                    return path;
                }
            });

            g_ffmpegModule = module;
            g_ffmpegReady = true;
            console.log('[VideoExport] ✓ FFmpeg core cargado correctamente');
            return true;
        } catch (err) {
            console.error('[VideoExport] Error al cargar FFmpeg core:', err);
            g_ffmpegReady = false;
            return false;
        }
    }

    // ========================================================================
    // VIDEO EXPORT API
    // ========================================================================

    const VideoExportJS = {
        startEncoder: function(width, height, fps) {
            console.log(`[VideoExport] startEncoder(${width}x${height}, ${fps}fps)`);
            const canvas = document.querySelector('canvas');
            if (!canvas) {
                console.error('[VideoExport] No canvas encontrado');
                return;
            }
            try {
                g_stream = canvas.captureStream(fps);
            } catch (err) {
                console.error('[VideoExport] captureStream no soportado:', err);
                return;
            }
            
            let mimeType = "";
            const candidates = [
                "video/webm;codecs=vp9",
                "video/webm;codecs=vp8",
                "video/webm",
                ""
            ];
            
            for (const c of candidates) {
                if (c === "" || MediaRecorder.isTypeSupported(c)) {
                    mimeType = c;
                    break;
                }
            }
            
            const options = {
                videoBitsPerSecond: 2500000
            };
            
            if (mimeType !== "")
                options.mimeType = mimeType;
            
            try {
                g_mediaRecorder = new MediaRecorder(g_stream, options);
            } catch (err) {
                console.error('[VideoExport] MediaRecorder error:', err);
                return;
            }
            
            console.log('[VideoExport] MediaRecorder:', g_mediaRecorder.mimeType);
            
            g_recordedChunks = [];
            g_isRecording = true;
            g_mediaRecorder.ondataavailable = (event) => {
                if (event.data && event.data.size > 0) {
                    g_recordedChunks.push(event.data);
                }
            };
            g_mediaRecorder.onerror = (event) => {
                console.error('[VideoExport] MediaRecorder error:', event.error);
            };
            g_mediaRecorder.start(100);
            console.log('[VideoExport] ✓ Grabación iniciada');
        },

        encodeFrame: function() {
            // MediaRecorder captura automáticamente
        },

        finishEncoder: async function(filename) {
            console.log('[VideoExport] finishEncoder()');
            
            if (!g_isRecording) {
                console.warn('[VideoExport] No hay grabación activa');
                return;
            }
            
            g_isRecording = false;
            
            await new Promise((resolve, reject) => {
                g_mediaRecorder.onstop = () => {
                    resolve();
                };
                
                g_mediaRecorder.onerror = e => {
                    reject(e.error || e);
                };
                
                g_mediaRecorder.requestData();
                
                setTimeout(() => {
                    try {
                        g_mediaRecorder.stop();
                    } catch(e) {}
                }, 0);
            });

            const blob = new Blob(g_recordedChunks, {
                type: g_mediaRecorder.mimeType || "video/webm"
            });
            
            console.log("[VideoExport] Chunks:", g_recordedChunks.length);
            console.log("[VideoExport] Blob:", blob.size);
            
            if (blob.size === 0) {
                console.error("[VideoExport] Blob vacío");
                this._cleanup();
                return;
            }
            
            console.log(`[VideoExport] WebM grabado: ${(blob.size / 1024 / 1024).toFixed(2)} MB`);
            
            if (g_ffmpegReady && g_ffmpegModule) {
                await this._convertToMP4(blob, filename);
            } else {
                console.log('[VideoExport] FFmpeg no disponible, descargando como WebM');
                const webmName = filename.replace('.mp4', '.webm');
                this._downloadBlob(blob, webmName);
            }
            
            this._cleanup();
        },

        _convertToMP4: async function(webmBlob, filename) {
            try {
                if (!g_ffmpegReady || !g_ffmpegModule) {
                    throw new Error('FFmpeg no listo');
                }
                
                const Module = g_ffmpegModule;
                
                console.log('[VideoExport] Convirtiendo a MP4...');
                const webmData = new Uint8Array(await webmBlob.arrayBuffer());
                
                console.log("[VideoExport] Input:", webmData.length);
                
                if (webmData.length === 0)
                    throw new Error("input.webm vacío");
                
                Module.FS.writeFile('/input.webm', webmData);
                
                const args = [
                    '-i', '/input.webm',
                    '-c:v', 'libx264',
                    '-preset', 'ultrafast',
                    '-crf', '23',
                    '-c:a', 'aac',
                    '-b:a', '128k',
                    '/output.mp4'
                ];
                
                const ret = Module.exec(...args);
                if (ret !== 0) {
                    throw new Error(`FFmpeg retornó código ${ret}`);
                }
                
                const data = Module.FS.readFile('/output.mp4');
                const mp4Blob = new Blob([data], { type: 'video/mp4' });
                console.log(`[VideoExport] ✓ MP4 generado: ${(mp4Blob.size / 1024 / 1024).toFixed(2)} MB`);
                
                this._downloadBlob(mp4Blob, filename);
                
                try {
                    Module.FS.unlink('/input.webm');
                    Module.FS.unlink('/output.mp4');
                } catch (e) {}
                
            } catch (err) {
                console.error('[VideoExport] Error en conversión MP4:', err);
                console.log('[VideoExport] Fallback a WebM');
                const webmName = filename.replace('.mp4', '.webm');
                this._downloadBlob(webmBlob, webmName);
            }
        },

        _downloadBlob: function(blob, filename) {
            try {
                const url = URL.createObjectURL(blob);
                const link = document.createElement('a');
                link.href = url;
                link.download = filename;
                link.style.display = 'none';
                document.body.appendChild(link);
                console.log(`[VideoExport] Iniciando descarga: ${filename}`);
                link.click();
                console.log('[VideoExport] ✓ Descargado');
                setTimeout(() => {
                    if (link.parentNode) link.parentNode.removeChild(link);
                    URL.revokeObjectURL(url);
                }, 2000);
            } catch (e) {
                console.error('[VideoExport] Error en descarga:', e);
            }
        },

        _cleanup: function() {
            if (g_stream) {
                g_stream.getTracks().forEach(t => t.stop());
            }
            g_recordedChunks = [];
            g_mediaRecorder = null;
            g_isRecording = false;
        }
    };

    window.VideoExportJS = VideoExportJS;

    // ========================================================================
    // INICIALIZACIÓN AUTOMÁTICA
    // ========================================================================

    function initWhenReady() {
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', initWhenReady);
            return;
        }
        console.log('[VideoExport] Inicializando...');
        initFFmpeg().then(ok => {
            console.log(`[VideoExport] ${ok ? '✓ FFmpeg listo' : '⚠ FFmpeg fallback a WebM'}`);
        });
    }

    initWhenReady();
})();
