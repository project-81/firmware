"""
A module for aggregating board tasks.
"""

# built-in
from pathlib import Path
from typing import Dict, Optional, Tuple

# third-party
from vcorelib.io import ARBITER
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin
from yambs.config import Config

# internal
from local.boards.pico import PIOASM_DIR, BuildPioasm
from local.common import add_program_path
from local.jlink.gdbserver import jlink_gdbserver_task
from local.prompts import manual_select


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


def select_board_app(
    boards: Dict[str, Dict[str, str]], **kwargs
) -> Optional[Tuple[str, Path]]:
    """Select a board and application."""

    # Select a board.
    board = manual_select("board", boards, default=kwargs.get("board", ""))
    if board is None:
        return None

    # Select an app.
    app = manual_select("app", boards[board], default=kwargs.get("app", ""))
    if app is None:
        return None

    return board, Path(boards[board][app])


class BoardAppMixin(SubprocessLogMixin):
    """A class mixin for facilitating board and application selection."""

    default_requirements = {"vmklib.init", "venv"}

    async def select_board_app(
        self, config: Config, deploy_json: bool = True, **kwargs
    ) -> Optional[Tuple[str, Path]]:
        """Select a board and application."""

        # Load 'deploy.json' if it's present.
        if deploy_json:
            if config.root.joinpath("deploy.json").is_file():
                kwargs.update(
                    ARBITER.decode(
                        config.root.joinpath("deploy.json"),
                        require_success=True,
                    ).data
                )

        # Load board->apps data.
        board_apps = config.directory("ninja_out").joinpath("board_apps.json")

        # Generate the file if it doesn't exist yet.
        if not board_apps.is_file():
            assert await self.exec("mk", "-C", str(config.root), "g")

        return select_board_app(
            ARBITER.decode(  # type: ignore
                board_apps, require_success=True
            ).data,
            **kwargs
        )
