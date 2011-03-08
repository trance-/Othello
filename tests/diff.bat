echo summary.log | awk '{ system("gvim -d output/" $0 " tests/expected/" $0); }'
