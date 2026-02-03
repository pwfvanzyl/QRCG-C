/* Importations: */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "qrencode.h"
#include "C-Utils/cutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* QRCG-C version variables: */
#define QRCG_C_FULL_VERSION  20260203 /* QRCG-C full version variable (2026/02/03). */
#define QRCG_C_MAJOR_VERSION 2026     /* QRCG-C major version variable (2026).      */
#define QRCG_C_MINOR_VERSION 2        /* QRCG-C minor version variable (02).        */
#define QRCG_C_PATCH_VERSION 3        /* QRCG-C patch version variable (03).        */

/* Global variables: */
#define STANDARD_SCREEN_VALUE 200 /* Default image size. */
#define QUIET_ZONE_MODULES      4 /* QR spec quiet zone. */

/* Functions prototypes: */
static void render_qrcode(unsigned char *img, int img_size, QRcode *qr);

/* Main code: */
int main(const int argc, const char *const argv[])
{
	/* Local variables: */
	int img_size;
	unsigned char *img;
	QRcode *qr;

	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <data> [size]\n", argv[0]);

		return 1;
	}

	else if(argc < 3)
	{
		img_size = STANDARD_SCREEN_VALUE;
	}

	else if(argc == 3)
	{
		img_size = atoi(argv[2]);

		if(img_size <= 0)
		{
			img_size = STANDARD_SCREEN_VALUE;
		}
	}

	else
	{
		fprintf(stderr, "Usage: %s <data> [size]\n", argv[0]);

		return 1;
	}

	img = (unsigned char *)malloc((size_t)(img_size * img_size));

	if(!img)
	{
		perror("malloc");

		return 1;
	}

	/********************************************/
	/* QRcode_encodeString arguments syntax:    */
	/* String;                                  */
	/* Symbol version (0 for minimum version);  */
	/* Error correction level;                  */
	/* Input format (like UTF-8);               */
	/* Case sensitive.                          */
	/* Returns 1 for success and 0 for failure. */
	/********************************************/

	qr = QRcode_encodeString(argv[1], 0, QR_ECLEVEL_H, QR_MODE_8, 1);

	if(!qr)
	{
		perror("QRcode_encodeString");

		free(img);

		return 1;
	}

	render_qrcode(img, img_size, qr);

	if(!stbi_write_png("qr_code.png", img_size, img_size, 1, img, img_size))
	{
		fprintf(stderr, "Failed to write PNG\n");

		return 1;
	}

	QRcode_free(qr);
	free(img);

	return 0;
}

static void render_qrcode(unsigned char *img, int img_size, QRcode *qr)
{
	int qr_size = qr->width;
	int scale = img_size / (qr_size + (QUIET_ZONE_MODULES * 2));
	int margin;
	int x, y, dx, dy;

	if(scale <= 0)
	{
		scale = 1;
	}

	margin = scale * QUIET_ZONE_MODULES;

	/* Clear image (white background) */
	memset(img, 255, (size_t)(img_size * img_size));

	/* Draw QR modules */
	for(y = 0; y < qr_size; y++)
	{
		for(x = 0; x < qr_size; x++)
		{
			unsigned char color = (qr->data[y * qr_size + x] & 1) ? 0 : 255;

			for(dy = 0; dy < scale; dy++)
			{
				for(dx = 0; dx < scale; dx++)
				{
					int px = margin + (x * scale) + dx;
					int py = margin + (y * scale) + dy;

					if(px < img_size && py < img_size)
					{
						img[py * img_size + px] = color;
					}
				}
			}
		}
	}
}
