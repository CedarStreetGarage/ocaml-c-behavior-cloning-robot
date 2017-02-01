open Types
open Config
open Monad
open Alignedba

external decompose_video : ba_int -> capture_size -> ba_float -> feature_size -> filter_params -> video_status = "decompose_video"
external make_feature_string : feature_size -> unit = "initialize_feature_string"
external string_of_feature : ba_float -> string = "string_of_feature"

let _ = make_feature_string feature_size

let make_feature feature_size = 
    create_float (feature_size.feature_width * feature_size.feature_height)

let decompose frame feature = 
    decompose_video frame frame_size feature feature_size filter_params
