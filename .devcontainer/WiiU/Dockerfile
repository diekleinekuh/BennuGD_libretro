FROM devkitpro/devkitppc:20251231
RUN apt-get install -y --no-install-recommends zlib1g-dev && rm -rf /var/lib/apt/lists/*
RUN git clone --recursive https://github.com/yawut/wiiurpxtool-ng && \
    cd wiiurpxtool-ng && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . && \
    cp ./wiiurpxtool-ng /usr/bin/wiiurpxtool && \
    cd ../.. && \
    rm -rf wiiurpxtool-ng

RUN useradd -ms /bin/bash containeruser
USER containeruser
