open Types
open Config
open Monad
open Scanf
open Unix

let descriptor = openfile control_interface_device [O_RDWR] 0o666

let channel = { 
                input  = in_channel_of_descr  descriptor;
                output = out_channel_of_descr descriptor
              }

let scanner = { 
                buffer = Scanning.from_channel channel.input
              }

let request_data command = 
    output_string channel.output command >>
    flush channel.output

let ident = fun x -> x

let get_mode () =
    request_data mode_command >>
    match bscanf scanner.buffer "%s\n" ident with
        | mode_train -> Train
        | mode_drive -> Drive
        | _          -> Stop

let get_rc_params () =
    request_data rc_params_command >>
    let speed     = bscanf scanner.buffer "%f " ident
    and direction = bscanf scanner.buffer "%f\n" ident
    in
    { speed = speed; direction = direction }
