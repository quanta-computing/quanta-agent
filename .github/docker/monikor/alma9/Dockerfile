FROM almalinux:9

COPY --chmod=755 ./.github/docker/scripts/entrypoint /usr/share/quanta

RUN bash -e <<EOF
dnf install -y epel-release dnf-plugins-core && \
dnf config-manager --set-enabled crb && \
dnf install -y \
  gcc \
  rpm-build \
  make \
  automake \
  autoconf \
  libtool \
  pkg-config \
  libcurl-devel \
  libyaml-devel \
  mysql-devel \
  postgresql-devel \
  redhat-lsb-core \
  check \
  check-devel
dnf clean all
dnf install -y varnish-libs-devel

EOF

ENTRYPOINT ["/usr/share/centreon/container.sh"]
