FROM mcr.microsoft.com/devcontainers/base:ubuntu
# Why not ubuntu:latest? Because the devcontainer base image has some optimizations and tools pre-installed that are useful for development.

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    gdb \
    llvm \
    libclang-rt-18-dev \
    valgrind \
    make \
    git \
    vim \
    curl \
    ca-certificates \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Optional but nice: install GitHub CLI (gh)
RUN type -p curl >/dev/null || (apt-get update && apt-get install -y curl) \
 && curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg \
    | dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg \
 && chmod go+r /usr/share/keyrings/githubcli-archive-keyring.gpg \
 && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" \
    > /etc/apt/sources.list.d/github-cli.list \
 && apt-get update \
 && apt-get install -y gh \
 && apt-get clean && rm -rf /var/lib/apt/lists/*
