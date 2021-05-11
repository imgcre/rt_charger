ARG TEMPLATE_NAME=stm32f10x
ARG BUILDER_WORK_DIR=/root
ARG BUILDER_STM32_BSP_DIR=$BUILDER_WORK_DIR/rt-thread/bsp/stm32

FROM imgcre/rt_build_env:10-2020-q4-major as builder
ARG TEMPLATE_NAME
ARG BUILDER_WORK_DIR
ARG BUILDER_STM32_BSP_DIR
ARG RTT_GIT_URL=https://github.com/RT-Thread/rt-thread.git

WORKDIR $BUILDER_WORK_DIR
RUN git clone $RTT_GIT_URL --depth=1 && \
    cp -r \
        $BUILDER_STM32_BSP_DIR/libraries/templates/$TEMPLATE_NAME \
        $BUILDER_STM32_BSP_DIR/$TEMPLATE_NAME

WORKDIR $BUILDER_STM32_BSP_DIR/$TEMPLATE_NAME
COPY rtconfig.py.patch .

RUN pwd && \
    patch rtconfig.py rtconfig.py.patch && \
    rm rtconfig.py.patch && \
    scons -j32 --dist

FROM imgcre/rt_build_env:10-2020-q4-major
ARG TEMPLATE_NAME
ARG BUILDER_STM32_BSP_DIR
ARG PROJECTS_DIR=/root/projects
ARG SRC_DIR=${BUILDER_STM32_BSP_DIR}/${TEMPLATE_NAME}/dist/${TEMPLATE_NAME}
ARG DEST_DIR=$PROJECTS_DIR/$TEMPLATE_NAME

ENV DBG_SERVER=localhost \
    TELNET_PORT=4444 \
    GDB_PORT=3333

WORKDIR ${DEST_DIR}
COPY --from=builder $SRC_DIR .
COPY . .

RUN \
    #remove patch files
    rm *.patch project.* template.* && \
    # install packages for build&debug
    apt-get install -y \
        libncurses5 \
        telnet \
        python3-pip \
    && \
    apt-get clean && \
    # config env tool
    scons --pyconfig-silent && \
    echo ". ~/.env/env.sh" >> /root/.bashrc && \
    pip3 install requests

VOLUME ${PROJECTS_DIR}
