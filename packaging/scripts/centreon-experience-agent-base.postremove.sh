#!/bin/sh

case "$1" in
  purge|0) ;;
  *) exit 0 ;;
esac

rm -Rf /var/run/quanta

if getent passwd quanta-agent > /dev/null 2>&1; then
  userdel quanta-agent
fi
if getent group quanta-agent > /dev/null 2>&1; then
  groupdel quanta-agent
fi
