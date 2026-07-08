#ifndef VIDEO_EXPORT_H
#define VIDEO_EXPORT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool recording;
    int frameCount;
} VideoExportState;

static VideoExportState g_videoExport = {0};

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    
    EM_JS(void, js_start_encoder, (int width, int height, int fps), {
        if (window.VideoExportJS) {
            window.VideoExportJS.startEncoder(width, height, fps);
        }
    });
    
    EM_JS(void, js_finish_encoder, (const char *filename), {
        if (window.VideoExportJS) {
            const filenameStr = UTF8ToString(filename);
            window.VideoExportJS.finishEncoder(filenameStr);
        }
    });
#endif

static inline void VideoExportInit(void) {
    g_videoExport.recording = false;
    g_videoExport.frameCount = 0;
}

static inline void VideoExportStart(int width, int height) {
    #ifdef __EMSCRIPTEN__
    if (!g_videoExport.recording) {
        g_videoExport.recording = true;
        g_videoExport.frameCount = 0;
        js_start_encoder(width, height, 60);
        printf("VideoExport: grabacion iniciada\n");
    }
    #endif
}

static inline void VideoExportCaptureFrame(void) {
    if (g_videoExport.recording) {
        g_videoExport.frameCount++;
    }
}

static inline void VideoExportStop(void) {
    #ifdef __EMSCRIPTEN__
    if (g_videoExport.recording) {
        g_videoExport.recording = false;
        printf("VideoExport: deteniendo grabacion (%d frames)\n", g_videoExport.frameCount);
        js_finish_encoder("chess_game.webm");
    }
    #endif
}

static inline bool VideoExportIsRecording(void) {
    return g_videoExport.recording;
}

static inline int VideoExportGetFrameCount(void) {
    return g_videoExport.frameCount;
}

static inline void VideoExportCleanup(void) {
    if (g_videoExport.recording) {
        VideoExportStop();
    }
}

#ifdef __cplusplus
}
#endif

#endif