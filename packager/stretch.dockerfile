FROM debian:stretch
MAINTAINER Matthieu 'Korrigan' ROSINSKI <mro@quanta-computing.com>

RUN apt-get update && apt-get install -y curl gnupg
RUN echo 'deb http://apt.quanta.gr quanta/' > /etc/apt/sources.list.d/quanta.list
RUN curl -s https://www.quanta-monitoring.com/quanta-repo-key.gpg -o - | apt-key add -

RUN apt-get update && apt-get install -y \
  build-essential \
  debhelper \
  dh-exec \
  dh-autoreconf \
  autotools-dev \
  devscripts \
  dput \
  vim \
  quilt \
  openssh-client \
  libyaml-dev \
  libcurl4-openssl-dev \
  sendmail \
  wget

RUN apt-get update && apt-get install -y \
  varnish \
  libmariadbclient-dev-compat \
  libvarnishapi-dev \
  libvarnishapi1
