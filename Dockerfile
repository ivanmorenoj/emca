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
    i2c-tools \
    libconfig++-dev \
    libmysqlcppconn-dev \
    libi2c-dev \
    build-essential \
    git-core \
    sudo \
    file

# get wiringPi from source
RUN cd /tmp/ && \
    git clone git://git.drogon.net/wiringPi && \
    cd wiringPi && \
    ./build && cd /root && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

CMD /bin/bash