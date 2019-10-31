# Author: Ivan Moreno
#   July 2019
#
# to run this image use the next command
#   docker run --rm -it \
#       -v /path/to/workdir:/workdir \
#       ivan28823/emcaworkspace 

FROM arm32v7/debian:buster-slim AS builder

#	docker run --rm --privileged multiarch/qemu-user-static:register
# need to install qemu-arm-static in your system
#   for debian based apt-get install -y --no-install-recommends qemu-user-static
#   for archlinux based via aur yay -S qemu-user-static
# then copy /usr/bin/qemu-arch-static to workdir
COPY qemu-arm-static /usr/bin/qemu-arm-static

# install build dependencies 
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    gcc \
    g++ \
    make \
    git \
    sudo \
    apt-transport-https \
    ca-certificates \
    gnupg2 \
    software-properties-common && \
    cd /tmp/ && \
    git clone http://github.com/WiringPi/WiringPi.git wiringPi && \
    cd wiringPi && \
    ./build

FROM arm32v7/debian:buster-slim

# copy from builder
COPY qemu-arm-static /usr/bin/qemu-arm-static
COPY --from=builder /usr/local/lib/libwiringPi* /usr/lib/
COPY --from=builder /usr/local/include/wiring* /usr/include/

# install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libconfig++-dev \
    libmysqlcppconn-dev \
    libi2c-dev \
    gcc \
    g++ \
    git \
    make && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#change workdir
WORKDIR /workdir

RUN useradd -Ms /bin/bash develop && chown develop /workdir

USER develop

#set entry point
ENTRYPOINT [ "/usr/bin/make" ]