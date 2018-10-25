#!/usr/bin/env python3

import gfapy
import re

# TODO: make this paths parametric!
fname = "../paired_reads.spades_meta/assembly_graph_with_scaffolds.gfa"
bins_pfx = "../paired_reads.spades_meta.maxbin/pe_sim_10."
nbins = 10

nodes = {}
gfa1 = gfapy.Gfa()

with open(fname) as f:
  for line in f:
    elems = line.rstrip().split("\t")
    # separately parse P lines, as they are unfortunately non-standard
    # (as they may contain single oriented segment names)
    if elems[0] == "P":
      nodenum = re.search("NODE_(\d+)_", elems[1]).group(1)
      if not nodenum in nodes:
        nodes[nodenum] = []
      for seg_orient in elems[2].split(","):
        nodes[nodenum].append(seg_orient[:-1])
    else:
      gfa1.append(line)

gfa2 = gfa1.to_gfa2()
print(gfa2)

for b in range(1,nbins+1):
  # this will work for 1000 bins at most, but is OK for our purpose
  bin_fname = bins_pfx + "%03d" % b + ".fasta"
  bin_segments = set()
  with open(bin_fname) as f:
    for line in f:
      if line[0] == ">":
        node = line[1:-1]
        nodenum = re.search("NODE_(\d+)_", node).group(1)
        for n in nodes[nodenum]:
          bin_segments.add(n)
  print("U\tBin_{}\t{}".format(str(b), " ".join(bin_segments)))

