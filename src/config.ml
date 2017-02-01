open Types


(* Video capture *)
let video_device = "/dev/video0"
let frame_size   = { capture_width = 160; capture_height = 120 }

(* Feature vector *)
let feature_size  = { feature_width = 8; feature_height = 8 }
let filter_params = { threshold = 128; lower_limit = 0.25; upper_limit = 0.75 }

(* Control interface *)
let control_interface_device = "/dev/tty0"
let mode_command             = "Mode"
let mode_train               = "Train"
let mode_drive               = "Drive"
let rc_params_command        = "RC"

(* Serial device for motor control *)
let motor_control_device = "/dev/tty1"
