#! /bin/bash

DEST_DIR=./codegen

if [ -d $DEST_DIR ]; then
    rm -rf $DEST_DIR
    mkdir $DEST_DIR
else
    mkdir $DEST_DIR
fi

protoc \
    --proto_path=./protobuf/rpc \
    --grpc_out=${DEST_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./protobuf/rpc/*.proto

protoc \
    --proto_path=./protobuf/rpc \
    --cpp_out=${DEST_DIR} ./protobuf/rpc/*.proto