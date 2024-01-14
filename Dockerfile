FROM ubuntu:22.04

COPY . ./app

WORKDIR ./app

RUN apt-get update && \
    apt-get install -y build-essential python3 python3-pip cmake ninja-build && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN  pip install --user conan

RUN  ~/.local/bin/conan profile detect --force

RUN  ~/.local/bin/conan install . --output-folder=build && \
    cd build && \
    cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

# lets add build to the path env, so the user can invoke the binaries from there.    
ENV PATH=$PATH:/app/build/