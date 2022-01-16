From ubuntu:21.04

ENV DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC
ENV PATH="/root/.cargo/bin:${PATH}" CC=clang-13 CXX=clang++-13

RUN apt update \
	&& apt install -y build-essential curl cmake git wget lsb-release software-properties-common 

RUN curl https://sh.rustup.rs -sSf | bash -s -- --default-toolchain 1.58.0 -y

RUN wget https://apt.llvm.org/llvm.sh \
	&& chmod +x ./llvm.sh \
	&& ./llvm.sh 13 no


COPY . /usr/src/cxx_example
WORKDIR /usr/src/cxx_example

RUN cargo install cxxbridge-cmd

RUN mkdir docker-build && cd docker-build && cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_LTO=ON .. \
	&& make && ./main
