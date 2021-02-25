import sys
from pyboolector import Boolector

if __name__ == '__main__':
    f_in = open(sys.argv[1], 'r')
    f_out = open('check', 'w')
    for line in f_in:
        if line[:7] != '(assert':
            f_out.write(line)
        else:
            f_out.write('(assert (xor (\n')
    f_in = open(sys.argv[1][:-3] + '.out', 'r')
    for line in f_in:
        f_out.write(line)
    f_out.write('))')
    f_in.close()
    f_out.close()

    btor = Boolector()
    (res, status, error_msg) = btor.Parse('check')
    result = btor.Sat()
    if result != 10:
        print(sys.argv[1] + ': PASS')
    else:
        print(sys.argv[1] + ': NOT PASS')
