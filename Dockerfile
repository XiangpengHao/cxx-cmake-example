From ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC
ENV PATH="/root/.cargo/bin:${PATH}" CC=clang-17 CXX=clang++-17

RUN apt-get update \
	&& apt-get install -y build-essential curl cmake git wget lsb-release software-properties-common 

RUN curl https://sh.rustup.rs -sSf | bash -s -- --default-toolchain 1.74.1 -y

RUN wget https://apt.llvm.org/llvm.sh \
	&& chmod +x ./llvm.sh \
	&& ./llvm.sh 17 no

COPY . /usr/src/cxx_example
WORKDIR /usr/src/cxx_example

RUN cargo install cxxbridge-cmd --version 1.0.110

RUN mkdir docker-build && cd docker-build && cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_LTO=ON .. \
	&& make && ./main
