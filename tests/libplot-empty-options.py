#!/usr/bin/env python3
"""Check the real GNU libplot backend with no driver-specific options."""
import argparse
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser()
parser.add_argument("binary", type=Path)
args = parser.parse_args()
binary = args.binary.resolve()
with tempfile.TemporaryDirectory(prefix="pstoedit-test-") as directory:
    source = Path(directory) / "line.eps"
    output = Path(directory) / "line.svg"
    source.write_text("""%!PS-Adobe-3.0 EPSF-3.0
%%BoundingBox: 0 0 100 100
newpath 10 10 moveto 90 90 lineto 2 setlinewidth stroke
showpage
%%EOF
""")
    subprocess.run([str(binary), "-f", "plot-svg", str(source), str(output)],
                   check=True, timeout=30)
    root = ET.parse(output).getroot()
    assert root.tag == "{http://www.w3.org/2000/svg}svg", root.tag
    shapes = [element for element in root.iter()
              if element.tag.rsplit("}", 1)[-1] in {"path", "polyline", "line"}]
    assert shapes, "The known diagonal line must appear in the SVG output"
print("GNU libplot exports a line without driver-specific options")
