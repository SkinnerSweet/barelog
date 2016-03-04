# The MIT License (MIT)
#
# Copyright (c) 2015 Thomas Bertauld <thomas.bertauld@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

RM = rm -rf
MKDIR = mkdir

CC = $(CROSS_COMPILE)gcc
LD = $(CC)

HLIBTYPE ?= a
TLIBTYPE ?= a

TARGET_CC ?= e-gcc -T $(EPIPHANY_HOME)/bsps/current/fast.ldf
TCC = $(TARGET_CC)
TLD = $(TCC)
TLIBS = -lc

AR = ar rcs

HOST_DIR = ./host
TARGET_DIR = ./target
COMMON_DIR = ./common
PLATFORM_DIR = ./platforms

TINCLUDE_DIR = $(TARGET_DIR)/include
HINCLUDE_DIR = $(HOST_DIR)/include
CINCLUDE_DIR = $(COMMON_DIR)/include

HINCLUDE = -I $(HINCLUDE_DIR) 
TINCLUDE = -I $(TINCLUDE_DIR) 
CINCLUDE = -I. -I $(CINCLUDE_DIR) -I $(PLATFORM_DIR)

CCFLAGS = -O2 -std=c99 $(CINCLUDE) -Wall
HCFLAGS = $(CCFLAGS)
TCFLAGS = $(CCFLAGS)
SOFLAGS = -fpic

LIBDIR = ../libs

ifeq ($(HLIBTYPE),so)
	HCFLAGS = $(HCFLAGS) $(SOFLAGS)
endif

ifeq ($(TLIBTYPE),so)
	TCFLAGS = $(TCFLAGS) $(SOFLAGS)
endif

TTARGET = barelog_logger
HTARGET = barelog_host

TOBJS = $(TTARGET).o barelog_device_mem_manager.o barelog_event_target.o barelog_snprintf.o
HOBJS = $(HTARGET).o barelog_host_mem_manager.o barelog_event.o

.PHONY: all

all: host target clean

host: $(LIBDIR) $(HTARGET).$(HLIBTYPE)

target: $(LIBDIR) $(TTARGET).$(TLIBTYPE)

$(HTARGET).so: $(HOBJS) 
	$(LD) -o $(LIBDIR)/lib$@ -shared $^

$(HTARGET).a: $(HOBJS)
	$(AR) $(LIBDIR)/lib$@ $^

$(TTARGET).so: $(TOBJS)
	$(TLD) -o $(LIBDIR)/lib$@ -shared $^ $(TLIBS)

$(TTARGET).a: $(TOBJS)
	$(AR) $(LIBDIR)/lib$@ $^

$(HTARGET).o: $(HOST_DIR)/$(HTARGET).c $(HINCLUDE_DIR)/$(HTARGET).h 
	$(CC) $(HCFLAGS) $(HINCLUDE) -c $< 

$(TTARGET).o: $(TARGET_DIR)/$(TTARGET).c $(TINCLUDE_DIR)/$(TTARGET).h 
	$(TCC) $(TCFLAGS) $(TINCLUDE) -c $< $(TLIBS)

barelog_host_mem_manager.o: $(HOST_DIR)/barelog_host_mem_manager.c $(HINCLUDE_DIR)/barelog_host_mem_manager.h $(COMMON_DIR)/barelog_mem_space.c $(CINCLUDE_DIR)/barelog_mem_space.h
	$(CC) $(HCFLAGS) $(HINCLUDE) -c $<

barelog_device_mem_manager.o: $(TARGET_DIR)/barelog_device_mem_manager.c $(TINCLUDE_DIR)/barelog_device_mem_manager.h
	$(TCC) $(TCFLAGS) $(TINCLUDE) -c $< $(TLIBS)

barelog_event.o: $(COMMON_DIR)/barelog_event.c $(CINCLUDE_DIR)/barelog_event.h
	$(CC) $(HCFLAGS) -c $<

barelog_event_target.o: $(COMMON_DIR)/barelog_event.c $(CINCLUDE_DIR)/barelog_event.h
	$(TCC) $(TCFLAGS) -c $< -o $@ $(TLIBS) 

barelog_snprintf.o: $(TARGET_DIR)/barelog_snprintf.c $(TINCLUDE_DIR)/barelog_snprintf.h
	$(TCC) $(TCFLAGS) $(TINCLUDE) -c $< $(TLIBS) 

$(LIBDIR):
	$(MKDIR) $@

clean:
	$(RM) $(HTARGET).o $(HOBJS)
	$(RM) $(TTARGET).o $(TOBJS)

mrproper: clean
	$(RM) $(LIBDIR)/lib$(HTARGET).so $(LIBDIR)/lib$(TTARGET).so
	$(RM) $(LIBDIR)/lib$(HTARGET).a $(LIBDIR)/lib$(TTARGET).a