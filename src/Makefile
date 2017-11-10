################################################################################
# Makefile for ARM Linux                                                       #
# Tested on EDS SoC 16.1 on Windows 7 64-Bit                                   #
#                                                                              #
# Copyright (C) 2015 by Joel Bodenmann <joel@embedded.pro>                     #
# Modified 10 November 2017 by Byron Quezada <github.com/bqUAB>                #
#                                                                              #
# Feel free to use this makefile without any warranty on your own risk.        #
################################################################################

# This is the name of the binaries that will be generated
TARGET            = mpu_9250_demo

# Here we add all *.c files that we want to compile
CSRCS             = main.c

# Here we add the paths to all include directories
INCS              = ../include

# Here we add the standard libraries used
LIBS              =

# Parameters for SCP upload. Set up SSH keys to bypass password prompt
SCP_TARGET_IP   = 192.168.0.11
SCP_USER        = debian
SCP_TARGET_PATH = /home/debian/bin
SCP             = scp
SCP_FLAGS       =

# Compiler settings
ARCH    = arm-linux-gnueabihf
CC      = $(ARCH)-gcc
SIZE    = $(ARCH)-size
CFLAGS  = -g -std=gnu99 -Wall
LDFLAGS = -g -Wall

################################################################################
# Don't change anything below this line                                        #
################################################################################

# Some directory and file magic
BINDIR   = ../bin
BUILDDIR = ../build
OBJDIR   = $(BUILDDIR)/obj

# Generate the object names
OBJS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(CSRCS:%.c=%.o))))

# Add some paths
CFLAGS  += $(INCS:%=-I %)
LDFLAGS += $(INCS:%=-I %)

# Add standard libraries
CFLAGS  += $(LIBS)

# This is the default target if the user does just calls 'make'
all: build size

# Build all the files
build: builddirs $(BINDIR)/$(TARGET)

# Create the required directories (if not already existing)
builddirs:
	@echo
	@echo "Creating directories..."
	@mkdir -p $(BINDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)

# Link everything together
$(BINDIR)/$(TARGET): $(OBJS)
	@echo
	@echo Linking $@
	@$(CC) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS)

# Compile c files
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo
	@echo Compiling $^
	@$(CC) $(CFLAGS) -c -o $@ $^

# Print size information
size: $(BINDIR)/$(TARGET)
	@echo
	$(SIZE) $^

# Clean up
clean:
	@echo
	@echo "Cleaning..."
	@rm -rf $(BINDIR) $(BUILDDIR) $(OBJS) $(TARGET) $(TARGET).* $(INCS)/*~ \
	  *.a *.o *~ core
	@echo
	@echo Done

# Clean must be a phony target so make knows this never exists as a file
.PHONY: clean

# Upload to target
upload:
	$(SCP) $(SCP_FLAGS) $(BINDIR)/$(TARGET) \
	$(SCP_USER)@$(SCP_TARGET_IP):$(SCP_TARGET_PATH)
