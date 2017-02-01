#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <caml/bigarray.h>
#include <caml/signals.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include "defs.h"


int fd, 
    framewidth, 
    frameheight;

unsigned char *framebuffer;


CAMLprim value initialize_video(value frame_size, value device) {

    struct v4l2_format         format;
    struct v4l2_requestbuffers requestbuffers;
    struct v4l2_buffer         buffer;

    CAMLparam2(frame_size, device);

    framewidth  = Int_val(Field(frame_size, WIDTH_POSITION));
    frameheight = Int_val(Field(frame_size, HEIGHT_POSITION));

    /* Open the video device */
    fd = open(String_val(device), O_RDWR | O_NONBLOCK);
    if (OPEN_ERROR == fd) {
        CAMLreturn(Val_int(Open_Error));
    }
  
    /* Set the capture format */
    memset(&format, 0, sizeof(format));
    format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    format.fmt.pix.width       = framewidth;
    format.fmt.pix.height      = frameheight;
    if(IOCTL_ERROR == ioctl(fd, VIDIOC_S_FMT, &format)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }
 
    /* Set up number and type of buffers */
    memset(&requestbuffers, 0, sizeof(requestbuffers));
    requestbuffers.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    requestbuffers.count  = 1;
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_REQBUFS, &requestbuffers)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }
 
    /* Set up buffer info for mmap'ing */
    memset(&buffer, 0, sizeof(buffer));
    buffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index  = 0;
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_QUERYBUF, &buffer)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }

    /* mmap the specific frame buffer */
    framebuffer = mmap(NULL, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffer.m.offset);
    if (MAP_FAILED == framebuffer) {
        CAMLreturn(Val_int(MMAP_Error));
    }

    CAMLreturn(Val_int(Success));
}


CAMLprim value capture_video(value data) {

    int i, 
        j, 
        idx; 

    unsigned char *grayscale;

    struct v4l2_buffer buffer;
    struct timeval     tv;
    fd_set             fds;

    CAMLparam1(data);

    grayscale = Caml_ba_data_val(data);

    caml_enter_blocking_section();
   
    /* Set up specific buffer */
    memset(&buffer, 0, sizeof(buffer));
    buffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index  = 0;

    /* Turn video stream on */
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_STREAMON, &buffer.type)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }

    /* Queue buffer */
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_QBUF, &buffer)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }

    /* Wait on the mapped video */
    memset(&tv, 0, sizeof(tv));
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    tv.tv_sec = 2;
    if (SELECT_ERROR == select(fd + 1, &fds, NULL, NULL, &tv)) {
        CAMLreturn(Val_int(Select_Error));
    }

    /* De-queue buffer */
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_DQBUF, &buffer)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }

    /* Turn video stream off */
    if (IOCTL_ERROR == ioctl(fd, VIDIOC_STREAMOFF, &buffer.type)) {
        CAMLreturn(Val_int(IOCTL_Error));
    }

    /* Convert the YUYV to scaled Y-channel */
    for (i = 0; i < frameheight; i++) {
        for (j = 0; j < framewidth; j++) {
            idx = i * framewidth + j;
            grayscale[idx] = framebuffer[2 * idx];
        }
    }
   
    caml_leave_blocking_section();

    CAMLreturn(Val_int(Success));
}


/* Close video descriptor */
CAMLprim value close_video(value unit) {

    CAMLparam1(unit);

    if (CLOSE_ERROR == close(fd)) {
        CAMLreturn(Val_int(Close_Error));
    }

    CAMLreturn(Val_int(Success));
}
