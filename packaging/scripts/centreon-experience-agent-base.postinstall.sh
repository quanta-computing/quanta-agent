#!/bin/sh

if [ -f /usr/share/debconf/confmodule ]; then
  . /usr/share/debconf/confmodule
  db_version 2.0 || [ $? -lt 30 ]
fi

HOSTID=$(ip link | grep link/ether | head -1 | awk '{print $2}' | tr -d ':')
if [ -f /etc/quanta/agent.yml ]; then
  sed -i "s/__YOUR_HOSTID__/$HOSTID/" /etc/quanta/agent.yml

  if [ -f /usr/share/debconf/confmodule ]; then
    db_get quanta-agent/quanta_token
    TOKEN="$RET"
    if [ -n "$TOKEN" ]; then
      sed -i "s/__YOUR_QUANTA_TOKEN__/$TOKEN/" /etc/quanta/agent.yml
    fi
  fi
fi

getent group quanta-agent > /dev/null || groupadd -r quanta-agent
getent passwd quanta-agent > /dev/null || \
  useradd -r -g quanta-agent -d /var/run/quanta -s /sbin/nologin \
  -c "Quanta agent" quanta-agent
if [ ! -d /var/run/quanta ]; then
  mkdir /var/run/quanta
  chown quanta-agent:quanta-agent /var/run/quanta
fi

systemctl daemon-reload ||:
systemctl unmask quanta-agent.service ||:
systemctl preset quanta-agent.service ||:
systemctl enable quanta-agent.service ||:
systemctl restart quanta-agent.service ||:

if [ -f /usr/share/debconf/confmodule ]; then
  db_stop
fi
