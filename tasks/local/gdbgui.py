"""
A module for project-specific gdbgui tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessExecStreamed


def register_gdbgui(manager: TaskManager, third_party: Path) -> bool:
    """Register git tasks."""

    manager.register(
        SubprocessExecStreamed(
            "gdbgui",
            args="",
            program=str(
                third_party.joinpath(
                    "gdbgui", "build", "executable", "gdbgui.pex"
                )
            ),
        ),
        ["third-party-nox-gdbgui-build_executables_current_platform"],
    )

    return True
