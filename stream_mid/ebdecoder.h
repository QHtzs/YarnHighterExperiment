#ifndef EBDECODER_H
#define EBDECODER_H

/*
@author:ExcellentBear
@brief:Simple Wraper of ffmpeg
@license:  GNU GENERAL PUBLIC LICENSE VERSION3
Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.
*/


#include "eboptiondict.h"
#include <functional>
#include <vector>
#include <string>
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




class EbDecoder
{
public:
    static void DecodeVideo(const std::string &url,  const std::vector<std::string> &decoder_names_pri, const std::vector<EbOptionDict> &open_options, std::function<void(int, int, int, unsigned char * const &)> callback=nullptr, int rate=2, int av_task_theads=1);

};

#ifdef __cplusplus
}
#endif


#endif // EBDECODER_H


