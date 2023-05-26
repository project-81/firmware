"""
A module for aggregating board tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task.manager import TaskManager

# internal
from local.boards.pico import PIOASM_DIR, BuildPioasm
from local.common import add_program_path
from local.jlink.gdbserver import jlink_gdbserver_task


def register_boards(manager: TaskManager, third_party: Path) -> bool:
    """Register all board tasks."""

    # Should just read top-level config directly.
    for board in ["relax_kit", "grand_central", "pi_pico"]:
        assert manager.register(jlink_gdbserver_task(board, third_party), [])

    manager.register(
        BuildPioasm("pioasm", third_party),
        ["third-party-clones", "build-ninja"],
    )
    add_program_path(
        "pioasm",
        third_party,
        *PIOASM_DIR,
        "build",
        "pioasm",
        update_path=True,
        local_bin=True
    )

    return True
