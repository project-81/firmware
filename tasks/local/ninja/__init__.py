"""
A module implementing tasks for building ninja.
"""

# built-in
from pathlib import Path

# internal
from local.common import add_program_path
from local.configure import ThirdPartyConfigure

# third-party
from vcorelib.task.manager import TaskManager


def register_ninja(manager: TaskManager, third_party: Path) -> bool:
    """Register ninja targets."""

    # Build ninja.
    manager.register(
        ThirdPartyConfigure(
            "build-ninja",
            third_party,
            "--bootstrap",
            project="ninja",
            ninja="ninja",
            configure="configure.py",
        ),
        ["github-shallow-clone.ninja-build.ninja-v1.11.1"],
    )

    # A path to the ninja binary.
    add_program_path("ninja", third_party, "ninja", "ninja", update_path=True)

    return True
