# Author: Ivan Moreno
#   July 2019
#
# to run this image use the next command
#   docker run --rm -it \
#       -v /usr/bin/qemu-arm-static:/usr/bin/qemu-arm-static \
#       -v /path/to/workdir:/workdir \
#       ivan28823/emcaworkspace \
#       bash -c "cd /workdir && make clean && make"

FROM arm32v7/debian:buster-slim

# need to install qemu-arm-static in your system
#   for debian based apt-get install -y --no-install-recommends qemu-user-static
#   for archlinux based via aur yay -S qemu-user-static
# then copy /usr/bin/qemu-arch-static to workdir
COPY qemu-arm-static /usr/bin/qemu-arm-static

#update and upgrade
RUN apt-get update && apt-get -y upgrade

# install dependencies
RUN apt-get install -y --no-install-recommends \
    libconfig++-dev \
    libmysqlcppconn-dev \
    libi2c-dev \
    gcc \
    g++ \
    make \
    git \
    sudo

# get wiringPi from source
RUN cd /tmp/ && \
    git clone git://git.drogon.net/wiringPi && \
    cd wiringPi && \
    ./build && cd /

#clean
RUN export SUDO_FORCE_REMOVE=yes && \
    apt-get purge -y git sudo && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#change workdir
WORKDIR /workdir

#set entry point
ENTRYPOINT [ "/usr/bin/make" ]