FROM node:6

COPY sources.list /etc/apt/sources.list

COPY . /src

RUN apt-get update && apt-get install -y --no-install-recommends \
  git \
  unzip \
  cmake \
  libv4l-dev \
  opencl-headers \
  vim \
  \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/local

RUN curl -SLO "https://codeload.github.com/opencv/opencv/zip/2.4.13.2" \
  && unzip 2.4.13.2 \
  && mv /usr/local/opencv-2.4.13.2 /usr/local/opencv \
  && rm 2.4.13.2

RUN mkdir /usr/local/opencv/build

WORKDIR /usr/local/opencv/build

RUN cmake -D CMAKE_BUILD_TYPE=RELEASE \
  -D BUILD_PYTHON_SUPPORT=ON \
  -D CMAKE_INSTALL_PREFIX=/usr/local \
  -D WITH_OPENGL=ON \
  -D WITH_TBB=OFF \
  -D BUILD_EXAMPLES=ON \
  -D BUILD_NEW_PYTHON_SUPPORT=ON \
  -D WITH_V4L=ON \
  -D WITH_OPENCL=ON \
  ..

RUN make -j7
RUN make install

RUN npm -g i node-gyp@3.4.0

RUN ldconfig

WORKDIR /

COPY ./entrypoint.sh /

ENTRYPOINT ["/entrypoint.sh"]
