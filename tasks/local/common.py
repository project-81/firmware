"""
Utilities common to multiple project tasks.
"""

# built-in
from os import environ, pathsep
from pathlib import Path
from typing import Dict

PATHS: Dict[str, Path] = {}


def add_path(path: Path) -> None:
    """Add to the system path variable."""

    str_path = str(path)
    if str_path not in environ["PATH"]:
        environ["PATH"] = str_path + pathsep + environ["PATH"]


def add_program_path(
    program: str, third_party: Path, *parts: str, update_path: bool = False
) -> None:
    """Register a path to a program."""

    prog = third_party.joinpath(*parts).resolve()
    assert program not in PATHS, prog

    if update_path:
        add_path(prog.parent)

    PATHS[program] = prog


def program_str(program: str) -> str:
    """Get a string path to a program."""
    return str(PATHS[program])
