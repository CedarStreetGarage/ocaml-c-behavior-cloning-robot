open Types
open Config
open Monad
open Capture
open Decompose
open Control
open Motor
open Neuralnet
open Statement
open Jpeg

(* Test values for when not connected *)
let get_mode () = Train
let get_rc_params () = { speed = 0.0; direction = 0.0 }

(* Main program *)
let fw      = feature_size.feature_width
let fh      = feature_size.feature_height
let network = {
                eta    = 0.25;
                layers = [make_layer (fw * fh) fh;
                          make_layer fh 2;
                          make_layer 2 1]
              }

let frame   = make_frame frame_size
let feature = make_feature feature_size

let () =
    if open_video frame_size video_device then 
        while true do
            let rc = capture frame >>
                     decompose frame feature >>
                     get_rc_params ()
            in
            match get_mode () with
                | Stop  -> print_stopped () >>
                           speed 0.0 >>
                           direction 0.0
                | Drive -> print_driving () >>
                           print_string (string_of_feature feature) >>
                           speed (evaluate network feature).(0) >>
                           direction rc.direction
                | Train -> print_training () >>
                           save_jpeg frame rc.direction >>
                           print_string (string_of_feature feature) >>
                           (* train network feature [|rc.direction|] >> *)
                           speed rc.speed >>
                           direction rc.direction
        done
