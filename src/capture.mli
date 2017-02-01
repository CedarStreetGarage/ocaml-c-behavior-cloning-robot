open Types
open Bigarray

val open_video : capture_size -> string -> bool
val make_frame : capture_size -> ba_int
val capture : ba_int -> bool
