"""
A module for aggregating board tasks.
"""

# built-in
from pathlib import Path

# internal
from local.boards.relax_kit import register_relax_kit

# third-party
from vcorelib.task.manager import TaskManager


def register_boards(manager: TaskManager, third_party: Path) -> bool:
    """Register all board tasks."""

    result = register_relax_kit(manager, third_party)
    return result
