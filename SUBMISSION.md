# Draft submission to The Squares Project

**Credit name.** Miguel Lara, independent contributor. GitHub: https://github.com/larasmiguel.

**Claim.** Proposed upper bound \(s(977)\le17+21/\sqrt2\approx31.8492424049<32\), constructed with 441 central squares rotated 45° and 536 unrotated corner squares (134 per corner). This is better than the elementary side-32 grid bound; the best existing *published* bound at n=977 has not been independently established.

**Certificate.** [`certificates/n977_certificate.md`](certificates/n977_certificate.md) is the exact geometric construction and proof argument. [`certificates/n977_coordinates.csv`](certificates/n977_coordinates.csv) and [`visualization/n977.svg`](visualization/n977.svg) are numerical artifacts at side 31.85. Pin this submission to the full GitHub commit SHA from the repository after the commit is made.

**Checking.** One numerical checker: `python3 verification/verify_977.py`. Arithmetic: IEEE-754 double precision and Shapely geometric predicates, numerical overlap-area tolerance 1e-11. Reports 977 squares, 441 rotated, 536 unrotated, 0 outside the 31.85 container, and 0 overlapping pairs above tolerance. See [`verification/n977_verification.json`](verification/n977_verification.json) for versions, number of candidate pairs tested, platform and measured runtime. This is *one* checker, not multiple independent certifications.

**What the checkers share.** The underlying symmetry/coordinate description. The numerical checker reads a published CSV and uses a third-party geometry library; the mathematical certificate is a separate geometric argument, but both were prepared in the same AI-assisted investigation.

**Not done.** Independent external verification, formal proof assistant or interval-arithmetic certification, exhaustive prior-art comparison, or proof of optimality.

**Lineage and credit.** Developed by Miguel Lara in a computational investigation of a rotated central diamond and four corner staircases; specific precedents remain to be researched. Please credit "Miguel Lara" for the submitted construction, subject to editorial priority checks.

**AI assistance.** I used OpenAI ChatGPT extensively for exploratory geometry, generating and debugging C++/Python, writing verification code, and drafting mathematical reasoning and documentation. I directed the investigation and requested independent checks. This disclosure should not be interpreted as an external peer review.
