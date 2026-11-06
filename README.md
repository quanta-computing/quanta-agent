# monikor
a simple pluggable monitoring agent

# build

```
autoreconf -f -i
./configure
make
```

# tests

```
make check
```

`tests/check_libmonikor` covers the metric library; `tests/check_varnish` runs the varnish
module's parser over saved `varnishstat -1` outputs (`tests/varnish/fixtures/*.txt`, one per
Varnish generation) and compares the extracted metrics with the matching `.expected` file.

To check what the module would extract from a real server, save `varnishstat -1 > capture.txt`
there and run `tests/varnishstat-parse capture.txt` (built by `make check`); its output is the
`.expected` format, so a capture can be added as a new fixture as is.

# install

make install

# build debian packages

debuild -us -uc
