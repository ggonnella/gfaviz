#!/usr/bin/env python3
"""
Create a GFA2 file from a PAF file
"""

import sys
import os
import argparse
import re

op = argparse.ArgumentParser(description=__doc__)
op.add_argument("filename")
op.add_argument('--version', action='version', version='%(prog)s 1.0')
op.add_argument('--verbose', action="store_true",
    help="Show state of processing to standard error")
op.add_argument('--nverbose', type=int, default=10000,
    help="Number of lines interval for verbose output")
op.add_argument('--limit', type=int,
    help="Limit the output to the first N segments and their edges")
op.add_argument('--query',
    help="Limit the output to the specified segment (implies --neighbours --skipnedges)")
op.add_argument('--neighbours', action="store_true",
    help="Output neighbours of query or limit-set segments")
op.add_argument('--skipnedges', action="store_true",
    help="Do not show edges between neighbours")
op.add_argument('--rmpfx', action="store_true",
    help="Remove prefix of read names")
opts = op.parse_args()

segments = set()
neighbours = set()

def rmpfx(name):
  return re.match(".*?/(.*)", name).group(1)

def print_segment(name, ln):
  elems = ["S"]
  elems.append(name)
  elems.append(str(ln))
  elems.append("*")
  print("\t".join(elems))

def print_edge(qname, qlen, qstart, qend, orient, tname, tlen, tstart, tend):
  elems = ["E"]
  elems.append("*")
  elems.append(qname + "+")
  elems.append(tname + orient)
  elems.append(str(qstart))
  qend_str = str(qend+1)
  if qend + 1 == qlen:
    qend_str += "$"
  elems.append(qend_str)
  elems.append(str(tstart))
  tend_str = str(tend+1)
  if tend + 1 == tlen:
    tend_str += "$"
  elems.append(tend_str)
  elems.append("*")
  print("\t".join(elems))

def process_with_query(query, qname, query_found, tname, neighbours):
  if qname == query:
    shall_print_edge = True
    if not query_found:
      print_segment(qname, qlen)
      query_found = True
    if tname not in neighbours:
      print_segment(tname, tlen)
      neighbours.add(tname)
  elif tname == query:
    shall_print_edge = True
    if not query_found:
      print_segment(tname, tlen)
      query_found = True
    if qname not in neighbours:
      print_segment(qname, qlen)
      neighbours.add(qname)
  else:
    shall_print_edge = False
  return query_found, shall_print_edge

def process_with_limit(place, qname, tname, segments, neighbours, opts):
  qname_in_segments = qname in segments
  tname_in_segments = tname in segments
  shall_print_edge = False
  if qname_in_segments and tname_in_segments:
    shall_print_edge = True
  elif qname_in_segments: # and not tname_in_segments
    if place > 0: # add t
      print_segment(tname, tlen)
      segments.add(tname)
      place -= 1
      shall_print_edge = True
    elif opts.neighbours:
      shall_print_edge = True
      if not tname in neighbours:
        print_segment(tname, tlen)
        neighbours.add(tname)
  elif tname_in_segments: # and not qname_in_segments
    if place > 0: # add q
      print_segment(qname, qlen)
      segments.add(qname)
      shall_print_edge = True
      place -= 1
    elif opts.neighbours:
      shall_print_edge = True
      if not qname in neighbours:
        print_segment(qname, qlen)
        neighbours.add(qname)
  else: # not tname_in_segments and not qname_in_segments
    if place > 1: # add t and q
      print_segment(tname, tlen)
      segments.add(tname)
      print_segment(qname, qlen)
      segments.add(qname)
      shall_print_edge = True
      place -= 2
    elif place == 1: # add q only
      print_segment(qname, qlen)
      segments.add(qname)
      place = 0
      if opts.neighbours:
        shall_print_edge = True
        if not tname in neighbours:
          print_segment(tname, tlen)
          neighbours.add(tname)
    elif opts.neighbours and not opts.skipnedges \
        and tname in neighbours and qname in neighbours:
      shall_print_edge = True
  return place, shall_print_edge

with open(opts.filename) as f:
  line_n = 0
  place = opts.limit
  query_found = False
  for line in f:
    line_n += 1
    elems = line.rstrip().split("\t")
    qname = elems[0]
    if opts.rmpfx:
      qname = rmpfx(qname)
    qlen = int(elems[1])
    qstart = int(elems[2])
    qend = int(elems[3])
    orient = elems[4]
    tname = elems[5]
    if opts.rmpfx:
      tname = rmpfx(tname)
    tlen = int(elems[6])
    tstart = int(elems[7])
    tend = int(elems[8])
    if opts.verbose and len(segments) % opts.nverbose == 0:
      sys.stderr.write("{} lines processed; ".format(line_n))
      sys.stderr.write("{} segments\n".format(len(segments)))
    if opts.query:
      query_found, shall_print_edge = \
        process_with_query(opts.query, qname, query_found, tname, neighbours)
    else:
      place, shall_print_edge = \
          process_with_limit(place, qname, tname, segments, neighbours, opts)
    if shall_print_edge:
      print_edge(qname, qlen, qstart, qend, orient, tname, tlen, tstart, tend)
