#!/usr/bin/env python3
"""
Add fragments to a GFA2 file using alignments from a SAM file
"""

import sys
import os
import argparse
import re
import gfapy

op = argparse.ArgumentParser(description=__doc__)
op.add_argument("filenamesam")
op.add_argument("filenamegfa")
op.add_argument('--version', action='version', version='%(prog)s 1.0')
opts = op.parse_args()

def output_segment(sname, slen):
  out = []
  out.append("S")
  out.append(sname)
  out.append(slen)
  out.append("*")
  print("\t".join(out))

def output_header():
  out = []
  out.append("H")
  out.append("VN:Z:2.0")
  print("\t".join(out))

def parse_cigar(cigar_string):
  cigar = []
  for m in re.finditer("([0-9]+)([MIDNSHPX=])", cigar_string):
    cigar.append((int(m.group(1)), m.group(2)))
  return cigar

def cigar_to_cigarstr(cigar):
  s = []
  for n,o in cigar:
    # remove padding
    if n > 0 and o in ["M", "D", "I", "P"]:
      s.append("{}{}".format(n, o))
  return "".join(s)

def compute_rlen(cigar):
  rlen = 0
  for oplen, opcode in cigar:
    if opcode in ["M", "D", "N", "=", "X"]:
      rlen += oplen
  return rlen

def compute_qstart(cigar):
  qstart = 0
  for oplen, opcode in cigar:
    if opcode in ["S", "H"]:
      qstart += oplen
    else:
      break
  return qstart

def compute_qlen(cigar):
  qlen = 0
  started = False
  for oplen, opcode in cigar:
    if opcode in ["M", "I", "=", "X"]:
      qlen += oplen
      started = True
    elif started:
      break
  return qlen

def compute_qdollar(cigar):
  return cigar[-1][1] not in ["S", "H"]

def add_fragment(gfa, qname, flag, rname, pos, cigar_str, segment_lengths):
  out = []
  out.append("F")
  out.append(rname)
  if (int(flag) & 16):
    direction = "-"
  else:
    direction = "+"
  qname_dir = "{}{}".format(qname, direction)
  out.append(qname_dir)
  out.append(pos)
  cigar = parse_cigar(cigar_str)
  cigar_str = cigar_to_cigarstr(cigar)
  rlen = compute_rlen(cigar)
  rpos_str = str(int(pos) + rlen)
  if int(pos) + rlen == segment_lengths[rname]:
    rpos_str += "$"
  out.append(rpos_str)
  qstart = compute_qstart(cigar)
  out.append(str(qstart))
  qlen = compute_qlen(cigar)
  qend = qstart + qlen
  qend_str = str(qend)
  if compute_qdollar(cigar):
    qend_str += "$"
  out.append(qend_str)
  out.append(cigar_str)
  line = "\t".join(out)
  gfa.add_line(line)

def process_end_of_header(gfa, segments_to_save):
  for sname in gfa.segment_names:
    if sname not in segments_to_save:
      gfa.rm(sname)

gfa = gfapy.Gfa.from_file(opts.filenamegfa)
output_header()
segment_lengths = {}
segments_to_save = set()
was_in_header = True
with open(opts.filenamesam) as f:
  for line in f:
    elems = line.rstrip().split("\t")
    if line[0] == "@":
      if elems[0] == "@SQ":
        sname = elems[1][3:]
        slen = elems[2][3:]
        #output_segment(sname, slen)
        segments_to_save.add(sname)
        segment_lengths[sname] = int(slen)
    else:
      if was_in_header:
        process_end_of_header(gfa, segments_to_save)
        was_in_header = False
      qname = elems[0]
      flag = elems[1]
      rname = elems[2]
      pos = elems[3]
      cigar = elems[5]
      if rname not in segment_lengths:
        raise RuntimeError(
            "Query not found {}, check the SAM header".format(qname))
      add_fragment(gfa, qname, flag, rname, pos, cigar, segment_lengths)

print(gfa)
