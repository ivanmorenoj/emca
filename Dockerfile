# Author: Ivan Moreno
#   July 2019

FROM docker.io/arm32v7/debian:buster-slim AS builder

# install QEMU multiarch for arm
#	docker run --rm --privileged docker.io/multiarch/qemu-user-static --reset -p yes

# Download qemu-user-static
ADD https://github.com/multiarch/qemu-user-static/releases/latest/download/qemu-arm-static /usr/bin/qemu-arm-static

# install build dependencies 
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    gcc g++ make git sudo apt-transport-https ca-certificates gnupg2 \
    software-properties-common && \
    cd /tmp/ && \
    git clone http://github.com/WiringPi/WiringPi.git wiringPi && \
    cd wiringPi && \
    ./build

FROM docker.io/arm32v7/debian:buster-slim

# copy from builder
COPY --from=builder /usr/bin/qemu-arm-static /usr/bin/qemu-arm-static
COPY --from=builder /usr/local/lib/libwiringPi* /usr/lib/
COPY --from=builder /usr/local/include/wiring* /usr/include/

ENV DEBIAN_FRONTEND noninteractive
# install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libconfig++-dev libmysqlcppconn-dev \
    libi2c-dev gcc g++ git make && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#change workdir
WORKDIR /workdir

RUN useradd -Ms /bin/bash develop && chown develop:develop /workdir

USER develop

#set entry point
ENTRYPOINT [ "/usr/bin/make" ]
