OUTPUT_FILE   = robot

OCAML_FILES   = types.ml \
                config.ml \
                monad.mli \
                monad.ml \
                alignedba.mli \
                alignedba.ml \
                statement.mli \
                statement.ml \
                capture.mli \
                capture.ml \
                decompose.mli \
                decompose.ml \
                jpeg.mli \
                jpeg.ml \
                control.mli \
                control.ml \
                motor.mli \
                motor.ml \
                neuralnet.mli \
                neuralnet.ml \
                main.ml

C_FILES       = alignedba_c.c \
                capture_c.c \
                decompose_c.c \
                jpeg_c.c \
                neuralnet_c.c

ML_PKG_STRING = -package bigarray -linkpkg
C_LIB_STRING  = -cclib -lv4l2 -cclib -ljpeg
C_OPT_STRING  = -ccopt -O3 -ccopt -Wall
ML_OPT_STRING = -O3 -g
DEBUG         = 

SOURCE_DIR    = src
BUILD_DIR     = build
IMAGES_DIR    = images


robot:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	rm -rf $(IMAGES_DIR)
	mkdir $(IMAGES_DIR)
	cp $(SOURCE_DIR)/* $(BUILD_DIR)
	cd $(BUILD_DIR) && ocamlfind ocamlopt $(DEBUG) -o $(OUTPUT_FILE) $(C_LIB_STRING) $(C_OPT_STRING) $(OCAML_FILES) $(C_FILES) $(ML_PKG_STRING)
	cp $(BUILD_DIR)/$(OUTPUT_FILE) $(OUTPUT_FILE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(IMAGES_DIR)
	rm $(OUTPUT_FILE)
