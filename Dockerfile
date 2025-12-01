# ---------- Build stage ----------
FROM debian:bookworm-slim AS builder

# Инструменти за build
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

# Configure & build с CMake
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    cmake --build . --config Release


# ---------- Runtime stage ----------
FROM debian:bookworm-slim AS runtime

WORKDIR /app

COPY --from=builder /app/build/backend/taskboard_backend /app/taskboard_backend


EXPOSE 5000

CMD ["./taskboard_backend"]
