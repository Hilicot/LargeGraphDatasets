#!/bin/bash

filename="snap-wiki.txt"
num_nodes=1791489
num_edges=28511807

cd raw
echo "# Undirected graph (each unordered pair of nodes is saved once): $filename" > /tmp/tmp$$
echo "# ..Fake header.." >> /tmp/tmp$$
echo "# Nodes: $num_nodes Edges: $num_edges" >> /tmp/tmp$$
echo "# NodeId        NodeId" >> /tmp/tmp$$
cat $filename >> /tmp/tmp$$
mv /tmp/tmp$$ $filename