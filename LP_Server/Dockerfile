FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build \
    && cmake --build build --config Release

EXPOSE 8080

CMD ["./build/server"]
