# monikor
a simple pluggable monitoring agent

# build

autoreconf -f -i
./configure
make

# install

make install

# build debian packages

debuild -us -uc
