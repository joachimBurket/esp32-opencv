
#include "app_screen.h"

#include "esp32/rom/tjpgd.h"
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/sockets.h"


#include "board_def.h"


#define TAG "[Screen]"

CEspLcd *tft = NULL;

int image_debug = 0;

// === Special coordinates constants ===
typedef struct __attribute__((__packed__))
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} dispWin_t;

dispWin_t dispWin = {
    .x1 = 0,
    .y1 = 0,
    .x2 = TFT_WITDH,
    .y2 = TFT_HEIGHT,
};

typedef struct {
    FILE *fhndl; // File handler for input function
    int x;           // image top left point X position
    int y;           // image top left point Y position
    int s;
    bool found_header;
    uint8_t *membuff;       // memory buffer containing the image
    uint32_t bufsize;       // size of the memory buffer
    uint32_t bufptr;        // memory buffer current position
    color_t *linbuf[2]; // memory buffer used for display output
    uint8_t linbuf_idx;
} JPGIODEV;



// User defined call-back function to input JPEG data from file
//---------------------
static UINT tjd_s_input(
		JDEC *jd,		// Decompression object
		BYTE *buff, // Pointer to the read buffer (NULL:skip)
		UINT nd			// Number of bytes to read/skip from input stream
)
{
	int rb = 0;
	char recv_buf[16];
	// Device identifier for the session (5th argument of jd_prepare function)
	JPGIODEV *dev = (JPGIODEV *)jd->device;

	while (!dev->found_header)
	{
		rb += read(dev->s, recv_buf, 1);
		putchar(recv_buf[0]); // debug
		if (recv_buf[0] == '\r')
		{
			rb += read(dev->s, recv_buf, 3);
			putchar(recv_buf[0]); putchar(recv_buf[1]); putchar(recv_buf[2]); // debug
			if ((recv_buf[0] == '\n') && (recv_buf[1] == '\r') && (recv_buf[2] == '\n'))
			{
				ESP_LOGI("TFT", "Found header ended at %d", rb);
				dev->found_header = true;
			}
		}
	}

	if (buff)
	{ // Read nd bytes from the input strem
		rb = read(dev->s, buff, nd);
		// ESP_LOGI("TFT", "Socket read %d, actual %d", nd, rb);
		return rb; // Returns actual number of bytes read
	}
	else
	{ // Remove nd bytes from the input stream
		int seek_size = nd;
		while (seek_size)
		{
			if (seek_size > sizeof(recv_buf))
			{
				rb = read(dev->s, recv_buf, sizeof(recv_buf));
			}
			else
			{
				rb = read(dev->s, recv_buf, seek_size);
			}
			seek_size -= rb;
		}
		return nd; // Returns actual number of bytes read
							 // if (fseek(dev->fhndl, nd, SEEK_CUR) >= 0) return nd;
							 // else return 0;
	}
}

// User defined call-back function to input JPEG data from file
//---------------------
static UINT tjd_input(
		JDEC *jd,		// Decompression object
		BYTE *buff, // Pointer to the read buffer (NULL:skip)
		UINT nd			// Number of bytes to read/skip from input stream
)
{
	int rb = 0;
	// Device identifier for the session (5th argument of jd_prepare function)
	JPGIODEV *dev = (JPGIODEV *)jd->device;

	if (buff)
	{ // Read nd bytes from the input strem
		rb = fread(buff, 1, nd, dev->fhndl);
		return rb; // Returns actual number of bytes read
	}
	else
	{ // Remove nd bytes from the input stream
		if (fseek(dev->fhndl, nd, SEEK_CUR) >= 0)
			return nd;
		else
			return 0;
	}
}

// User defined call-back function to input JPEG data from memory buffer
//-------------------------
static UINT tjd_buf_input(
    JDEC *jd,       // Decompression object
    BYTE *buff, // Pointer to the read buffer (NULL:skip)
    UINT nd         // Number of bytes to read/skip from input stream
)
{
    // Device identifier for the session (5th argument of jd_prepare function)
    JPGIODEV *dev = (JPGIODEV *)jd->device;
    if (!dev->membuff)
        return 0;
    if (dev->bufptr >= (dev->bufsize + 2))
        return 0; // end of stream

    if ((dev->bufptr + nd) > (dev->bufsize + 2))
        nd = (dev->bufsize + 2) - dev->bufptr;

    if (buff) {
        // Read nd bytes from the input strem
        memcpy(buff, dev->membuff + dev->bufptr, nd);
        dev->bufptr += nd;
        return nd; // Returns number of bytes read
    } else {
        // Remove nd bytes from the input stream
        dev->bufptr += nd;
        return nd;
    }
}


