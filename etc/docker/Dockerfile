FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        clang-20 \
        cmake \
        curl \
        libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://mirrors.kernel.org/gnu/make/make-4.4.1.tar.gz \
    | tar -xz -C /tmp \
    && cd /tmp/make-4.4.1 \
    && ./configure --prefix=/usr/local \
    && make -j"$(nproc)" \
    && make install \
    && rm -rf /tmp/make-4.4.1

WORKDIR /rukh

COPY Makefile ./
COPY etc/make/ ./etc/make/
COPY etc/cmake/ ./etc/cmake/
COPY src/ ./src/

RUN make rukh.build \
    RUKH_PROFILE=release \
    RUKH_CMAKE_CONFIGURE_ARGS=-DCMAKE_CXX_COMPILER=clang++-20

FROM ubuntu:24.04 AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates libcurl4 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/rukh

COPY --from=build /rukh/_build/engine/release/rukh ./rukh

ENTRYPOINT ["./rukh"]
