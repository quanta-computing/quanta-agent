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
module's parser over `varnishstat -1`-formatted fixtures (`tests/varnish/fixtures/*.txt`, one per
Varnish generation, with that version's real counter list and layout but synthetic values) and
compares the extracted metrics with the matching `.expected` file.

To check what the module would extract from a real server, run `varnishstat -1 > capture.txt` on
that server and feed the file to `tests/varnishstat-parse` (built by `make`, also reads stdin);
its output is the `.expected` format. To turn a capture into a fixture, anonymize the VCL and
backend names (`VBE.*` lines), check the printed values against the capture by hand, save both
files under `tests/varnish/fixtures/` and add a test case for it in `tests/varnish/check.c`.

# install

make install

# build debian packages

debuild -us -uc
