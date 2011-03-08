ls -1 output | awk '{ system("awk -f tests/parse.awk output/" $0); }' | tee output\summary.log
