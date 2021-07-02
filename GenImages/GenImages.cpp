#include <stdio.h>

typedef unsigned short ushort;
typedef unsigned char byte;
typedef FILE *file_p;

int main() {
	file_p fr = fopen("images.txt", "r"), fw = fopen("images.dat", "wb");
	ushort n;
	fscanf(fr, "%hu", &n);
	for (ushort i = 0; i < n; i++) {
		ushort w, h;
		fscanf(fr, "%hu %hu", &w, &h);
		while (fgetc(fr) != '\n');
		fputwc(w, fw);
		fputwc(h, fw);
		for (ushort j = 0; j < h; j++) {
			byte dat = 0;
			ushort pos = 0, cnt = 0;
			while (pos < w) {
				char ch = fgetc(fr);
				if (ch == '.' || ch == ' ') {
					dat |= 1 << 8 - 1 - cnt;
					pos++;
					cnt++;
				} else if (ch == 'X' || ch == 'x') {
					pos++;
					cnt++;
				} else if (ch == '\n') {
					if (cnt > 0) {
						pos += 8 - cnt;
						cnt = 8;
					}
				} else {
					return 1;
				}
				if (cnt >= 8) {
					fputc(dat, fw);
					dat = 0;
					cnt = 0;
				}
			}
			if (cnt > 0) {
				fputc(dat, fw);
				if ((pos + 8 - cnt) % 16 > 0)
					fputc(0, fw);
			}
		}
	}
	return 0;
}
