#include "cameraapp.h"

int camera_dbg_en = 0;
static struct v4l2_buffer   vbuf;
static camera_handle camera;
static int devOpen = 0;
static int streamOn = 0;

int cam_init(int index,int width, int height)
{
    camera.win_width = width;
    camera.win_height = height;
    if (width == 1280)
        camera.fps = 60;
    else
        camera.fps = 90;

    camera.photo_num = 5;

    // camera.pixelformat = V4L2_PIX_FMT_NV12;
    // camera.pixelformat = V4L2_PIX_FMT_SGRBG10;
    // camera.pixelformat = V4L2_PIX_FMT_MPEG1;
    camera.pixelformat = V4L2_PIX_FMT_MJPEG;

    char camera_path[16];
    camera.camera_index = index;

    /* 1.open /dev/videoX node */
    sprintf(camera_path, "%s%d", "/dev/video", index);
    camera.videofd = open(camera_path, O_RDWR, 0);
    if (camera.videofd < 0)
    {
        printf(" open %s fail!!!\n", camera_path);
        return -1;
    }

    /* 2.Query device capabilities */
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
    if (ioctl(camera.videofd, VIDIOC_QUERYCAP, &cap) < 0) {
        printf(" Query device capabilities fail!!!\n");
    } else {
        printf(" Querey device capabilities succeed\n");
        printf(" cap.driver=%s\n", cap.driver);
        printf(" cap.card=%s\n", cap.card);
        printf(" cap.bus_info=%s\n", cap.bus_info);
        printf(" cap.version=0x%08x\n", cap.version);
        printf(" cap.capabilities=0x%08x\n", cap.capabilities);
    }

    if ((cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_CAPTURE_MPLANE)) <= 0) {
        printf(" The device is not supports the Video Capture interface!!!\n");
        close(camera.videofd);
        return -1;
    }

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        camera.driver_type = V4L2_CAP_VIDEO_CAPTURE_MPLANE;
    } else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        camera.driver_type = V4L2_CAP_VIDEO_CAPTURE;
    } else {
        printf(" %s is not a capture device.\n", camera_path);
        close(camera.videofd);
        return -1;
    }


    /* 3.select the current video input */
    struct v4l2_input inp;
    memset(&inp, 0, sizeof(inp));
    inp.index = 0;
    inp.type = V4L2_INPUT_TYPE_CAMERA;
    if (ioctl(camera.videofd, VIDIOC_S_INPUT, &inp) < 0) {
        printf(" VIDIOC_S_INPUT failed! s_input: %d\n", inp.index);
        close(camera.videofd);
        return -1;
    }

    struct v4l2_streamparm parms;
    memset(&parms, 0, sizeof(struct v4l2_streamparm));
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    else
        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parms.parm.capture.timeperframe.numerator = 1;
    parms.parm.capture.timeperframe.denominator = camera.fps;
    if (ioctl(camera.videofd, VIDIOC_S_PARM, &parms) < 0) {
        printf(" Setting streaming parameters failed, numerator:%d denominator:%d\n",
               parms.parm.capture.timeperframe.numerator,
               parms.parm.capture.timeperframe.denominator);
        close(camera.videofd);
        return -1;
    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        fmt.fmt.pix_mp.width = camera.win_width;
        fmt.fmt.pix_mp.height = camera.win_height;
        fmt.fmt.pix_mp.pixelformat = camera.pixelformat;
        fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
    } else {
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = camera.win_width;
        fmt.fmt.pix.height = camera.win_height;
        fmt.fmt.pix.pixelformat = camera.pixelformat;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
    }

    if (ioctl(camera.videofd, VIDIOC_S_FMT, &fmt) < 0) {
        printf(" setting the data format failed!\n");
        close(camera.videofd);
        return -1;
    }

    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        if (camera.win_width != fmt.fmt.pix_mp.width || camera.win_height != fmt.fmt.pix_mp.height)
            printf(" does not support %u * %u\n", camera.win_width, camera.win_height);

        camera.win_width = fmt.fmt.pix_mp.width;
        camera.win_height = fmt.fmt.pix_mp.height;


        if (ioctl(camera.videofd, VIDIOC_G_FMT, &fmt) < 0)
            printf(" get the data format failed!\n");

        camera.nplanes = fmt.fmt.pix_mp.num_planes;
    } else {
        if (camera.win_width != fmt.fmt.pix.width || camera.win_height != fmt.fmt.pix.height)
            printf(" does not support %u * %u\n", camera.win_width, camera.win_height);

        camera.win_width = fmt.fmt.pix.width;
        camera.win_height = fmt.fmt.pix.height;
        printf(" VIDIOC_S_FMT succeed\n");
        printf(" fmt.type = %d\n", fmt.type);
        printf(" fmt.fmt.pix.width = %d\n", fmt.fmt.pix.width);
        printf(" fmt.fmt.pix.height = %d\n", fmt.fmt.pix.height);

    }

    /* 10.Initiate Memory Mapping or User Pointer I/O */
    struct v4l2_requestbuffers  req;
    memset(&req, 0, sizeof(struct v4l2_requestbuffers));
    req.count = 4;
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    else
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(camera.videofd, VIDIOC_REQBUFS, &req) < 0) {
        printf(" VIDIOC_REQBUFS failed\n");
        close(camera.videofd);
        return -1;
    }

    /* Query the status of a buffers */
    camera.buf_count = req.count;
    int n_buffers = 0;
    struct v4l2_buffer buf;
    printf(" reqbuf number is %d\n", camera.buf_count);

    camera.buffers = (struct buffer *)calloc(req.count, sizeof(struct buffer));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        memset(&buf, 0, sizeof(struct v4l2_buffer));
        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        else
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
            buf.length = camera.nplanes;
            buf.m.planes = (struct v4l2_plane *)calloc(buf.length, sizeof(struct v4l2_plane));
        }

        if (ioctl(camera.videofd, VIDIOC_QUERYBUF, &buf) == -1) {
            printf(" VIDIOC_QUERYBUF error\n");

            if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
                free(buf.m.planes);
            free(camera.buffers);

            close(camera.videofd);

            return -1;
        }

        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
            for (int i = 0; i < camera.nplanes; i++) {
                camera.buffers[n_buffers].length[i] = buf.m.planes[i].length;
                camera.buffers[n_buffers].start[i] = mmap(NULL , buf.m.planes[i].length,
                                     PROT_READ | PROT_WRITE, \
                                     MAP_SHARED , camera.videofd, \
                                     buf.m.planes[i].m.mem_offset);

                printf(" map buffer index: %d, mem: %p, len: %x, offset: %x\n",
                       n_buffers, camera.buffers[n_buffers].start[i], buf.m.planes[i].length,
                       buf.m.planes[i].m.mem_offset);
            }
            free(buf.m.planes);
        } else {
            camera.buffers[n_buffers].length[0] = buf.length;
            camera.buffers[n_buffers].start[0] = mmap(NULL , buf.length,
                                 PROT_READ | PROT_WRITE, \
                                 MAP_SHARED , camera.videofd, \
                                 buf.m.offset);
            printf(" map buffer index: %d, mem: %p, len: %x, offset: %x\n", \
                   n_buffers, camera.buffers[n_buffers].start[0], buf.length, buf.m.offset);
        }
    }

    /* 11.Exchange a buffer with the driver */
    for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
        memset(&buf, 0, sizeof(struct v4l2_buffer));
        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        else
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
            buf.length = camera.nplanes;
            buf.m.planes = (struct v4l2_plane *)calloc(buf.length, sizeof(struct v4l2_plane));
        }

        if (ioctl(camera.videofd, VIDIOC_QBUF, &buf) == -1) {
            printf(" VIDIOC_QBUF error\n");

            if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
                free(buf.m.planes);
            free(camera.buffers);

            close(camera.videofd);
            return -1;
        }
        if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            free(buf.m.planes);
    }
    printf("dev open.\n");
    devOpen = 1;
    return 0;
}

