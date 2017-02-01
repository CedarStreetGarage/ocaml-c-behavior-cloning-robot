open Types
open Config
open Monad
open List
open Alignedba

external seed_rng : unit -> unit = "seed_rng"
external initialize_layer : layer -> layer = "initialize_layer"
external feed_forward : layer -> ba_float -> ba_float = "feed_forward"
external back_propagate : layer -> ba_float -> float -> ba_float = "back_propagate"

let _ = seed_rng ()

let make_layer inputs outputs = 
    initialize_layer {
                       inputs  = inputs;
                       outputs = outputs;
                       input   = create_float inputs;
                       output  = create_float outputs;
                       bias    = create_float outputs;
                       weight  = create_float (inputs * outputs)
                     }

let rec forward layers input =
    match layers with
        | h::t -> forward t (feed_forward h input)
        | _    -> input

let rec backward layers target eta =
    match rev layers with
        | h1::h2::t -> let target' = back_propagate h1 h2.input eta
                       in
                       backward (h2::t) target' eta 
        | _         -> target

let evaluate net feature =
    array_of_ba (forward net.layers feature)

let train net feature target =
    forward net.layers feature >>
    array_of_ba (backward net.layers (ba_of_array target) net.eta)
