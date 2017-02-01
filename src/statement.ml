open Unix
open Monad
open Printf

let print s = printf "%.3f - %s\n%!" (gettimeofday ()) s 

let print_string f  = printf "%s%!" f

let print_stopped ()      = print "Stopped"
let print_driving ()      = print "Driving"
let print_training ()     = print "Training"
let print_open_error ()   = print "Capture: Error on open()"
let print_ioctl_error ()  = print "Capture: Error on ioctl()"
let print_select_error () = print "Capture: Error on select()"
let print_mmap_error ()   = print "Capture: Error on mmap()"
let print_close_error ()  = print "Capture: Error on close()"
