open Types
open Config
open Monad
open Alignedba 
open Statement

external initialize : capture_size -> string -> video_status = "initialize_video"
external capture_frame : ba_int -> video_status = "capture_video"
external close_video : unit -> video_status = "close_video"

let print_video_error error =
    match error with
        | Open_Error   -> print_open_error ()
        | IOCTL_Error  -> print_ioctl_error ()
        | Select_Error -> print_select_error ()
        | MMAP_Error   -> print_mmap_error ()
        | Close_Error  -> print_close_error ()
        | _            -> ()

let make_frame frame_size = 
    create_int (frame_size.capture_width * frame_size.capture_height)

let open_video frame_size device =
    let video = initialize frame_size device
    in
    match video with
        | Success     -> true
        | IOCTL_Error -> (print_video_error video) >>
                         (close_video ()) >>
                         false
        | _           -> (print_video_error video) >>
                         false

let capture frame =
    let video = capture_frame frame
    in 
    match video with
        | Success -> true
        | _       -> (print_video_error video) >>
                     close_video () >>
                     false
