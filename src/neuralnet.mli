open Types

val make_layer : int -> int -> layer
val train : network -> ba_float -> float array -> float array
val evaluate : network -> ba_float-> float array
