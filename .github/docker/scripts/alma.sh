#!/bin/bash

if [ -z "$SOURCEDIR" ]; then
  echo "SOURCEDIR not defined, exiting"
  exit 1
fi

echo "setup"
mkdir /build
mkdir -p /root/rpmbuild/SOURCES
mkdir /usr/include/postgresql
# to keep a single version of .c between alma and debian
ln -s /usr/include/libpq-fe.h /usr/include/postgresql/libpq-fe.h

# move source away from mount point
cp -r /sources/tmp/$SOURCEDIR /build/

# build alma packages
echo "Autoreconf"
cd /build/$SOURCEDIR
if [ -f configure.ac ]; then # we have a configure.ac
  rm -f aclocal.m4
  aclocal
  libtoolize --force
  autoreconf -f -i
else # We don't, it's the php-quanta-mon package, let's generate it
  phpize
fi

cd .. && tar czvf /root/rpmbuild/SOURCES/$SOURCEDIR.tgz $SOURCEDIR

# build alma packages
echo "rpmbuild"
ls -lah /root/rpmbuild/SOURCES/
cd $SOURCEDIR
chown root: *.spec
rpmbuild -ba --clean *.spec

echo "move stuff"
# move packages to mount point inside pkg folder
version=`cat /etc/os-release | grep VERSION_ID | cut -d '"' -f 2`
pkg_path=/sources/alma/${version}/pkg/$OUTDIR
mkdir -p ${pkg_path}
cp /root/rpmbuild/RPMS/x86_64/*.rpm ${pkg_path}
