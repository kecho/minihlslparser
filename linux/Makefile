
CC := gcc
MKDIR := mkdir
AR := ar

MHLSL_SRC := ../src
MHLSL_SAMPLES := ../samples
BUILD_FOLDER := ../build
MHLSL_SRC_FILES := minihlslparser.cpp
MHLSL_SAMPLES_FILES := simpleparser.cpp
MHLSL_SAMPLES_FILES_ABS := $(addprefix $(MHLSL_SAMPLES)/, $(MHLSL_SAMPLES_FILES))
MHLSL_SRC_FILES_ABS := $(addprefix $(MHLSL_SRC)/, $(MHLSL_SRC_FILES))

default: minihlsl samples
	

minihlsl: $(BUILD_FOLDER) $(MHLSL_SRC_FILES_ABS)
	$(CC) -c $(MHLSL_SRC_FILES_ABS) -I $(MHLSL_SRC) 
	$(AR) cvr $(BUILD_FOLDER)/libmhlsl.a *.o

samples: $(MHLSL_SAMPLES_FILES_ABS)
	$(foreach FILE, $(MHLSL_SAMPLES_FILES_ABS), $(CC) $(FILE) -L$(BUILD_FOLDER) -I $(MHLSL_SRC) -lmhlsl  -o $(BUILD_FOLDER)/$(basename $(notdir $(FILE))) )
		
$(BUILD_FOLDER):
	$(MKDIR) $(BUILD_FOLDER)
