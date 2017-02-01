open Unix
open Scanf
open Bigarray

type mode = | Drive
            | Train
            | Stop

type video_status = | Success 
                    | Open_Error 
                    | IOCTL_Error 
                    | Select_Error 
                    | MMAP_Error 
                    | Close_Error

type capture_size = { 
                      capture_width:  int;
                      capture_height: int 
                    }

type feature_size = {
                      feature_width:  int;
                      feature_height: int
                    }

type filter_params = {
                       threshold:   int;
                       lower_limit: float;
                       upper_limit: float
                     }

type rc_params = {
                   speed:     float;
                   direction: float
                 }

type ba_float = (float, float32_elt, c_layout) Array1.t

type ba_int = (int, int8_unsigned_elt, c_layout) Array1.t

type layer   = {
                 inputs:  int; 
                 outputs: int;
                 input:   ba_float;
                 output:  ba_float;
                 bias:    ba_float;
                 weight:  ba_float
               }

type network = {
                 eta:    float;
                 layers: layer list
               }

type channel = {
                 mutable input:  in_channel;
                 mutable output: out_channel
               }

type scan_buffer = {
                     mutable buffer: Scanning.scanbuf
                   }
