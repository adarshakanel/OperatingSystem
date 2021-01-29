#!/bin/env python3

from resource import getrusage as resource_usage, RUSAGE_SELF
from time import time as timestamp
import subprocess, re, time, resource, os, sys, string
import functools

def dbg(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        print(f"dbg: {func.__name__}({args},{kwargs})")
        return_value = func(*args, **kwargs)
        print(f"dbg: {func.__name__}() -> {return_value}")
        return return_value
    return wrapper

mypath = os.path.dirname(os.path.realpath(sys.argv[0]))
#print(f"mypath = {mypath}")
print()

# remove lines with no sequence
def remove_empty_lines(lines):
    result = []
    for line in lines:
        if len(line.strip()) > 0:
            result.append(line)
    return result

def last_2_lines( all_lines):
    ''' return last 2 lines, if not enough lines, pad with empty lines at start'''
    return (["",""] + all_lines) [-2:]
    
def extract_single_number(line):
    s = line.strip()
    s = map(lambda c: c if c.isdigit() else ' ', s)
    s = "".join(s)
    toks = s.split()
    if len(toks) != 1 : return None
    try:
        res = int(toks[0])
    except:
        return None
    return res
    

# see if the actual output in all_lines[] matches the correct output
# returns (line1,line2), msg, [last2lines]
# (0,0) if no match
# (1,0) or (0,1) for 1 line match
# (1,1) if both lines match

def match_output( all_lines, correct_output):
    assert len(correct_output) == 2
    # make sure we have at least two lines (pad with empty lines)
    last2lines = (["",""] + all_lines)[-2:]
    line1, line2 = last2lines[-2], last2lines[-1]

    # extract a single number
    num1, num2 = extract_single_number(line1), extract_single_number(line2)

    score1, score2, msg = 1, 1, ""
    if num1 != correct_output[0]: score1,msg = 0, msg+ "wrong line 1"
    if num2 != correct_output[1]: score2,msg = 0, msg+" wrong line2"

    return (score1, score2), msg, last2lines

class Test:
    def __init__(self, weights, timings, fname, argv, correct_output):
        self.weights = weights
        self.fname = fname
        self.argv = [str(argv)] if argv is not None else []
        self.timings = timings
        self.correct_output = correct_output

        self.maxweight = max(self.weights)
        self.maxtime = self.timings[-1][0]

        self.act_time = -1
        self.finished = False
        self.run_comment = ""

        lt, lp = 0, 0
        for t,p in self.timings:
            assert t >= lt
            assert p >= lp
            lt, lp = t, p
        assert lp == max(self.weights)

# ======================================================================
#                       tests
tests = []
'''
./fatsim < ../marking-files/fatsim0.txt
blocks in largest file: 5
blocks not in any file: 10

 ./fatsim < ../marking-files/fatsim1.txt
blocks in largest file: 0
blocks not in any file: 51

./fatsim < ../marking-files/fatsim2.txt
blocks in largest file: 0
blocks not in any file: 102

./fatsim < ../marking-files/fatsim3.txt
blocks in largest file: 12
blocks not in any file: 25

$ ./fatsim < ../marking-files/fatsim4.txt
blocks in largest file: 3272
blocks not in any file: 19338

$ ./fatsim < ../marking-files/fatsim5.txt
blocks in largest file: 12778
blocks not in any file: 167400

./fatsim < ../marking-files/fatsim6.txt
blocks in largest file: 24047
blocks not in any file: 349902

'''
tests.append( Test(
    (0,0,0), ((11,0), (12,0), (13,0)), "fatsim0.txt", None, [5,10]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim1.txt", None, [0,51]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim2.txt", None, [0,102]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim3.txt", None, [12,25]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim4.txt", None, [3272,19338]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim5.txt", None, [12778,167400]))

tests.append( Test(
    (5,2,0), ((11,1), (13,2), (15,5)), "fatsim6.txt", None, [24047,349902]))

# ======================================================================

def silent_run( pname, test):
    fname = mypath + "/" + test.fname
    with open(fname, "rb") as f:
        inp = f.read()
    start_time = time.perf_counter()
    try:
        sub = subprocess.run(
            [pname] + test.argv,
            input = inp,
            capture_output=True,
            timeout=test.maxtime
        )
        test.finished = True
        if sub.stdout:
            lines = sub.stdout.decode().split("\n")
        else:
            lines = []
    except subprocess.TimeoutExpired as e:
        test.finished = False
        if e.stdout:
            lines = e.stdout.decode().split("\n")
        else:
            lines = []

    test.act_time = time.perf_counter() - start_time
    test.all_lines = lines
    stripped_lines = remove_empty_lines(test.all_lines)
    test.match_scores, test.run_comment, test.last2lines = \
        match_output( stripped_lines, test.correct_output)

def run2( pname, test):
    print(f"+-------- {pname} {''.join(test.argv)} < {test.fname} " \
          f"-------------- out of {test.maxweight} pts")
    print("|")
    silent_run(pname, test)

    lines = remove_empty_lines(test.all_lines)
    if len(lines) < 6:
        print("\n".join(["|  "+line for line in lines]))
    else:
        print("\n".join(["|  "+line for line in lines[:3]]))
        print(f".... skipped {len(lines)-6} lines")
        print("\n".join(["|  "+line for line in lines[-3:]]))
    print("|")
    if not test.finished:
        print(f"+---- !!! program killed after {test.maxtime}s\n")
    else:
        print(f"+---- time: {test.act_time:.5f}s\n")

# returns score of the test
# (total_score, line1_match, line2_match, timing_comment)
def score_test(t):
    if not t.finished: return (0,False,False,"dnf")
    assert len(t.match_scores) == 2
    assert sum(t.match_scores) in range(0,3)
    s1 = t.weights[2-sum(t.match_scores)]
    timing_comment = f"< {t.timings[0][0]}s"
    deduction = 0
    for st in t.timings:
        if t.act_time > st[0]:
            timing_comment = f"> {st[0]}s -{st[1]}pt"
            deduction = st[1]
    score = max(s1 - deduction,0)
    # expl = f"+{s1},-{deduction}"
    return (score,t.match_scores[0]==1,t.match_scores[1]==1,timing_comment)

def trim_output(str, w):
    if len(str) < w: return str + ' ' * (w - len(str))
    return str[:w-4] + " ..."
    
def print_test_table_(tests):
    print()
    twidt = 100
    print("=" * twidt)
    print("                | timing     |       |                 |    | actual output")
    print("test            | comment    | score | correct output  | == | last 2 lines")
    print("=" * twidt)
    total = 0
    for test in tests: total += test.maxweight

    marks, printDiv = 0, False
    for test in tests:
        if printDiv:
            print("-" * twidt)
        printDiv = True

        print(f"{test.fname:<15}", end="")
        cell = f"{test.act_time:<.2f}s"
        print(f" | {cell:10}", end="")

        total_score, l1match, l2match, timing_comment = score_test(test)
        print(f" | {total_score}/{test.maxweight}pt", end="")
        marks += total_score

        cline1 = str(test.correct_output[0])
        cline2 = str(test.correct_output[1])
        oline1 = test.last2lines[0].strip()
        oline2 = test.last2lines[1].strip()
            
        outw = 15
        cline1 = trim_output(cline1, outw)
        cline2 = trim_output(cline2, outw)
        l1match_str = "==" if l1match else " x"
        outw = 45
        oline1 = trim_output(oline1, outw)
        oline2 = trim_output(oline2, outw)
        l2match_str = "==" if l2match else " x"
        
        print(f" | {cline1} | {l1match_str} | {oline1}", end="")

        print()
        print(f"{'':15}", end="")
        print(f" | {timing_comment:10} |      ", end="")
        print(f" | {cline2} | {l2match_str} | {oline2}", end="")

        print()
    print("=" * twidt)
    print(f"Total = {marks} / {total} = {100.0*marks/total:.2f}%")
    print("=" * twidt)

def print_test_table(tests, fd=sys.stdout):
    saved_stdout = sys.stdout
    sys.stdout = fd
    try:
        print_test_table_(tests)
    finally:
        sys.stdout = saved_stdout

import json
def save_scores_to_json(tests, fname):
    js = {}
    total_score = 0
    for i in range(len(tests)):
        test = tests[i]
        details = {}
        score, l1match, l2match, timing_comment = score_test(test)
        details["score"] = score
        details["line1"] = l1match
        details["line2"] = l2match
        details["timing_comment"] = timing_comment
        details["time"] = test.act_time
        details["finished"] = test.finished
        total_score += score
        js[f"test{i}"] = details
    js["totalscore"] = total_score
    with open(fname, "w") as fd:
        json.dump(js, fd, indent=4)

def main_single_exe(pname):
    for test in tests:
        run2( pname, test)

    print_test_table(tests)

    details_fname = "q2.details.txt.pavol"
    with open(details_fname, "w") as fd:
        print_test_table(tests,fd)

    scores_fname = "q2.details.json.pavol"
    save_scores_to_json(tests, scores_fname)

def main_select_tests(argv):
    assert len(argv) > 2
    indices = []
    for t in argv[1].split(","):
        try:
            parts = list(map(int,t.split("-")))
            if len(parts) > 2 or len(parts) < 1: raise ""
            if len(parts) == 1: indices.append(int(parts[0]))
            else:
                n1, n2 = int(parts[0]), int(parts[1])
                indices.extend(range(n1-1, n2))
        except:
            print(f"Could not understand '{t}' in '{argv[1]}'")
            raise
            sys.exit(-1)
    for i in indices:
        if i < 0 or i >= len(tests):
            print(f"Invalid index '{i+1}'")
            sys.exit(-1)
    print("test indices:", indices)

    executables = argv[2:]
    maxexename = 20
    for pname in executables:
        pnamestr = ""
        if len(executables) > 1:
            if len(pname) > 20:
                pnamestr = pname[:20] + "..."
            else:
                pnamestr = pname.just(23)
        for tind in indices:
            t = tests[tind]
            print(f"{pnamestr} {t.fname:} {''.join(t.argv)} {str(t.correct_output)}", end="")
            sys.stdout.flush()
            silent_run(pname, t)
            score, l1match, l2match, timing_comment = score_test(t)
            print(f" {t.act_time:.3f}s {'killed' if not t.finished else ''}", end="")
            print(f"{score}/{t.maxweight}pt [{timing_comment}]")
            print("  " + "\n  ".join(t.last2lines))

def usage():
    print("Available tests:")
    for i in range(len(tests)):
        t = tests[i]
        print(f"{i:2}: {t.maxweight:3}pt {t.fname:10} {str(t.argv):10} {str(t.correct_output)}")
    print("1) run all tests on a single executable:")
    print("   $ grade.py ./a.out")
    print("3) run select tests on multiple executables:")
    print("   $ grade.py [1,2,5] ./a.out")
    print("   $ grade.py [1,5] ./*/a.out")
    print("   $ grade.py [5] ./*/a.out")
    sys.exit(-1)
        

def main(argv):
    if len(argv) < 2:
        usage()
    elif len(argv) == 2:
        main_single_exe(argv[1])
    else:
        main_select_tests(argv)

if __name__ == '__main__':

    main(sys.argv)



