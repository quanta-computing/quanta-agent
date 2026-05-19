#!/bin/sh

if getent group varnish > /dev/null 2>&1 && getent passwd quanta-agent > /dev/null 2>&1; then
  usermod -aG varnish quanta-agent || true
fi