// User defined call-back function to output RGB bitmap to display device
//----------------------
static UINT tjd_output(
    JDEC *jd,           // Decompression object of current session
    void *bitmap, // Bitmap data to be output
    JRECT *rect     // Rectangular region to output
)
{
    // Device identifier for the session (5th argument of jd_prepare function)
    JPGIODEV *dev = (JPGIODEV *)jd->device;

    // ** Put the rectangular into the display device **
    int x;
    int y;
    int dleft, dtop, dright, dbottom;
    BYTE *src = (BYTE *)bitmap;

    int left = rect->left + dev->x;
    int top = rect->top + dev->y;
    int right = rect->right + dev->x;
    int bottom = rect->bottom + dev->y;

    if ((left > dispWin.x2) || (top > dispWin.y2))
        return 1; // out of screen area, return
    if ((right < dispWin.x1) || (bottom < dispWin.y1))
        return 1; // out of screen area, return

    if (left < dispWin.x1)
        dleft = dispWin.x1;
    else
        dleft = left;
    if (top < dispWin.y1)
        dtop = dispWin.y1;
    else
        dtop = top;
    if (right > dispWin.x2)
        dright = dispWin.x2;
    else
        dright = right;
    if (bottom > dispWin.y2)
        dbottom = dispWin.y2;
    else
        dbottom = bottom;

    if ((dleft > dispWin.x2) || (dtop > dispWin.y2))
        return 1; // out of screen area, return
    if ((dright < dispWin.x1) || (dbottom < dispWin.y1))
        return 1; // out of screen area, return

    uint32_t len = ((dright - dleft + 1) * (dbottom - dtop + 1)); // calculate length of data

    if ((len > 0) && (len <= JPG_IMAGE_LINE_BUF_SIZE)) {
        uint8_t *dest = (uint8_t *)(dev->linbuf[dev->linbuf_idx]);

        for (y = top; y <= bottom; y++) {
            for (x = left; x <= right; x++) {
                // Clip to display area
                if ((x >= dleft) && (y >= dtop) && (x <= dright) && (y <= dbottom)) {
                    *dest++ = (*src++) & 0xFC;
                    *dest++ = (*src++) & 0xFC;
                    *dest++ = (*src++) & 0xFC;
                } else
                    src += 3; // skip
            }
        }

        //ESP_LOGI(TAG, "x1:%d x2:%d y1:%d y2:%d\n", dleft, dright, dtop, dbottom);
        tft->setAddrWindow(dleft, dtop, dright, dbottom);

        uint16_t *p = (uint16_t *)malloc(sizeof(uint16_t) * len);
        if (!p) {
            ESP_LOGE(TAG, "malloc fail");
            return 0;
        }
        for (uint32_t i = 0; i < len; i++) {
            p[i] = tft->color565(dev->linbuf[dev->linbuf_idx][i].r, dev->linbuf[dev->linbuf_idx][i].g, dev->linbuf[dev->linbuf_idx][i].b);
        }
        tft->_fastSendBuf(p, len, true);        // TODO: Swapping bytes directly in the camera register could enhance display
        free(p);

        dev->linbuf_idx = ((dev->linbuf_idx + 1) & 1);
    } else {
        ESP_LOGE(TAG, "Data size error: %d jpg: (%d,%d,%d,%d) disp: (%d,%d,%d,%d)\r\n", len, left, top, right, bottom, dleft, dtop, dright, dbottom);
        return 0; // stop decompression
    }
    return 1; // Continue to decompression
}

