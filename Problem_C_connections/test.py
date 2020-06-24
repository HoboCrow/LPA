import subprocess as sb
import os
import difflib

indir = r'./inputs'
outdir = r'./outputs'

for entry in os.scandir(indir):
    outfile = open(f"{outdir}/{entry.name}", "r")
    infile = open(f"{indir}/{entry.name}", "r")
    out = sb.check_output("./a.out", stdin=infile).decode("utf-8")
    result = difflib.ndiff(out, outfile.read())
    print("".join(result))
# print(f"Out:🍕 {out}")
