#!/usr/bin/env python3
"""Run ngspice -b on testcase/*.cir and write DC node voltages to standardoutput/."""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

CODE_DIR = Path(__file__).resolve().parents[1]
TESTCASE_DIR = CODE_DIR / "testcase"
OUT_DIR = CODE_DIR / "standardoutput"

GROUND = {"0", "GND"}
NODE_TOKEN = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def discover_nodes(cir_path: Path) -> list[str]:
    """Match NodeMap: first-seen order of non-ground nodes in element lines."""
    seen: set[str] = set()
    order: list[str] = []

    for raw in cir_path.read_text().splitlines():
        line = raw.split("*", 1)[0].strip()
        if not line or line.startswith("."):
            continue
        parts = line.split()
        if not parts:
            continue
        name = parts[0]
        if name[0] not in "RLCVIQLDM":
            continue
        for token in parts[1:]:
            if token.startswith("("):
                continue
            if token.upper() in {"D", "NPN", "PNP", "NMOS", "PMOS"}:
                break
            if not NODE_TOKEN.match(token):
                continue
            if token in GROUND or token in seen:
                continue
            seen.add(token)
            order.append(token)
    return order


def has_command(cir_path: Path, cmd: str) -> bool:
    text = cir_path.read_text()
    return re.search(rf"^\s*\.{cmd}\b", text, re.MULTILINE | re.IGNORECASE) is not None


def run_ngspice(cir_path: Path) -> tuple[int, str]:
    proc = subprocess.run(
        ["ngspice", "-b", str(cir_path.name)],
        cwd=str(cir_path.parent),
        capture_output=True,
        text=True,
    )
    return proc.returncode, proc.stdout + proc.stderr


def convergence_unreliable(ngspice_text: str) -> bool:
    markers = (
        "singular matrix",
        "stepping failed",
        "Simulation interrupted",
    )
    return any(m in ngspice_text for m in markers)


def parse_node_voltages(ngspice_text: str) -> dict[str, float]:
    voltages: dict[str, float] = {}
    in_table = False
    for line in ngspice_text.splitlines():
        if "Node" in line and "Voltage" in line:
            in_table = True
            continue
        if in_table and line.strip().startswith("----"):
            continue
        if in_table and line.strip().startswith("Source"):
            break
        if not in_table:
            continue
        stripped = line.strip()
        if not stripped or stripped.startswith("----"):
            continue
        parts = stripped.split()
        if len(parts) < 2:
            continue
        node, value_s = parts[0], parts[1]
        if "#" in node:
            continue
        try:
            voltages[node.lower()] = float(value_s)
        except ValueError:
            continue
    return voltages


def format_voltage(value: float) -> str:
    if value == 0.0:
        return "0"
    s = f"{value:.6g}"
    if "e" in s or "E" in s:
        return s
    if "." in s:
        s = s.rstrip("0").rstrip(".")
    return s


def write_standard_out(
    cir_path: Path,
    voltages: dict[str, float],
    dest: Path,
    *,
    error_note: str | None = None,
) -> None:
    nodes = discover_nodes(cir_path)
    lines: list[str] = []
    if error_note:
        lines.append(f"* {error_note}")

    if has_command(cir_path, "OP"):
        lines.append("---- DC Operating Point ----")
        for node in nodes:
            key = node.lower()
            if key not in voltages:
                continue
            lines.append(f"V({node}) = {format_voltage(voltages[key])}")

    if has_command(cir_path, "PRINT"):
        lines.append("---- DC Print ----")
        for node in nodes:
            key = node.lower()
            if key not in voltages:
                continue
            lines.append(f"V({node}) = {format_voltage(voltages[key])}")

    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text("\n".join(lines) + ("\n" if lines else ""))


def main() -> int:
    cir_files = sorted(TESTCASE_DIR.glob("*.cir"))
    if not cir_files:
        print("No .cir files in testcase/", file=sys.stderr)
        return 1

    for cir_path in cir_files:
        dest = OUT_DIR / f"{cir_path.stem}.out"
        note: str | None = None
        code, output = run_ngspice(cir_path)
        voltages = parse_node_voltages(output)

        if code != 0 or not voltages:
            print(f"{cir_path.name}: ngspice failed or no OP table", file=sys.stderr)
            write_standard_out(
                cir_path,
                {},
                dest,
                error_note="ngspice could not complete DC OP on this netlist",
            )
            continue

        if convergence_unreliable(output):
            print(f"{cir_path.name}: unreliable OP, skipping voltages", file=sys.stderr)
            write_standard_out(
                cir_path,
                {},
                dest,
                error_note="ngspice DC OP did not converge reliably",
            )
            continue

        write_standard_out(cir_path, voltages, dest, error_note=note)
        print(f"Wrote {dest.relative_to(CODE_DIR)}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
