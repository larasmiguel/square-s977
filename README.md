# Packing 977 unit squares — candidate upper bound

**Contributor: Miguel Lara (independent).**

Proposed construction: 441 unit squares in a compact central 21×21 diamond rotated 45°, with 536 unrotated squares in four identical corner staircases (134 each). The exact construction gives:

\[s(977)\le 17+21/\sqrt{2}\approx31.8492424049<32.\]

This is a **candidate upper bound** submitted for independent review, not a claim of optimality or established priority over existing records.

## Files

- `certificates/n977_certificate.md` — mathematical construction and geometric non-overlap argument.
- `certificates/n977_coordinates.csv` — 977 numeric square centers and angles (radians), using the relaxed side 31.85 and the container's lower-left corner as the coordinate origin.
- `verification/verify_977.py` — independent-library numerical check of the **published CSV**, not a re-generation of it.
- `verification/n977_verification.json` — recorded checker output (includes machine-specific timing).
- `visualization/n977.svg` — illustration of the **same** relaxed 31.85 CSV coordinates.
- `SUBMISSION.md` — suggested record-report text and transparent AI disclosure.

## Reproduce

```bash
python3 -m pip install 'shapely>=2.1,<3'
python3 verification/verify_977.py
```

The checker reads the committed CSV; it never overwrites it. It uses floating-point polygon tests with a documented area tolerance, not symbolic or interval arithmetic. The exact construction is discussed separately in the certificate.

## Attribution and limitations

Geometric investigation and candidate construction: Miguel Lara, assisted by OpenAI ChatGPT in numerical experimentation, code generation, debugging, proof drafting, and documentation. Independent third-party review and comprehensive prior-art comparison remain outstanding.
