/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/sense.hpp"

#include <iostream>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <SDL/SDL.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

using dharc::Sense;
using dharc::Node;
using std::cout;

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace {
constexpr size_t kWidth = 320;
constexpr size_t kHeight = 240;
constexpr size_t size() { return kWidth * kHeight; }

struct buffer {
	void *start;
	size_t length;
};

buffer *buffers;
const char *dev_name = "/dev/video0";
int fd = -1;



void errno_exit(const char *s) {
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}



int xioctl(int fd, int request, void *arg) {
	int r;

	do {
		r = ioctl(fd, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}



bool read_frame() {
	v4l2_buffer buf;
	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN : return false;
		default     : errno_exit("VIDIOC_DQBUF");
        }
    }

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		errno_exit("VIDIOC_QBUF");
	}

	return true;
}



void init_userp(unsigned int buffer_size) {
	v4l2_requestbuffers req;
	unsigned int page_size;

	page_size = getpagesize();
	buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

	CLEAR(req);

	req.count = 1;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	buffers = (buffer*)calloc(1, sizeof(*buffers));
	buffers[0].length = buffer_size;
	buffers[0].start = memalign(page_size, buffer_size);
}



void init_device() {
	v4l2_capability cap;
	v4l2_cropcap cropcap;
	v4l2_crop crop;
	v4l2_format fmt;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is not a V4L2 device\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is not a video capture device\n", dev_name);
		exit(EXIT_FAILURE);
	}


	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
 		fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
		exit(EXIT_FAILURE);
	}


	/* Select video input, video standard and tune here. */

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;   /* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL : break;
			default     : break;
			}
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = kWidth;
	fmt.fmt.pix.height = kHeight;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
		errno_exit("VIDIOC_S_FMT");
	}

	/* Note VIDIOC_S_FMT may change width and height. */

	init_userp(fmt.fmt.pix.sizeimage);
}



void stop_capturing() {
	v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) {
		errno_exit("VIDIOC_STREAMOFF");
	}
}



void start_capturing(void)
{
    enum v4l2_buf_type type;

    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    buf.index = 0;
    buf.m.userptr = (unsigned long)buffers[0].start;
    buf.length = buffers[0].length;

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
}


void uninit_device(void)
{
    free(buffers[0].start);
    free(buffers);
}


void close_device()
{
    if (-1 == close(fd))
        errno_exit("close");

    fd = -1;
}


void open_device()
{
    struct stat st;

    if (-1 == stat(dev_name, &st))
    {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode))
    {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    fd = open(dev_name, O_RDWR /* required */  | O_NONBLOCK, 0);

    if (-1 == fd)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}


int sdl_filter(const SDL_Event * event)
{
    return event->type == SDL_QUIT;
}

};  // namespace


int main(int argc, char *argv[]) {
	Sense sense("localhost", 7878);

	Node cam;
	sense.makeInputBlock(320, 240, cam);

	atexit(SDL_Quit);
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_WM_SetCaption("Dharc Sense Camera", NULL);

	auto *buffer_sdl = new uint8_t[kWidth * kHeight * 3];

    SDL_SetVideoMode(kWidth, kHeight, 24, SDL_HWSURFACE);

    auto *data_sf = SDL_CreateRGBSurfaceFrom(buffer_sdl, kWidth, kHeight,
                                       24, kWidth * 3, 0, 0, 0, 0);

    SDL_SetEventFilter(sdl_filter);

	open_device();
	init_device();
	start_capturing();

	vector<float> data;
	data.resize(320 * 240);

	SDL_Event event;
	bool running = true;

	while (running) {
		while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;

		fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r)
        {
            if (EINTR == errno)
                continue;

            errno_exit("select");
        }

        if (0 == r)
        {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }


		read_frame();

		constexpr auto BWIDTH = 5U;

		vector<Node> strong = sense.readStrong(cam, 10.0);

		for (auto i = 0U; i < data.size(); ++i) {
			unsigned char y = *((char*)buffers[0].start + (2*i));

			data[i] = static_cast<float>(y) / 255.0f;
			buffer_sdl[i*3] = 0;
			buffer_sdl[(i*3)+1] = 0;
			buffer_sdl[(i*3)+2] = 0;
		}
		sense.writeInput(cam, data);

		for (auto i : strong) {
			if (i.harc() >= BWIDTH*BWIDTH) continue;

			size_t bx = i.macroX() * BWIDTH;
			size_t by = i.macroY() * BWIDTH;

			bx += i.harc() % BWIDTH;
			by += i.harc() / BWIDTH;

			if (bx >= kWidth || by >= kHeight) {
				std::cout << "FUCKUP\n";
			}

			size_t ix = (by * kWidth) + bx;
			if (buffer_sdl[(ix*3)+2] <= 200) buffer_sdl[(ix*3)+2] += 50;
		}

		SDL_Surface *screen = SDL_GetVideoSurface();
		if (SDL_BlitSurface(data_sf, NULL, screen, NULL) == 0)
		    SDL_UpdateRect(screen, 0, 0, 0, 0);
	}

	stop_capturing();
	uninit_device();
	close_device();

	return 0;
}

