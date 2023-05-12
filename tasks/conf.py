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
from git_common import register_git
from python_common import ThirdPartyEditable


def register(
    manager: TaskManager,
    project: str,
    cwd: Path,
    substitutions: Dict[str, str],
) -> bool:
    """Register project tasks to the manager."""

    third_party = cwd.joinpath("third-party")
    register_git(manager, third_party)

    manager.register(
        Phony("third-party-clones"),
        ["github-shallow-clone.Cherrg.gdbgui.fix_447"],
    )

    manager.register(
        ThirdPartyEditable("third-party-editable-{project}", third_party),
        ["third-party-clones"],
    )

    manager.register(Phony("deps"), ["third-party-editable-gdbgui"])

    del project
    del substitutions

    return True
