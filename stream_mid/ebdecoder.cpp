#include "ebdecoder.h"

extern "C"{

void EbDecoder::DecodeVideo(const std::string &url, const std::vector<std::string> &decoder_names_pri, const std::vector<EbOptionDict> &open_options, std::function<void(int, int, int,  unsigned char * const&)> callback, int rate, int av_task_theads)
{
    std::cout << "ffmpeg start..." <<std::endl;
    avformat_network_init();
    AVFormatContext* fmt_ctx = avformat_alloc_context();
    if(fmt_ctx==nullptr){
        std::cerr << "ERROR: allocate format context" << std::endl;
        return;
    }

    AVDictionary *avdict = nullptr;
    av_dict_set(&avdict,"rtsp_transport","tcp", 0);
    for(auto ite=open_options.begin(); ite !=open_options.end(); ite++) av_dict_set(&avdict, ite->key.c_str(), ite->value.c_str(), ite->flag);

    std::cout << "open url:"<<url.c_str()<<std::endl;
    if(avformat_open_input(&fmt_ctx, url.c_str(), nullptr, &avdict) != 0){
        std::cerr << "can not open url:" << url << std::endl;
        return;
    }

    std::cout << "find stream info" <<std::endl;
    if(avformat_find_stream_info(fmt_ctx, nullptr) < 0){
        avformat_close_input(&fmt_ctx);
        std::cerr << "can not find stream information" << std::endl;
        return;
    }

    int videoindex = -1;
    for(unsigned int i=0; i<fmt_ctx->nb_streams; i++){
        if(fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = static_cast<int>(i);
            break;
        }
    }
    if(videoindex==-1) {
        avformat_close_input(&fmt_ctx);
        std::cerr << "can not find video stream" <<std::endl;
        return;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[videoindex]->codecpar);
    if(av_task_theads > 1)codec_ctx->thread_count = av_task_theads;

    AVCodec *codec = nullptr;
    for(size_t i=0; i<decoder_names_pri.size(); i++){
        codec = avcodec_find_decoder_by_name(decoder_names_pri[i].c_str());
        if(codec && codec->id==fmt_ctx->streams[videoindex]->codecpar->codec_id){
            break;
        }else{
            codec = nullptr;
        }
    }
    if(codec==nullptr){
        std::cout << "no fit assigner decoders, to select decoders by avcodec_find_decoder" << std::endl;
        codec = avcodec_find_decoder(fmt_ctx->streams[videoindex]->codecpar->codec_id);
    }
    if(codec==nullptr){
        std::cerr << "no decoder found " << std::endl;
        avformat_close_input(&fmt_ctx);
        return;
    }
    std::cout << codec->name << " is selected" << std::endl;

    int ret = avcodec_open2(codec_ctx, codec, nullptr);
    if(ret != 0){
        std::cerr << "avcodec_open2() failed. can not open audio decoder" << std::endl;
        avformat_close_input(&fmt_ctx);
        return;
    }

    int dst_width = codec_ctx->width/rate, dst_height = codec_ctx->height/rate;
    SwsContext *sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height,
                                         codec_ctx->pix_fmt,
                                         dst_width,
                                         dst_height,
                                         AV_PIX_FMT_RGB24,
                                         SWS_BICUBIC, nullptr, nullptr, nullptr);


    AVFrame *frame = av_frame_alloc();
    if(!frame){
        avformat_close_input(&fmt_ctx);
        avcodec_close(codec_ctx);
        std::cout << "failed to alloc frame" <<std::endl;
        return;
    }

    AVFrame *frame_rgb24 = av_frame_alloc();
    if(!frame_rgb24){
        avformat_close_input(&fmt_ctx);
        avcodec_close(codec_ctx);
        av_frame_free(&frame);
        std::cout << "failed to alloc framergb24" <<std::endl;
        return;
    }

    uint8_t *out_buff = static_cast<uint8_t*>(av_malloc(sizeof(uint8_t)
                     * static_cast<size_t>(
                    av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                             dst_width,
                                             dst_height,
                                             1)
                                           )));

    av_image_fill_arrays(frame_rgb24->data,
                         frame_rgb24->linesize,
                         out_buff,
                         AV_PIX_FMT_RGB24,
                         dst_width,
                         dst_height,
                         1);

    AVPacket *packet = av_packet_alloc();
    if(!packet){
        avformat_close_input(&fmt_ctx);
        avcodec_close(codec_ctx);
        av_frame_free(&frame);
        av_frame_free(&frame_rgb24);
        std::cout << "failed to alloc avpacket" <<std::endl;
        return;
    }
    av_new_packet(packet, codec_ctx->width*codec_ctx->height);

    std::cout << "decoding..." <<std::endl;
    int got_pic, status = 0;
    while (true) {
        if (av_read_frame(fmt_ctx, packet) < 0) break;
        if( packet->stream_index == videoindex){
            ret = avcodec_send_packet(codec_ctx, packet);
            if(ret < 0) { status = -4; break;}
            while( (got_pic = avcodec_receive_frame(codec_ctx, frame)) >=0 ){
                if(got_pic==0){
                    sws_scale(sws_ctx,  static_cast<uint8_t const * const *>(frame->data),
                              frame->linesize, 0, codec_ctx->height, frame_rgb24->data,
                              frame_rgb24->linesize);
                    if(callback){
                        callback(dst_width, dst_height, 3, out_buff);
                    }
                }
            }
            //if(got_pic == AVERROR_EOF) break;
        }
    }
    if(callback) callback(0, 0, 0, out_buff);
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_free(out_buff);
    av_frame_free(&frame);
    av_frame_free(&frame_rgb24);
    avcodec_close(codec_ctx);
    avformat_close_input(&fmt_ctx);
    sws_freeContext(sws_ctx);
}

}

