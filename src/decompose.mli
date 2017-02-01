open Types
open Bigarray

val string_of_feature : ba_float -> string
val make_feature : feature_size -> ba_float
val decompose :  ba_int -> ba_float -> video_status
