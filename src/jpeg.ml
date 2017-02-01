open Types
open Config
open Unix
open Printf

external save_jpeg_ocaml : string -> ba_int -> capture_size -> unit = "save_jpeg_ocaml_int"

let save_jpeg frame direction =
    let s = sprintf "images/%.3f_%.3f.jpg" (gettimeofday ()) direction
    in 
    save_jpeg_ocaml s frame frame_size
