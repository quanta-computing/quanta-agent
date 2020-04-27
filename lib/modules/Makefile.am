AUTOMAKE_OPTIONS = foreign
SUBDIRS = apache \
  cpu \
  disk \
  memcached \
  memory \
  mysqlstat \
  network \
  nfs \
  nginx \
  postgresql \
  process \
  redis \
  system

if VARNISH3
  SUBDIRS += varnish3
else VARNISH4 || VARNISH5
  SUBDIRS += varnish
endif
