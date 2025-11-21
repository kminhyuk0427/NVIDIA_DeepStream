################################################################################
# SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: LicenseRef-NvidiaProprietary
#
# NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
# property and proprietary rights in and to this material, related
# documentation and any modifications thereto. Any use, reproduction,
# disclosure or distribution of this material and related documentation
# without an express license agreement from NVIDIA CORPORATION or
# its affiliates is strictly prohibited.
################################################################################

#Implementing a counter -> 12.6
CUDA_VER?=12.6

#Implementing a counter
ifeq ($(CUDA_VER),12.2)
  CUDA_HOME?=/usr/local/cuda-12.2
else
  CUDA_HOME?=/usr/local/cuda-$(CUDA_VER)
endif

ifeq ($(CUDA_VER),)
  $(error "CUDA_VER is not set")
endif

APP:= deepstream-app

TARGET_DEVICE = $(shell gcc -dumpmachine | cut -f1 -d -)

NVDS_VERSION:=7.1

LIB_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/
APP_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/bin/

ifeq ($(TARGET_DEVICE),aarch64)
  CFLAGS:= -DPLATFORM_TEGRA
endif

SRCS:= $(wildcard *.c) $(wildcard *.cpp)
SRCS+= $(wildcard ../../apps-common/src/*.c)
SRCS+= $(wildcard ../../apps-common/src/deepstream-yaml/*.cpp)

#Implementing a counter
SRCS+= mqtt/mqtt_client.c
# SRCS+= count_manager.c

INCS:= $(wildcard *.h)

PKGS:= gstreamer-1.0 gstreamer-video-1.0 x11 json-glib-1.0

OBJS:= $(SRCS:.c=.o)
OBJS:= $(OBJS:.cpp=.o)

CFLAGS+= -I./ -I../../apps-common/includes \
		 -I../../../includes -DDS_VERSION_MINOR=1 -DDS_VERSION_MAJOR=5 \
		 -I$(CUDA_HOME)/include
#-I /usr/local/cuda-$(CUDA_VER)/include

#Implementing a counter
CFLAGS+= -Iinclude
#Implementing a counter
CFLAGS+= -I./mqtt

#LIBS:= -L/usr/local/cuda-$(CUDA_VER)/lib64/ -lcudart
LIBS:= -L$(CUDA_HOME)/lib64/ -lcudart

LIBS+= -L$(LIB_INSTALL_DIR) -lnvdsgst_meta -lnvds_meta -lnvdsgst_helper -lnvdsgst_customhelper \
	  -lnvdsgst_smartrecord -lnvds_utils -lnvds_msgbroker -lm -lyaml-cpp \
    -lcuda -lgstrtspserver-1.0 -ldl -Wl,-rpath,$(LIB_INSTALL_DIR)

CFLAGS+= $(shell pkg-config --cflags $(PKGS))

LIBS+= $(shell pkg-config --libs $(PKGS))

#Implementing a counter
LIBS+= -lmosquitto -lrt

all: $(APP)

%.o: %.c $(INCS) Makefile
	$(CC) -c -o $@ $(CFLAGS) $<

%.o: %.cpp $(INCS) Makefile
	$(CXX) -c -o $@ $(CFLAGS) $<

$(APP): $(OBJS) Makefile
	$(CXX) -o $(APP) $(OBJS) $(LIBS)

install: $(APP)
	cp -rv $(APP) $(APP_INSTALL_DIR)

clean:
	rm -rf $(OBJS) $(APP)
