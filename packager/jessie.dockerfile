FROM debian:jessie
MAINTAINER Matthieu 'Korrigan' ROSINSKI <mro@quanta-computing.com>

RUN apt-get update && apt-get install -y curl
RUN echo 'deb http://apt.quanta.io quanta/' > /etc/apt/sources.list.d/quanta.list
RUN curl -s https://apt.quanta.io/quanta-repo-key.gpg -o - | apt-key add -

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
  libmysqlclient-dev \
  libpq-dev \
  sendmail \
  wget

RUN apt-get update && apt-get install -y \
  varnish \
  libvarnishapi-dev \
  libvarnishapi1
