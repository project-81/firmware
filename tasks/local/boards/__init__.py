"""
A module for aggregating board tasks.
"""

# built-in
from pathlib import Path

# internal
from local.jlink.gdbserver import jlink_gdbserver_task

# third-party
from vcorelib.task.manager import TaskManager


def register_boards(manager: TaskManager, third_party: Path) -> bool:
    """Register all board tasks."""

    # Should just read top-level config directly.
    for board in ["relax_kit", "grand_central"]:
        assert manager.register(jlink_gdbserver_task(board, third_party), [])

    return True
