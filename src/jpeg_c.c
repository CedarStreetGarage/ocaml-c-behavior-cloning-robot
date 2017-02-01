#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <caml/bigarray.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include "defs.h"


/* Saves a normalized floating point image 
 * For testing preprocessing
 */
void save_jpeg_c_float(char *filename, float *image, int width, int height) {

    int i,
        idx;

    unsigned char *grayscale = NULL;

    struct jpeg_compress_struct comp;
    struct jpeg_error_mgr       error;

    JSAMPROW rp[1];

    FILE *fp = fopen(filename, "wb");

    if(!fp) {
        return;
    }

    posix_memalign((void *)&grayscale, CACHE_LINE_SIZE, width);

    comp.err = jpeg_std_error(&error);
    jpeg_create_compress(&comp);
    jpeg_stdio_dest(&comp, fp);

    comp.input_components = 1;
    comp.num_components   = 1;
    comp.data_precision   = 1;
    comp.image_width      = width;
    comp.image_height     = height;
    comp.in_color_space   = JCS_GRAYSCALE;
    comp.dct_method       = JDCT_FLOAT;

    jpeg_set_defaults(&comp);
    jpeg_set_quality(&comp, JPEG_QUALITY, TRUE);
    jpeg_start_compress(&comp, TRUE);

    while(comp.next_scanline < comp.image_height) {
        for (i = 0; i < comp.image_width; i++) {
            idx = comp.next_scanline * comp.image_width + i;
            grayscale[i] = (unsigned char)(255.0 * image[idx]);
        }
        rp[0] = grayscale;
        jpeg_write_scanlines(&comp, rp, 1);
    }

    jpeg_finish_compress(&comp);
    jpeg_destroy_compress(&comp);

    free(grayscale);
    fclose(fp);
}


/* Saves an int-formatted grayscale image
 * For saving training frames 
 */
CAMLprim value save_jpeg_ocaml_int(value filename, value data, value frame_size) {

    int width,
        height;

    char *file;

    unsigned char *image;

    struct jpeg_compress_struct comp;
    struct jpeg_error_mgr       error;

    JSAMPROW rp[1];

    CAMLparam3(filename, data, frame_size);

    width  = Int_val(Field(frame_size, WIDTH_POSITION));
    height = Int_val(Field(frame_size, HEIGHT_POSITION));
    file   = String_val(filename);
    image  = Caml_ba_data_val(data);

    FILE *fp = fopen(file, "wb");

    if (!fp) {
        CAMLreturn(Val_unit);
    }

    comp.err = jpeg_std_error(&error);
    jpeg_create_compress(&comp);
    jpeg_stdio_dest(&comp, fp);

    comp.input_components = 1;
    comp.num_components   = 1;
    comp.data_precision   = 1;
    comp.image_width      = width;
    comp.image_height     = height;
    comp.in_color_space   = JCS_GRAYSCALE;
    comp.dct_method       = JDCT_FLOAT;

    jpeg_set_defaults(&comp);
    jpeg_set_quality(&comp, JPEG_QUALITY, TRUE);
    jpeg_start_compress(&comp, TRUE);

    while(comp.next_scanline < comp.image_height) {
        rp[0] = &image[comp.next_scanline * comp.image_width];
        jpeg_write_scanlines(&comp, rp, 1);
    }

    jpeg_finish_compress(&comp);
    jpeg_destroy_compress(&comp);

    fclose(fp);

    CAMLreturn(Val_unit); 
}
