/**
date 1025: there is a problem: flush when using multithread

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "SDL2/SDL.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
};
#endif
#endif

//Output YUV420P data as a file 
#define OUTPUT_YUV420P 0

pthread_mutex_t mutex;
int thread_exit = 1; // flag of thread exit

struct mypara
{
	SDL_Window    *screen;
	SDL_Renderer    *sdlRenderer;
	int    id;
};

static int lockmgr(void **mtx, enum AVLockOp op)
{
	pthread_mutex_t** pmutex = (pthread_mutex_t**)mtx;
	switch (op) {
	case AV_LOCK_CREATE:
		*pmutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(*pmutex, NULL);
		break;
	case AV_LOCK_OBTAIN:
		pthread_mutex_lock(*pmutex);
		break;
	case AV_LOCK_RELEASE:
		pthread_mutex_unlock(*pmutex);
		break;
	case AV_LOCK_DESTROY:
		pthread_mutex_destroy(*pmutex);
		free(*pmutex);
		break;
	}
	return 0;
}

void * thread_routine(void *arg)
{
	struct mypara *recv_para = (struct mypara *)arg;;  //recv para data
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame, *pFrameYUV;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	//char filepath[]="bigbuckbunny_480x272.h265";
	char filepath[] = "rtsp://192.168.131.4/0";
	//SDL---------------------------
	int screen_w = 0, screen_h = 0;
	SDL_Window *screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect, sdlRect_tmp;

	FILE *fp_yuv;

	//av_register_all();
	//avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0){
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoindex = i;
			break;
		}
	if (videoindex == -1){
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL){
		printf("Codec not found.\n");
		return -1;
	}
	//pthread_mutex_lock(&mutex);
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Could not open codec.\n");
		return -1;
	}
	//pthread_mutex_unlock(&mutex);

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

#if OUTPUT_YUV420P 
	fp_yuv = fopen("output.yuv", "wb+");
#endif  

	//if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
	//	printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
	//	return -1;
	//} 

	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	//SDL 2.0 Support for multiple windows
	//screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	screen_w*2, screen_h,
	//	SDL_WINDOW_OPENGL);

	screen = (*recv_para).screen; //get the screen
	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	//sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  
	sdlRenderer = (*recv_para).sdlRenderer;//get the sdlRenderer
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pthread_mutex_lock(&mutex);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	pthread_mutex_unlock(&mutex);


	//temp sdlRect for render copy
	sdlRect_tmp.x = 0;
	sdlRect_tmp.y = 0;
	sdlRect_tmp.w = screen_w;
	sdlRect_tmp.h = screen_h;

	//four rect in one line
	// total 4*4 = 16 rect
	sdlRect.x = 0 + screen_w / 2 * ((*recv_para).id % 4);
	sdlRect.y = 0 + screen_h / 2 * ((*recv_para).id / 4);
	sdlRect.w = screen_w / 2;
	sdlRect.h = screen_h / 2;


	//SDL End----------------------
	while (thread_exit && av_read_frame(pFormatCtx, packet) >= 0){
		if (packet->stream_index == videoindex){
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0){
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture){
				//printf("id:%d\n",(*recv_para).id); //打印线程id
				//printf("x_pos:%d   y_pos:%d\n",sdlRect.x,sdlRect.y); //print rect position
				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

#if OUTPUT_YUV420P
				y_size = pCodecCtx->width*pCodecCtx->height;
				fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
				fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
				fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
				//SDL---------------------------
#if 0
				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
#else
				pthread_mutex_lock(&mutex);  //mutex or SEGFAULT
				SDL_UpdateYUVTexture(sdlTexture, &sdlRect_tmp,//sdl tmp
					pFrameYUV->data[0], pFrameYUV->linesize[0],
					pFrameYUV->data[1], pFrameYUV->linesize[1],
					pFrameYUV->data[2], pFrameYUV->linesize[2]);
#endif	
				//SDL_RenderClear( sdlRenderer );  
				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
				//SDL_RenderCopy( sdlRenderer, sdlTexture,  NULL, &sdlRect1);  
				SDL_RenderPresent(sdlRenderer);
				pthread_mutex_unlock(&mutex);
				//SDL End-----------------------
				//Delay 40ms
				//SDL_Delay(40);
			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);
#if OUTPUT_YUV420P
		int y_size = pCodecCtx->width*pCodecCtx->height;
		fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
		fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
		fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
		//SDL---------------------------

		SDL_UpdateTexture(sdlTexture, &sdlRect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
		//SDL End-----------------------
		//Delay 40ms
		//SDL_Delay(40);
	}

	sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P 
	fclose(fp_yuv);
#endif 

	SDL_RenderClear(sdlRenderer);
	SDL_Quit();

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}
int main(int argc, char* argv[]){
	SDL_Window *screen;

	if (av_lockmgr_register(lockmgr)) {
		av_log(NULL, AV_LOG_FATAL, "Could not initialize lock manager!\n");
		return -1;
	}
	pthread_mutex_init(&mutex, NULL);
	av_register_all();
	avformat_network_init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		//      if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {    
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	//screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 704*2,576*2,SDL_WINDOW_OPENGL);  
	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", 0, 0, 704 * 2, 576 * 2, SDL_WINDOW_OPENGL);

	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

	//16 threads in a window
	struct mypara paras[16];
	for (int i = 0; i < 16; ++i)
	{
		paras[i].sdlRenderer = sdlRenderer;
		paras[i].screen = screen;
		paras[i].id = i;
	}

	pthread_t thread_id[16];
	for (int i = 0; i < 16; ++i)
	{
		pthread_create(&thread_id[i], NULL, thread_routine, &paras[i]);
	}

	//add sdl_event loop, or sdl no response
	SDL_Event e;
	for (;;) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			thread_exit = 0;
			SDL_Log("Program quit after %i ticks", e.quit.timestamp);
			break;
		}
	}
	//don't  need to pthread_join ,because of the sdl_event's for loop
	//pthread_join(thread_id1,NULL);
	//pthread_join(thread_id2,NULL);
	//pthread_join(thread_id3,NULL);
	//pthread_join(thread_id4,NULL);
	av_lockmgr_register(NULL);
	return 0;
}