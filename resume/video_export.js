(function() {
    'use strict';

    let g_mediaRecorder = null;
    let g_recordedChunks = [];
    let g_canvas = null;
    let g_stream = null;

    const VideoExportJS = {
        startEncoder: function(width, height, fps) {
            console.log(`[VideoExport] Iniciando grabación: ${width}x${height} @ ${fps}fps`);
            
            g_canvas = Module.canvas;
            if (!g_canvas) {
                console.error('Canvas no encontrado');
                return;
            }

            try {
                g_stream = g_canvas.captureStream(fps);
                
                const options = {
                    mimeType: 'video/webm;codecs=vp8',
                    videoBitsPerSecond: 2500000
                };
                
                if (!MediaRecorder.isTypeSupported(options.mimeType)) {
                    console.warn('VP8 no soportado, usando codec default');
                    g_mediaRecorder = new MediaRecorder(g_stream);
                } else {
                    g_mediaRecorder = new MediaRecorder(g_stream, options);
                }
                
                g_recordedChunks = [];
                
                g_mediaRecorder.ondataavailable = (event) => {
                    console.log(`[VideoExport] Chunk: ${event.data.size} bytes`);
                    if (event.data.size > 0) {
                        g_recordedChunks.push(event.data);
                    }
                };
                
                g_mediaRecorder.onerror = (event) => {
                    console.error('[VideoExport] Error:', event.error);
                };
                
                // Asignar onstop ANTES de start
                g_mediaRecorder.onstop = () => {
                    console.log(`[VideoExport] Grabación detenida, ${g_recordedChunks.length} chunks`);
                    
                    try {
                        const blob = new Blob(g_recordedChunks, { type: 'video/webm' });
                        console.log(`[VideoExport] Blob creado: ${(blob.size / 1024 / 1024).toFixed(2)} MB`);
                        
                        const url = URL.createObjectURL(blob);
                        const a = document.createElement('a');
                        a.href = url;
                        a.download = 'chess_game.webm';
                        document.body.appendChild(a);
                        console.log('[VideoExport] Disparando descarga...');
                        a.click();
                        
                        setTimeout(() => {
                            document.body.removeChild(a);
                            URL.revokeObjectURL(url);
                            console.log('[VideoExport] Limpieza completada');
                        }, 500);
                        
                        if (g_stream) {
                            g_stream.getTracks().forEach(track => track.stop());
                        }
                        g_mediaRecorder = null;
                        g_recordedChunks = [];
                        
                    } catch (e) {
                        console.error('[VideoExport] Error:', e);
                    }
                };
                
                g_mediaRecorder.start();
                console.log('[VideoExport] Grabación activa');
                
            } catch (e) {
                console.error('[VideoExport] Error init:', e);
            }
        },

        encodeFrame: function(rgbaData) {
            // MediaRecorder captura automáticamente
        },

        finishEncoder: function(filename) {
            console.log('[VideoExport] finishEncoder llamado');
            
            if (!g_mediaRecorder) {
                console.error('MediaRecorder no activo');
                return;
            }

            console.log('[VideoExport] Llamando stop()...');
            g_mediaRecorder.stop();
        }
    };

    window.VideoExportJS = VideoExportJS;
    console.log('[VideoExport] Listo');
})();