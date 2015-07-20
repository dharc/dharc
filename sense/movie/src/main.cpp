/* libSDL and libVLC sample code
 * Copyright © 2008 Sam Hocevar <sam@zoy.org>
 * license: [http://en.wikipedia.org/wiki/WTFPL WTFPL] */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mutex.h>

#include <vlc/vlc.h>

#include "dharc/sense.hpp"
#include "dharc/node.hpp"

#include <vector>

#define WIDTH 320
#define HEIGHT 240

#define VIDEOWIDTH 320
#define VIDEOHEIGHT 240

using std::vector;
using dharc::Node;
using dharc::Sense;

struct ctx
{
    SDL_Surface *surf;
    SDL_mutex *mutex;
};

static void *lock(void *data, void **p_pixels)
{
    ctx *pctx = (ctx*)data;

    SDL_LockMutex(pctx->mutex);
    SDL_LockSurface(pctx->surf);
    *p_pixels = pctx->surf->pixels;
    return NULL; /* picture identifier, not needed here */
}

vector<uint8_t> ddata;
vector<uint8_t> ldata;
Sense *sense;
Node dblock;

#define INDEX(X,Y,WIDTH) ((Y * WIDTH) + X)

static void unlock(void *data, void *id, void *const *p_pixels)
{
    ctx *pctx = (ctx*)data;

	vector<uint8_t> rdata = sense->reform2D(RegionID::SENSE_CAMERA_0_LUMINANCE, 5, 5);

    /* VLC just rendered the video, but we can also render stuff */
    uint16_t *pixels = (uint16_t*)*p_pixels;

    for (auto i = 0U; i < ldata.size(); ++i) {
		uint16_t y = pixels[i];
		pixels[i] = 0;

		float r = (float)(y >> 11) / 31.0f * 255.0f;
		float g = (float)((y >> 5) & 0x3f) / 63.0f * 255.0f;
		float b = (float)(y & 0x1f) / 31.0f * 255.0f;

		int dy = (int)((0.257 * r) + (0.504 * g) + (0.098 * b) + 16.0);
		//ddata[i] = (std::abs(dy - ldata[i]) & 0xC0) + (dy >> 2);
		ldata[i] = dy;

		//ddata[i] = static_cast<float>(y & 0x001f) / 31.0f;
	}

	/*for (auto i = 1U; i < ldata.size()-1; ++i) {
		int delta = 0;
		delta += ldata[i-1];
		delta += ldata[i+1];
		if (i > 320) delta += ldata[i-320];
		if (i < ldata.size()+320) delta += ldata[i+320];
		delta /= 4;
		ddata[i] = (std::abs(ldata[i] - delta) & 0xF0) + (ldata[i] >> 4);
	}*/

	sense->write2D(RegionID::SENSE_CAMERA_0_LUMINANCE, ldata, 5, 5);

	assert(rdata.size() == 320 * 240);

	for (auto i = 0U; i < rdata.size(); ++i) {
		//odata[i] += rdata[i];
		//if (odata[i] < 0) odata[i] = 0;
		//if (odata[i] > 255) odata[i] = 255;
		//uint16_t colour = odata[i];
		//uint16_t colour = (ldata[i] >= 0) ? ldata[i] : 0 - ldata[i];
		//colour *= 2;

		uint16_t colour = rdata[i];
		pixels[i] = (colour >> 3) | ((colour >> 2) << 5) | ((colour >> 3) << 11);
	}

    SDL_UnlockSurface(pctx->surf);
    SDL_UnlockMutex(pctx->mutex);

    assert(id == NULL); /* picture identifier, not needed here */
}

static void display(void *data, void *id)
{
    /* VLC wants to display the video */
    (void) data;
    assert(id == NULL);
}

int main(int argc, char *argv[])
{
	sense = new Sense("192.168.1.78", 7878);

	ddata.resize(320*240);
	ldata.resize(320*240);

    libvlc_instance_t *libvlc;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
    char const *vlc_argv[] =
    {
        "--no-audio", /* skip any audio track */
        "--no-xlib", /* tell VLC to not use Xlib */
		"--network-caching=50",
		"--live-caching=50",
    };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

    SDL_Surface *screen, *empty;
    SDL_Event event;
    SDL_Rect rect;
    int done = 0, action = 0, pause = 0, n = 0;

    struct ctx ctx;

    if(argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     *  Initialise libSDL
     */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD) == -1)
    {
        printf("cannot initialize SDL\n");
        return EXIT_FAILURE;
    }

	SDL_WM_SetCaption("Dharc Movie Stream", NULL);

    empty = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT,
                                 32, 0, 0, 0, 0);
    ctx.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT,
                                    16, 0x001f, 0x07e0, 0xf800, 0);

    ctx.mutex = SDL_CreateMutex();

    int options = SDL_ANYFORMAT | SDL_HWSURFACE | SDL_DOUBLEBUF;

    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
    if(!screen)
    {
        printf("cannot set video mode\n");
        return EXIT_FAILURE;
    }

    /*
     *  Initialise libVLC
     */
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    m = libvlc_media_new_location(libvlc, argv[1]);
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_release(m);

    libvlc_video_set_callbacks(mp, lock, unlock, display, &ctx);
    libvlc_video_set_format(mp, "RV16", VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH*2);
    libvlc_media_player_play(mp);

    /*
     *  Main loop
     */
    rect.w = 0;
    rect.h = 0;

    while(!done)
    { 
        action = 0;

        /* Keys: enter (fullscreen), space (pause), escape (quit) */
        while( SDL_PollEvent( &event ) ) 
        { 
            switch(event.type)
            {
            case SDL_QUIT:
                done = 1;
                break;
            case SDL_KEYDOWN:
                action = event.key.keysym.sym;
                break;
            }
        }

        switch(action)
        {
        case SDLK_ESCAPE:
            done = 1;
            break;
        case SDLK_RETURN:
            options ^= SDL_FULLSCREEN;
            screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
            break;
        case ' ':
            pause = !pause;
            break;
        }

        rect.x = 0;
        rect.y = 0;

        if(!pause)
            n++;

        /* Blitting the surface does not prevent it from being locked and
         * written to by another thread, so we use this additional mutex. */
        SDL_LockMutex(ctx.mutex);
        SDL_BlitSurface(ctx.surf, NULL, screen, &rect);
        SDL_UnlockMutex(ctx.mutex);

        SDL_Flip(screen);
        SDL_Delay(10);

        SDL_BlitSurface(empty, NULL, screen, &rect);
    }

    /*
     * Stop stream and clean up libVLC
     */
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(libvlc);

    /*
     * Close window and clean up libSDL
     */
    SDL_DestroyMutex(ctx.mutex);
    SDL_FreeSurface(ctx.surf);
    SDL_FreeSurface(empty);

    SDL_Quit();

    return 0;
}
