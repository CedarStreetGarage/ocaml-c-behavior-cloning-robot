#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/bigarray.h>
#include "defs.h"


/* A few activation functions and derivatives */
float tanh_fun (float x) {
    return tanh(x);
}

float tanh_der (float x) {
    float t = tanh_fun(x);
    return 1.0 - t * t;
}

float relu_fun (float x) {
    if (x >= 0.0) {
        return x;
    }
    return 0.0;
}

float relu_der (float x) {
    if (x >= 0.0) {
        return 1.0;
    }
    return 0.0;
}

/* Generic activation function */
float activation_fun (float x) {
    return tanh_fun(x);
}

float activation_der (float x) {
    return tanh_der(x);
}

/* Quadratic cost gradient */
float quadratic_cost_gradient (float x, float y) {
    return x - y;
}

/* Generic cost gradient */
float cost_gradient (float x, float y) {
    return quadratic_cost_gradient(x, y);
}

/* (Inefficient) Box-Muller transform */
float rand_normal (float max) {
    float u1 = (float)rand() * max / (float)RAND_MAX;
    float u2 = (float)rand() * max / (float)RAND_MAX;
    return sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
}


CAMLprim value seed_rng (value unit) {

    CAMLparam1(unit);

    srand(time(NULL));

    CAMLreturn(Val_unit);
}


/* Takes layer (stucture of ints and bigarrays)
 * Returns layer
 */
CAMLprim value initialize_layer (value layer) {

    float max,
          *input,
          *output,
          *bias,
          *weight;

    int i,
        inputs,
        outputs;

    CAMLparam1(layer);

    inputs  = Int_val(Field(layer, NN_INPUTS));
    outputs = Int_val(Field(layer, NN_OUTPUTS));

    input  = Caml_ba_data_val(Field(layer, NN_INPUT));
    output = Caml_ba_data_val(Field(layer, NN_OUTPUT));
    bias   = Caml_ba_data_val(Field(layer, NN_BIAS));
    weight = Caml_ba_data_val(Field(layer, NN_WEIGHT));

    max = sqrt((float)inputs);

    for (i = 0; i < inputs; i++) {
        input[i] = 0.0;
    }
    for (i = 0; i < outputs; i++) {
        output[i] = 0.0;
        bias[i] = rand_normal(max);
    }
    for (i = 0; i < (inputs * outputs); i++) {
        weight[i] = rand_normal(max);
    }

    CAMLreturn(layer);
}


/* Takes layer and layer_input (bigarrays)
 * Mutates layer input to the new input
 * Mutates layer output to the new output
 * Returns output (bigarray)
 */
CAMLprim value feed_forward (value layer, value layer_input) {

    float r,
          *in,
          *input,
          *output,
          *bias,
          *weight;

    int i,
        j,
        z,
        inputs,
        outputs;

    CAMLparam2(layer, layer_input);
    CAMLlocal1(result);

    inputs  = Int_val(Field(layer, NN_INPUTS));
    outputs = Int_val(Field(layer, NN_OUTPUTS));

    in    = Caml_ba_data_val(layer_input);
    input = Caml_ba_data_val(Field(layer, NN_INPUT));
    
    result = Field(layer, NN_OUTPUT);
    output = Caml_ba_data_val(result);
    
    bias   = Caml_ba_data_val(Field(layer, NN_BIAS));
    weight = Caml_ba_data_val(Field(layer, NN_WEIGHT));

    for (i = 0; i < inputs; i++) {
        input[i] = in[i];
    }

    for (i = 0; i < outputs; i++) {
        r = bias[i];
        for (j = 0; j < inputs; j++) {
            z = i * inputs + j;
            r += weight[z] * input[j];
        }
        output[i] = activation_fun(r);
    } 

    CAMLreturn(result);
}


/* This takes layer and target_output (bigarray), and eta
 * Mutates layer output to the error term
 * Mutates layer biases and weights
 * Mutates layer input to the backpropagated input
 * Returns input (bigarray)
 */
// NEEDS TOTAL REWORK!!!
CAMLprim value back_propagate (value layer, value target_output, value eta) {

    float r,
          learning_constant,
          *input,
          *bias,
          *weight,
          *output,
          *target;

    int i,
        j,
        z,
        inputs,
        outputs;

    CAMLparam3(layer, target_output, eta);
    CAMLlocal1(result);
    
    inputs  = Int_val(Field(layer, NN_INPUTS));
    outputs = Int_val(Field(layer, NN_OUTPUTS));

    result = Field(layer, NN_INPUT);   
    input  = Caml_ba_data_val(result);

    output = Caml_ba_data_val(Field(layer, NN_OUTPUT));
    bias   = Caml_ba_data_val(Field(layer, NN_BIAS));
    weight = Caml_ba_data_val(Field(layer, NN_WEIGHT));

    target = Caml_ba_data_val(target_output);

    learning_constant = (float)Double_val(eta) / (float)inputs;

    /* http://neuralnetworksanddeeplearning.com/chap2.html */

    for (i = 0; i < outputs; i++) {         // Compute error term (replaces output) and update bias estimates
        r = target[i] - output[i];
        output[i] = activation_der(r); 
        bias[i]  -= output[i] * learning_constant;  
    }    

    for (i = 0; i < inputs; i++) {   
        r = 0.0;
        for (j = 0; j < outputs; j++) {     // Dot product of network error and weight
            z = i * outputs + j;
            r += weight[z] * output[j]; 
        }
        for (j = 0; j < outputs; j++) {     // Update weight estimates
            z = i * outputs + j;
            weight[z] -= r * input[i] * learning_constant;
        }
        input[i] = r;                       // Replace input with pre-image of input
    }

    CAMLreturn(result);
}
