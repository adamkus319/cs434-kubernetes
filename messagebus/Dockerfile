FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    git \
    wget \
    clang \
    ca-certificates \
    build-essential \
    libssl-dev \
    make \
    cmake \
    autoconf \
    automake \
    pkg-config \
    libtool \
    golang \
    curl

RUN cd / && git clone -b v1.38.0 https://github.com/grpc/grpc && \
    cd /grpc && \
    git submodule update --init && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
    ../.. && \
    make && \
    make install && \
    ldconfig

RUN cp -r /grpc/third_party/abseil-cpp/absl /usr/local/include/
RUN cp -r /grpc /usr/local/include/

WORKDIR /workspace