int camera_streamon()
{
    enum v4l2_buf_type type;
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    }
    else
    {
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }

    if (ioctl(camera.videofd, VIDIOC_STREAMON, &type) == -1)
    {
        printf(" VIDIOC_STREAMON error! %s\n", strerror(errno));
        return -1;
    }
    else
    {
        printf(" stream on succeed\n");
        streamOn = 1;
    }

    memset(&vbuf, 0, sizeof(struct v4l2_buffer));
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    }
    else
    {
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }

    vbuf.memory = V4L2_MEMORY_MMAP;
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        vbuf.length = camera.nplanes;
        vbuf.m.planes = (struct v4l2_plane *)calloc(camera.nplanes, sizeof(struct v4l2_plane));
    }

    return 0;
}

int camera_capture(void *data, int *len)
{
    if (!devOpen)
        return -1;
    int ret = -1;
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(camera.videofd, &fds);
    ret = select(camera.videofd + 1, &fds, NULL, NULL, &tv);
    if (ret == -1)
    {
        printf(" select error\n");
        return ret;
    }
    else if (ret == 0)
    {
        printf(" camera%d select timeout,end capture thread!\n", camera.camera_index);
        ret = -1;
        return ret;
    }

    /* dqbuf */
    ret = ioctl(camera.videofd, VIDIOC_DQBUF, &vbuf);
    if (ret == 0)
    {
        //printf("*****DQBUF[%d] FINISH*****\n", vbuf.index);
    }
    else
    {
        //printf("****DQBUF FAIL*****\n");
    }

    gettimeofday(&tv, NULL);
    //if (*len >= camera.buffers[vbuf.index].length[0])
    {
        // *len = camera.buffers[vbuf.index].length[0];
    }

    *len = vbuf.bytesused;
    memcpy(data, camera.buffers[vbuf.index].start[0], *len);
    /* qbuf */
    if (ioctl(camera.videofd, VIDIOC_QBUF, &vbuf) == 0)
    {
        //printf("************QBUF[%d] FINISH**************\n", vbuf.index);
    }
    else
    {
        //printf("*****QBUF FAIL*****\n");
    }
    return ret;
}
int camera_streamoff()
{
    if (!streamOn)
        return -1;
    enum v4l2_buf_type type;
    /* streamoff */
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    else
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera.videofd, VIDIOC_STREAMOFF, &type) == -1)
    {
        printf(" VIDIOC_STREAMOFF error! %s\n", strerror(errno));
        return -1;
    }
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        free(vbuf.m.planes);
    }
    streamOn = 0;
    return 0;
}
void camera_deinit()
{
    if(!devOpen)
    return ;
/* munmap camera->buffers */
    if (camera.driver_type == V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
        for (int i = 0; i < camera.buf_count; ++i)
            for (int j = 0; j < camera.nplanes; j++)
                munmap(camera.buffers[i].start[j], camera.buffers[i].length[j]);
    } else {
        for (int i = 0; i < camera.buf_count; i++)
            munmap(camera.buffers[i].start[0], camera.buffers[i].length[0]);
    }

    /* free camera->buffers and close camera->videofd */
    printf(" close /dev/video%d\n", camera.camera_index);


    free(camera.buffers);
    close(camera.videofd);
    devOpen = 0;
}
