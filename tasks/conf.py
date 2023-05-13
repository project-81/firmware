"""
A module for project-specific task registration.
"""

# built-in
from pathlib import Path
from typing import Dict

# third-party
from vcorelib.task import Phony
from vcorelib.task.manager import TaskManager

# internal
from local.crosstool import register_crosstool
from local.gdbgui import register_gdbgui
from local.git import register_git
from local.ninja import register_ninja
from local.node import register_node
from local.openocd import register_openocd
from local.python import register_python


def register(
    manager: TaskManager,
    project: str,
    cwd: Path,
    substitutions: Dict[str, str],
) -> bool:
    """Register project tasks to the manager."""

    third_party = cwd.joinpath("third-party")
    third_party.mkdir(parents=True, exist_ok=True)
    third_party.joinpath("tarballs").mkdir(parents=True, exist_ok=True)

    register_git(manager, third_party)
    register_python(manager, third_party)
    register_ninja(manager, third_party)
    register_node(manager, third_party)
    register_openocd(manager, third_party)
    register_gdbgui(manager, third_party)
    register_crosstool(manager, cwd)

    manager.register(
        Phony("third-party-clones"),
        [
            "github-clone.picolibc.picolibc",
            "github-clone.openocd-org.openocd",
            "github-shallow-clone.Cherrg.gdbgui-fix_447",
            "github-shallow-clone.nodejs.node-v20.1.0",
            "github-shallow-clone.ninja-build.ninja-v1.11.1",
        ],
    )

    manager.register(Phony("deps"), ["toolchains"])

    del project

    return True
