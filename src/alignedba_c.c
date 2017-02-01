#include <stdlib.h>
#include <caml/bigarray.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include "defs.h"

/* Creates aligned float Bigarray, no initialization */
CAMLprim value create_aligned_ba_float (value size) {

    int l;

    long dims[1]; 

    float *out = NULL;

    CAMLparam1(size);
    CAMLlocal1(result);

    l = Int_val(size);
    dims[0] = (long)l;

    posix_memalign((void *)&out, CACHE_LINE_SIZE, l * sizeof(float));

    result = caml_ba_alloc(CAML_BA_FLOAT32 | CAML_BA_C_LAYOUT, 1, out, dims);

    CAMLreturn(result); 
}

/* Creates aligned uint8 Bigarray, no initialization */
CAMLprim value create_aligned_ba_int (value size) {

    int l;

    long dims[1]; 

    unsigned char *out = NULL;

    CAMLparam1(size);
    CAMLlocal1(result);

    l = Int_val(size);
    dims[0] = (long)l;

    posix_memalign((void *)&out, CACHE_LINE_SIZE, l * sizeof(unsigned char));

    result = caml_ba_alloc(CAML_BA_UINT8 | CAML_BA_C_LAYOUT, 1, out, dims);

    CAMLreturn(result); 
}

/* Creates aligned float Bigarray, array initialized */
CAMLprim value bigarray_of_array (value input) {

    int i,
        l;

    long dims[1];

    float *out = NULL;
    
    CAMLparam1(input);
    CAMLlocal1(result);

    l = Wosize_val(input);
    dims[0] = (long)l;

    posix_memalign((void *)&out, CACHE_LINE_SIZE, l * sizeof(float));

    for (i = 0; i < l; i++) {
        out[i] = (float)Double_field(input, i);
    } 

    result = caml_ba_alloc(CAML_BA_FLOAT32 | CAML_BA_C_LAYOUT, 1, out, dims);

    CAMLreturn(result);
}

/* Creates float array from float Bigarray */
CAMLprim value array_of_bigarray (value input) {

    float *in;
    
    int i,
        l;

    CAMLparam1(input);
    CAMLlocal1(result);
    
    l  = Caml_ba_array_val(input)->dim[0];
    in = Caml_ba_data_val(input);

    result = caml_alloc(l * Double_wosize, Double_array_tag);

    for (i = 0; i < l; i++) {
        Store_double_field(result, i, (double)in[i]);        
    }     

    CAMLreturn(result);
}
