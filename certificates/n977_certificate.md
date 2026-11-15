# Exact construction certificate — 977 unit squares

This describes a **candidate upper bound**, not a lower bound or optimality proof. All squares have side length 1.

## Side length

Let \(R=21/\sqrt 2\) and \(L=17+R\). Then
\[
  L=17+21/\sqrt2\approx31.8492424049<32,
\]
since \(21/\sqrt 2<15\).

## Central diamond: 441 squares

Take a compact \(21\times21\) array of axis-aligned unit squares, rotate the *whole array* counter-clockwise by \(45^\circ\), and center it at \((L/2,L/2)\). In index notation, for \(i,j\in\{0,\dots,20\}\), with \(u=i-10\), \(v=j-10\), place a unit square with angle \(\pi/4\) and center
\[
\left(L/2+(u-v)/\sqrt2,\quad L/2+(u+v)/\sqrt2\right).
\]
Its union is precisely the closed diamond \(|x-L/2|+|y-L/2|\le R\). The diamond lies inside the container: \(R\le L/2\), because \(R\le17\).

## Corner staircases: 536 squares

In the bottom-left corner, place axis-aligned unit squares with lower-left coordinate \((c,r)\), where \(0\le r<15\), \(0\le c<w_r\), and the row widths are

```text
r:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
w_r:15 15 14 13 12 11 10  9  8  7  6  5  4  3  2
```

These widths sum to 134. All their cells satisfy \(c+r+2\le17\), so their interiors lie in \(x+y<17\). The central diamond lies in \(x+y\ge L-R=17\) in the bottom-left region (indeed, its entire footprint satisfies \(x+y\ge L-R\)). Thus the lower-left staircase and diamond have disjoint interiors.

Produce the other three staircases using rotations of \(90^\circ\), \(180^\circ\), and \(270^\circ\) about \((L/2,L/2)\). By rotation, each remains disjoint from the central diamond. Each corner staircase occupies at most 15 units in either direction from its corner, whereas \(L/2>15\), so the four staircases occupy four disjoint corner quadrants and cannot intersect each other. Within each staircase, the grid cells are disjoint in their interiors.

The count is
\[
21^2+4(134)=441+536=977.
\]
Every square lies inside \([0,L]^2\), and interiors are pairwise disjoint. Therefore
\[
\boxed{s(977)\le17+21/\sqrt2<32}.
\]

**Numerical artifact distinction:** the accompanying CSV and SVG use the slightly larger side \(L'=31.85\) to provide a floating-point numerical witness. The above exact construction uses \(L=17+21/\sqrt2\). The same row pattern works at both side lengths. The CSV is *not* an exact-arithmetic certificate.

**Review status:** this proof has been prepared with AI assistance and has not been independently peer-reviewed. Novelty relative to previously published constructions has not been established.
