#!/bin/sh

HOSTID=$(ip link | grep link/ether | head -1 | awk '{print $2}' | tr -d ':')
if [ -f /etc/quanta/agent.yml ]; then
  sed -i "s/__YOUR_HOSTID__/$HOSTID/" /etc/quanta/agent.yml
fi

getent group quanta-agent > /dev/null || groupadd -r quanta-agent
getent passwd quanta-agent > /dev/null || \
  useradd -r -g quanta-agent -d /var/run/quanta -s /sbin/nologin \
  -c "Quanta agent" quanta-agent

if [ ! -d /var/run/quanta ]; then
  mkdir /var/run/quanta
  chown quanta-agent:quanta-agent /var/run/quanta
fi