//==================================================================================
void TFT_jpg_image(int x, int y, uint8_t scale, int s, char *fname, uint8_t *buf, int size)
{
	JPGIODEV dev;
	struct stat sb;
	char *work = NULL;	 // Pointer to the working buffer (must be 4-byte aligned)
	UINT sz_work = 3800; // Size of the working buffer (must be power of 2)
	JDEC jd;						 // Decompression object (70 bytes)
	JRESULT rc;

	dev.linbuf[0] = NULL;
	dev.linbuf[1] = NULL;
	dev.linbuf_idx = 0;

	dev.fhndl = NULL;
	dev.s = s;
	if (s >= 0)
	{
		// image from socket
		dev.membuff = NULL;
		dev.found_header = false;
		dev.bufsize = 0;
		dev.bufptr = 0;
	}
	else if (fname == NULL)
	{
		// image from buffer
		dev.membuff = buf;
		dev.bufsize = size;
		dev.bufptr = 0;
	}
	else
	{
		// image from file
		dev.membuff = NULL;
		dev.bufsize = 0;
		dev.bufptr = 0;

		if (stat(fname, &sb) != 0)
		{
			if (image_debug)
				printf("File error: %ss\r\n", strerror(errno));
			goto exit;
		}

		dev.fhndl = fopen(fname, "r");
		if (!dev.fhndl)
		{
			if (image_debug)
				printf("Error opening file: %s\r\n", strerror(errno));
			goto exit;
		}
	}

	if (scale > 3)
		scale = 3;

	work = (char*)malloc(sz_work);
	if (work)
	{
		if (dev.s >= 0)
			rc = jd_prepare(&jd, tjd_s_input, (void *)work, sz_work, &dev);
		else if (dev.membuff)
			rc = jd_prepare(&jd, tjd_buf_input, (void *)work, sz_work, &dev);
		else
			rc = jd_prepare(&jd, tjd_input, (void *)work, sz_work, &dev);
		if (rc == JDR_OK)
		{
			if (x == CENTER)
				x = ((dispWin.x2 - dispWin.x1 + 1 - (int)(jd.width >> scale)) / 2) + dispWin.x1;
			else if (x == RIGHT)
				x = dispWin.x2 + 1 - (int)(jd.width >> scale);

			if (y == CENTER)
				y = ((dispWin.y2 - dispWin.y1 + 1 - (int)(jd.height >> scale)) / 2) + dispWin.y1;
			else if (y == BOTTOM)
				y = dispWin.y2 + 1 - (int)(jd.height >> scale);

			if (x < ((dispWin.x2 - 1) * -1))
				x = (dispWin.x2 - 1) * -1;
			if (y < ((dispWin.y2 - 1)) * -1)
				y = (dispWin.y2 - 1) * -1;
			if (x > (dispWin.x2 - 1))
				x = dispWin.x2 - 1;
			if (y > (dispWin.y2 - 1))
				y = dispWin.y2 - 1;

			dev.x = x;
			dev.y = y;

			dev.linbuf[0] = (color_t*)heap_caps_malloc(JPG_IMAGE_LINE_BUF_SIZE * 3, MALLOC_CAP_DMA);
			if (dev.linbuf[0] == NULL)
			{
				if (image_debug)
					printf("Error allocating line buffer #0\r\n");
				goto exit;
			}
			dev.linbuf[1] = (color_t*)heap_caps_malloc(JPG_IMAGE_LINE_BUF_SIZE * 3, MALLOC_CAP_DMA);
			if (dev.linbuf[1] == NULL)
			{
				if (image_debug)
					printf("Error allocating line buffer #1\r\n");
				goto exit;
			}

			// Start to decode the JPEG file
			rc = jd_decomp(&jd, tjd_output, scale);

			if (rc != JDR_OK)
			{
				if (image_debug)
					printf("jpg decompression error %d\r\n", rc);
			}
			if (image_debug)
				printf("Jpg size: %dx%d, position; %d,%d, scale: %d, bytes used: %d\r\n", jd.width, jd.height, x, y, scale, jd.sz_pool);
		}
		else
		{
			if (image_debug)
				printf("jpg prepare error %d\r\n", rc);
		}
	}
	else
	{
		if (image_debug)
			printf("work buffer allocation error\r\n");
	}

exit:
	if (work)
		free(work); // free work buffer
	if (dev.linbuf[0])
		free(dev.linbuf[0]);
	if (dev.linbuf[1])
		free(dev.linbuf[1]);
	if (dev.fhndl)
		fclose(dev.fhndl); // close input file
}


#include "lvgl.h"

/*Write the internal buffer (VDB) to the display. 'lv_flush_ready()' has to be called when finished*/
static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    tft->drawBitmap((int16_t)x1, (int16_t)y1, (const uint16_t *)color_p, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1));
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_flush_ready();
}

/*Fill an area with a color on the display*/
static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    tft->fillRect((int16_t)x1, (int16_t)y1, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1), (uint16_t)color.full);
}

/*Write pixel map (e.g. image) to the display*/
static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    tft->drawBitmap((int16_t)x1, (int16_t)y1, (const uint16_t *)color_p, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1));
}

void lvgl_lcd_hal_init()
{

	lcd_conf_t lcd_pins = {
        .lcd_model = LCD_MOD_ST7789,
        .pin_num_miso = TFT_MISO,
        .pin_num_mosi = TFT_MOSI,
        .pin_num_clk = TFT_SCLK,
        .pin_num_cs = TFT_CS,
        .pin_num_dc = TFT_DC,
        .pin_num_rst = TFT_RST,		
        .pin_num_bckl = TFT_BK,
        .clk_freq = 26 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .spi_host = HSPI_HOST,
        .init_spi_bus = true,
    };

    /*Initialize SPI Handler*/
    if (tft == NULL) {
        tft = new CEspLcd(&lcd_pins, TFT_HEIGHT, TFT_WITDH);
    }

    /*screen initialize*/
    tft->invertDisplay(true);
    tft->setRotation(2);        // rotation needed if camera is on the back of the device
    tft->fillScreen(COLOR_BLACK);

    lv_disp_drv_t disp_drv;         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);    /*Basic initialization*/

    /* Set up the functions to access to your display */
    if (LV_VDB_SIZE != 0) {
        disp_drv.disp_flush = ex_disp_flush; /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/
    } else if (LV_VDB_SIZE == 0) {
        disp_drv.disp_fill = ex_disp_fill; /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/
        disp_drv.disp_map = ex_disp_map;   /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/
    }

    /* Finally register the driver */
    lv_disp_drv_register(&disp_drv);
}
