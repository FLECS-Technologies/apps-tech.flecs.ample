#
# Copyright 2021-2023 FLECS Technologies GmbH

FROM --platform=linux/amd64 flecspublic.azurecr.io/flecs-build AS builder

ARG ARCH
RUN [ ! -z "${ARCH}" ]

COPY app /usr/src/app/app
WORKDIR /usr/src/app

RUN cmake -S app/ -B app/build/ -G Ninja -DARCH=${ARCH} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/fs
RUN cmake --build app/build/
RUN cmake --build app/build/ --target install
