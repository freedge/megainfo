FROM quay.io/centos/centos:stream10-development AS build
WORKDIR /src
# we install glibc-static and build a static binary
# that can be extracted from the image and run on older systems
RUN dnf -y install 'dnf-command(config-manager)' && \
    dnf -y config-manager --set-enabled crb && \
    dnf install -y make gcc pciutils-devel glibc-static
COPY *.c *.h Makefile .
ENV LDFLAGS="-static -O3"
RUN make

FROM quay.io/centos/centos:stream10-development-minimal
COPY --from=build /src/megainfo /usr/bin/megainfo
# seems necessary to access the device
LABEL io.containers.capabilities=SYS_ADMIN
ENTRYPOINT ["/usr/bin/megainfo"]

