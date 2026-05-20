#!/bin/sh

if getent passwd quanta-agent > /dev/null 2>&1; then
  userdel quanta-agent
fi
if getent group quanta-agent > /dev/null 2>&1; then
  groupdel quanta-agent
fi
