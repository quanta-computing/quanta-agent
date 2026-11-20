# syntax=docker/dockerfile:1

##############################################################################
# monikor (the agent, upstream quanta-agent) dev image
#
# The agent of the dev stack: it measures its own container and the stack's
# services (redis/valkey, mysql) and pushes to the dev app, like a customer
# server would. Built from this checkout, for amd64 and arm64:
#
#   docker build -f docker/Dockerfile -t qm3k-agent:dev .
#
# Two stages: the builder runs the README's autotools build
# (`autoreconf -f -i && ./configure && make`) plus the libmonikor unit tests;
# the runtime stage only receives the installed binary, libmonikor and the
# module shared objects, with the shared libraries they link against.
#
# The context is the repo root, filtered by Dockerfile.dockerignore (next to
# this file) so the outputs of host builds never reach the image build.
#
# No configuration is baked in: mount agent.yml at /etc/quanta/agent.yml and
# the module configs in /etc/quanta/modules.d (readable by uid 1000), or pass
# another path as the command's argument. In agent.yml, `modules.path` must be
# /usr/local/lib (where `make install` puts the modules); `daemonize: no` and
# `logger.file: stderr` keep the agent in the foreground with its logs on
# `docker logs`, and directory/pidfile/listen.path must be writable by the
# agent's user (/tmp). The dev stack runs this image as its `agent` service,
# with a config rendered at start from a read-only mount. Someday the
# quanta-agent repo may ship its own compose file to run several agents.
#
# The runtime stage runs as the unprivileged user qm3k (uid/gid 1000), never
# as root: the agent only reads /proc and talks to the services it measures
# over the network.
##############################################################################
ARG DEBIAN_IMAGE=debian:bookworm-slim

FROM ${DEBIAN_IMAGE} AS builder

# Toolchain and -dev packages of debian/control's Build-Depends, plus
# default-libmysqlclient-dev for the mysqlstat module (it links
# -lmysqlclient, which Debian provides through MariaDB's compat library).
# `check` is required by configure (PKG_CHECK_MODULES) and runs the tests.
RUN apt-get update && apt-get install -y --no-install-recommends \
  autoconf \
  automake \
  libtool \
  pkg-config \
  build-essential \
  libcurl4-openssl-dev \
  libyaml-dev \
  libpq-dev \
  default-libmysqlclient-dev \
  check \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/monikor
COPY . .

# Installed into a staging root so the runtime stage copies one tree, with
# the libmonikor symlinks intact. The static and libtool archives, the header
# and the sample configs that `make install` also produces are build-time or
# packaging material: dropped.
RUN autoreconf -f -i \
  && ./configure \
  && make -j"$(nproc)" \
  && make check \
  && make install DESTDIR=/staging \
  && rm -rf /staging/usr/local/include /staging/usr/local/etc \
  && rm -f /staging/usr/local/lib/*.a /staging/usr/local/lib/*.la

FROM ${DEBIAN_IMAGE}

# Libraries the binary and the modules link against directly (ldd in the
# builder): libcurl4 and libyaml-0-2 for monikor itself, libmariadb3 and
# zlib1g for mysqlstat, libpq5 for postgresql; their own dependencies come
# with them. ca-certificates because libcurl4 only recommends it and a push
# URL may be https.
RUN apt-get update && apt-get install -y --no-install-recommends \
  libcurl4 \
  libyaml-0-2 \
  libmariadb3 \
  libpq5 \
  zlib1g \
  ca-certificates \
  && rm -rf /var/lib/apt/lists/*

COPY --from=builder /staging/usr/local/ /usr/local/

# ldconfig makes libmonikor.so.0 resolvable from /usr/local/lib. The ldd
# check fails the build when the apt list above misses a library: modules are
# dlopen'ed, so a gap would otherwise only show at run time, as a module
# logged "disabled" and silently absent from the pushed metrics.
RUN ldconfig \
  && ! ldd /usr/local/bin/monikor /usr/local/lib/*.so | grep 'not found'

# The agent's user, owning the config directory (the mount point of the
# config, see the header). monikor still logs "Running as root is strongly
# discouraged!" at start: it says so whenever agent.yml sets no `user:`,
# whatever the uid, and setting one only makes a non-root agent log that it
# cannot drop privileges.
RUN groupadd --gid 1000 qm3k \
  && useradd --uid 1000 --gid qm3k --no-create-home --home-dir /nonexistent --shell /usr/sbin/nologin qm3k \
  && mkdir -p /etc/quanta/modules.d \
  && chown -R qm3k:qm3k /etc/quanta
USER qm3k

CMD ["monikor", "/etc/quanta/agent.yml"]
