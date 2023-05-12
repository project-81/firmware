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
from configure_common import register_configure
from git_common import register_git
from python_common import register_python


def register(
    manager: TaskManager,
    project: str,
    cwd: Path,
    substitutions: Dict[str, str],
) -> bool:
    """Register project tasks to the manager."""

    third_party = cwd.joinpath("third-party")
    register_git(manager, third_party)
    register_python(manager, third_party)
    register_configure(manager, third_party, substitutions)

    manager.register(
        Phony("third-party-clones"),
        [
            "github-shallow-clone.Cherrg.gdbgui-fix_447",
            "github-shallow-clone.nodejs.node-v20.1.0",
            "github-shallow-clone.ninja-build.ninja-v1.11.1",
        ],
    )

    manager.register(
        Phony("gdbgui"),
        ["third-party-nox-gdbgui-build_executables_current_platform"],
    )

    manager.register(Phony("deps"), ["gdbgui"])

    del project
    del substitutions

    return True
