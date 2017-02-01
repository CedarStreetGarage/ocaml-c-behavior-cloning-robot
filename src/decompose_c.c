#include <math.h>
#include <stdlib.h>
#include <caml/bigarray.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include "jpeg_c.h"
#include "defs.h"


int w_blocks,
    h_blocks;

char *str;


CAMLprim value initialize_feature_string(value size) {

    int width,
        height,
        total;

    CAMLparam1(size);

    width = Int_val(Field(size, WIDTH_POSITION));
    height = Int_val(Field(size, HEIGHT_POSITION));

    total = 2 * (width * height) + height + 3;

    posix_memalign((void *)&str, CACHE_LINE_SIZE, total); 
    
    CAMLreturn(Val_unit); 
}


CAMLprim value decompose_video(value gray_data, value frame_size, value result_data, value feature_size, value filter_params) {

    float centroid_white_x,
          centroid_white_y,
          centroid_black_x,
          centroid_black_y,
          delta_x,
          delta_y,
          lower_limit_pct,
          upper_limit_pct,
          *result;

    int framewidth,
        frameheight,
        w_units,
        h_units,
        h_start,
        h_stop,
        w_start,
        w_stop,
        i, 
        j,
        x,
        y,
        threshold,
        lower_limit,
        upper_limit;

    long total_white_x,
         total_white_y,
         total_black_x,
         total_black_y,
         black_pixels,
         white_pixels;

    unsigned char *grayscale;

    CAMLparam5(gray_data, frame_size, result_data, feature_size, filter_params);

    framewidth  = Int_val(Field(frame_size, WIDTH_POSITION));
    frameheight = Int_val(Field(frame_size, HEIGHT_POSITION));

    w_blocks  = Int_val(Field(feature_size, WIDTH_POSITION));
    h_blocks  = Int_val(Field(feature_size, HEIGHT_POSITION));

    threshold = Int_val(Field(filter_params, THRESHOLD_POSITION));

    lower_limit_pct = Double_val(Field(filter_params, LOWER_LIMIT_POSITION));
    upper_limit_pct = Double_val(Field(filter_params, UPPER_LIMIT_POSITION));

    grayscale = Caml_ba_data_val(gray_data);
    result    = Caml_ba_data_val(result_data);

    w_units = framewidth / w_blocks;
    h_units = frameheight / h_blocks;

    lower_limit = (int)((float)w_units * (float)h_units * lower_limit_pct);
    upper_limit = (int)((float)w_units * (float)h_units * upper_limit_pct);

    /* Main loops over result blocks */
    for (x = 0; x < h_blocks; x++) {
        for (y = 0; y < w_blocks; y++) {

            /* Inner loops over grayscale pixels */
            h_start = x * h_units;
            h_stop  = (x + 1) * h_units;
            w_start = y * w_units;
            w_stop  = (y + 1) * w_units;

            total_white_x = 0;
            total_white_y = 0;
            total_black_x = 0;
            total_black_y = 0;
            black_pixels  = 0;
            white_pixels  = 0;

            /* Compute totals in the feature zone */
            for (i = h_start; i < h_stop; i++) {
                for (j = w_start; j < w_stop; j++) {
           
                    if (grayscale[i * framewidth + j] > threshold) {
                        total_white_x += i;
                        total_white_y += j;
                        white_pixels++;
                    }
                    else {
                        total_black_x += i;
                        total_black_y += j;
                        black_pixels++;
                    }

                }
            }

            /* Test image output */
            /*
            result[x * h_blocks + y] = (float)white_pixels / ((float)white_pixels + (float)black_pixels);            
            save_jpeg_c_float("/root/ROBOT/small.jpg", result, w_blocks, h_blocks);
            */
    
            /* Save result vector if adequate blend of black and white pixels */
            if (white_pixels > lower_limit || white_pixels < upper_limit) { 
                centroid_white_x = (float)total_white_x / (float)white_pixels;
                centroid_white_y = (float)total_white_y / (float)white_pixels; 
                centroid_black_x = (float)total_black_x / (float)black_pixels;
                centroid_black_y = (float)total_black_y / (float)black_pixels;

                delta_x = centroid_white_x - centroid_black_x;
                delta_y = centroid_white_y - centroid_black_y;
                
                if (delta_y == 0.0) { 
                    result[x * h_blocks + y] = 0.0; 
                }
                else {
                    result[x * h_blocks + y] = atan(delta_x / delta_y);
                }
            }
            else {
                result[x * h_blocks + y] = 0.0; 
            } 

        }
    }

    CAMLreturn(Val_int(Success));
}


CAMLprim value string_of_feature(value data) {

    float element,
          *feature;

    int i,
        j,
        z;

    char printchar;

    CAMLparam1(data);
    CAMLlocal1(result);

    z = 0;
    feature = Caml_ba_data_val(data);

    str[z] = '\n';
    for (i = 0; i < h_blocks; i++) {
        for (j = 0; j < w_blocks; j++) {
            element = feature[i * w_blocks + j];
            if (element < -1.0 * M_PI / 2) {
                printchar = ' ';
            }
            else if (element < -1.0 * M_PI / 4) {
                printchar = '-';
            }
            else if (element < 0) {
                printchar = '\\';
            }
            else if (element < M_PI / 4) {
                printchar = '/';
            }
            else if (element < M_PI / 2) {
                printchar = '-';
            }
            else {
                printchar = ' ';
            }
            str[++z] = printchar;
            str[++z] = ' ';
        }
        str[++z] = '\n';
    } 
    str[++z] = '\n';
    str[++z] = '\0'; 

    result = caml_copy_string(str);

    CAMLreturn(result); 
}
