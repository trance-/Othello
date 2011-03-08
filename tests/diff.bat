ls -1 output | awk '{ system("gvim -d output/" $0 " tests/expected/" $0); }'
