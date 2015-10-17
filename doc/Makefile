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

EXECUTABLES = doxygen dot 
CHECK_EXE := \
$(foreach exec,$(EXECUTABLES),\
$(if $(shell PATH=$(PATH) which $(exec)),,$(error "No $(exec) in PATH)))

PDF_EXECUTABLES = pdflatex
CHECK_PDF_EXE := \
$(foreach exec,$(PDF_EXECUTABLES),\
$(if $(shell PATH=$(PATH) which $(exec)),,$(error "No $(exec) in PATH)))

CONFIG_FILE ?= config/doxygen_config

.PHONY: all

all: clean doc

doc: $(CHECK_EXE)
	doxygen $(CONFIG_FILE)

pdf: $(CHECK_PDF_EXE)
	$(shell cd latex; make pdf; cp refman.pdf ..; cd..)

clean:
	$(RM) html latex doxygen*.db refman.pdf