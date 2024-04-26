FROM quay.io/centos/centos:stream9 AS build
WORKDIR /src
RUN dnf install -y make gcc pciutils-devel
COPY *.c .
COPY Makefile .
RUN make

FROM quay.io/centos/centos:stream9-minimal
COPY --from=build /src/megainfo /usr/bin/megainfo
ENTRYPOINT ["/usr/bin/megainfo"]

