#ifndef FFMPEGAVDECODER_H
#define FFMPEGAVDECODER_H


#include <functional>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
#include <libavutil/file.h>
#include <libavutil/imgutils.h>


class FfmpegAvDecoder
{
public:
    FfmpegAvDecoder();
    static void DecoderToRgb(const char* url, int group,  const char* decoder_name, std::function<void(int, int, int, int, unsigned char * const &)> m_callback, int rate=2);
};

#ifdef __cplusplus
}
#endif

#endif // FFMPEGAVDECODER_H
