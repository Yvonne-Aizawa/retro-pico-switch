# Docker Image Based on https://github.com/lukstep/raspberry-pi-pico-docker-sdk
FROM ubuntu:25.10 as build

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
                       git \
                       ca-certificates \
                       python3 \
                       tar \
                       build-essential \
                       gcc-arm-none-eabi \
                       libnewlib-arm-none-eabi \
                       libstdc++-arm-none-eabi-newlib \
                       cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Raspberry Pi Pico SDK
ARG SDK_PATH=/usr/local/picosdk
RUN git clone --depth 1 --branch 2.0.0 https://github.com/raspberrypi/pico-sdk $SDK_PATH && \
    cd $SDK_PATH && \
    git submodule update --init

# Fix pioasm compilation errors with GCC 15.2.0 - add missing cstdint includes
RUN sed -i '/#include "pio_enums.h"/a #include <cstdint>' $SDK_PATH/tools/pioasm/pio_types.h && \
    sed -i '1a #include <cstdint>' $SDK_PATH/tools/pioasm/output_format.h

ENV PICO_SDK_PATH=$SDK_PATH

# Build the Project
RUN mkdir /app
WORKDIR /app
COPY . .

RUN cmake -B build -DPICO_BOARD=pico_w -S .
RUN make -C build/

# Separate the Binaries for Exporting
FROM scratch
COPY --from=build /app/build/src/retro_pico_switch.uf2 /
COPY --from=build /app/build/lib/switch_controller_pico/examples/auto_press_example.uf2 /
COPY --from=build /app/build/lib/switch_controller_pico/examples/simple_button_example.uf2 /
