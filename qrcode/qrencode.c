#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "qrencode.h"

static const char* black = "  ";
static const char* white = "\033[47m  \033[0m";
//static const char* white = "##";

static void writeQRCodeTopLine(size_t size) {
	write(1, "\n", 1);
    write(1, black, strlen(black));
	for (size_t i = 0; i < size + 2; i++) {
		write(1, white, strlen(white));
	}
	write(1, "\n", 1);
}

static void writeQRCodeBottomLine(size_t size) {
    write(1, black, strlen(black));
	for (size_t i = 0; i < size + 2; i++) {
		write(1, white, strlen(white));
	}
	write(1, "\n", 1);
	write(1, "\n", 1);
}

static int writeQRCode(QRcode *qrcode)
{
	unsigned int size = qrcode->width;

	writeQRCodeTopLine(size);
	for(size_t y = 0; y < size; y++) {
		unsigned char* row = qrcode->data + (y * size);

		write(1, black, strlen(black));
		write(1, white, strlen(white));
		for(size_t x = 0; x < size; x++) {
			if (row[x] & 0x1) {
				write(1, black, strlen(black));
			} else {
				write(1, white, strlen(white));
			}
		}
		write(1, white, strlen(white));
		write(1, "\n", 1);
	}
  	writeQRCodeBottomLine(size);
	return 0;
}

void qrencode(char *intext)
{
	QRcode *qrcode;
	
	qrcode = QRcode_encodeString(intext, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	writeQRCode(qrcode);
	QRcode_free(qrcode);
}
