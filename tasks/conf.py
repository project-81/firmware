"""
A module for project-specific task registration.
"""

# built-in
from pathlib import Path
from typing import Dict

# third-party
from vcorelib.task import Phony
from vcorelib.task.manager import TaskManager

from local.boards import register_boards

# internal
from local.common import PATHS
from local.crosstool import register_crosstool
from local.gdbgui import register_gdbgui
from local.git import register_git
from local.jlink import register_jlink
from local.ninja import register_ninja
from local.node import register_node
from local.openocd import register_openocd
from local.python import register_python
from local.yambs import register_yambs


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

    # Set some paths here for convenience.
    PATHS["build"] = cwd.joinpath("build")
    PATHS["third-party"] = third_party

    for reg in [
        register_git,
        register_python,
        register_ninja,
        register_node,
        register_openocd,
        register_gdbgui,
        register_jlink,
        register_boards,
    ]:
        assert reg(manager, third_party), reg

    assert register_crosstool(manager, cwd)
    assert register_yambs(manager, cwd)

    manager.register(
        Phony("third-party-clones"),
        ["github-shallow-clone.Cherrg.gdbgui-fix_447"],
    )

    manager.register(Phony("deps"), ["toolchains"])

    del project

    return True
