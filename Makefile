#Templete to c++ project
CXX ?= g++

# path #
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable # 
BIN_NAME = emca

# extensions #
SRC_EXT = cpp

# remote host copy #
REMOTE_USER = pi
REMOTE_HOST = 192.168.1.15
REMOTE_DIR	= ~/emca_gases/
REMOTE_WAN	= ivan28823.duckdns.org
WAN_PORT	= 9797

# config file
CFG_FILE 		= mainConfig.cfg
SYSTEMD_UNIT	= install/emca.service

#container options
CONTAINER_NAME	= ivan28823/emcaworkspace

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# flags #
COMPILE_FLAGS = -Wall -g
INCLUDES = -I include -I /usr/include/cppconn -I src/
# Space-separated pkg-config libraries used by this project
LIBS = -lmysqlcppconn -lwiringPi -lm -lconfig++

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) -j$(NPROC) all

.PHONY: dirs
dirs:
	@echo "[LOG] Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "[LOG] Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "[LOG] Deleting directories"
	@$(RM) -rf $(BUILD_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: dirs $(BIN_PATH)/$(BIN_NAME)
	@echo "[LOG] Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "[LOG] Linking: $@"
	@$(CXX) $(OBJECTS) $(LIBS) -o $@

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "[LOG] Compiling: $< -> $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Install the binary on /usr/bin/ directory
install: dirs $(BIN_PATH)/$(BIN_NAME)
	@echo "[LOG] Installing on /usr/bin/$(BIN_NAME)" 
	@cp $(BIN_PATH)/$(BIN_NAME) /usr/bin/
	@echo "[LOG] copy $(CFG_FILE) on /etc/emca/config.cfg" 
	@mkdir -p /etc/emca/
	@cp $(CFG_FILE) /etc/emca/config.cfg 
	@echo "[LOG] creating /var/log/emca/ where is saved logfile"
	@mkdir -p /var/log/emca/
	@echo "[LOG] copy $(SYSTEMD_UNIT) on /etc/systemd/system/"
	@cp $(SYSTEMD_UNIT) /etc/systemd/system/
	@systemctl daemon-reload

# Copy all project to remote host, you have to fill remote variables
# this way is easier than type a scp command each time to copy the dev
# project to production on raspberry pi board
internet_copy:	
	@echo "[LOG] Copying project to remote host: $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)"
	@scp -P $(WAN_PORT) -r * $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR) 

# Over lan network
remote_copy:
	@echo "[LOG] Copying project to remote host: $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)"
	@rsync -avzhe ssh --progress * $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)

# Excecuting binary
exe:	dirs $(BIN_PATH)/$(BIN_NAME)
	@echo "[LOG] Excecuting $(BIN_NAME)"
	@./$(BIN_NAME)

#install qemu multiarch support
.PHONY: qemu
qemu:
	@echo "[LOG] qemu"
	@docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

#compile in amd64 for armv7l (raspberry pi 3) using qemu-arm-static
build_arm:
	@echo "[LOG] Building in docker container workdir: $(PWD)"
	@docker run --rm -it \
		-v $(PWD):/workdir  \
		$(CONTAINER_NAME)

build_ws:
	@echo "[LOG] Building docker container as $(CONTAINER_NAME)"
	@docker build -t $(CONTAINER_NAME) . 