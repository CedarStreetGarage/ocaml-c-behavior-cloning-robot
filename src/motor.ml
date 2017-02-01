open Types
open Config
open Monad
open Unix
open Printf

let descriptor = openfile motor_control_device [O_RDWR] 0o666

let channel = { 
                input  = in_channel_of_descr  descriptor;
                output = out_channel_of_descr descriptor 
              } 

let send data =
    output_string channel.output data >>
    flush channel.output

let speed y = send ("MS: " ^ (sprintf "%d\n" (int_of_float y)))

let direction y = send ("MD: " ^ (sprintf "%d\n" (int_of_float y)))
