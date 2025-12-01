FROM debian:bookworm-slim AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY backend ./backend

RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    cmake --build . --config Release

FROM debian:bookworm-slim AS runtime

WORKDIR /app

COPY --from=builder /app/build/backend/taskboard_backend /app/taskboard_backend

EXPOSE 5000

CMD ["./taskboard_backend"]