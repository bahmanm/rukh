FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        clang-20 \
        cmake \
        libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /rukh

COPY CMakeLists.txt ./
COPY etc/cmake/ ./etc/cmake/
COPY src/ ./src/

RUN cmake -S . -B /_build/linux-amd64-release \
        -DCMAKE_CXX_COMPILER=clang++-20 \
        -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /_build/linux-amd64-release --parallel 4

FROM ubuntu:24.04 AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates libcurl4 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/rukh

COPY --from=build /_build/linux-amd64-release/rukh ./rukh

ENTRYPOINT ["./rukh"]
