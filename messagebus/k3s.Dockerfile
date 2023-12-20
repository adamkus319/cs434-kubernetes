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
    curl \
    python3 \
    python3-pip

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
COPY . /workspace/

WORKDIR /workspace

# set environment variable(SERVER_ADDRESS) to 50051
ENV SERVER_ADDRESS=0.0.0.0:50051
ENV PEER_ADDRESSES=0.0.0.0:50052

# Run your build script
RUN chmod +x /workspace/tools/grpc_cpp.sh && \
    /workspace/tools/grpc_cpp.sh

RUN chmod +x /workspace/tools/build.sh && \
    /workspace/tools/build.sh

# Install Kubernetes Python client
RUN pip3 install kubernetes

# # Command to run when the container starts
# # Assuming the compiled binary is named 'main' and located in the build directory
CMD ["/workspace/build/main"]